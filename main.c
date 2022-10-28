#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <pthread.h>
#include <semaphore.h>

#include "main.h"
#include "linked_list.h"
#include "queue.h"

/* global variables */

int customersLeft = 0;

sem_t clerkSem;

Queue *econ_head = NULL;
Queue *buis_head = NULL;

//struct timeval init_time; // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.
//double overall_waiting_time; //A global variable to add up the overall waiting time for all customers, every customer add their own waiting time to this variable, mutex_lock is necessary.
//int queue_length[NQUEUE];// variable stores the real-time queue length information; mutex_lock needed
int econQueueLength = 0;
int buisQueueLength = 0;

/* Other global variable may include: 
 1. condition_variables (and the corresponding mutex_lock) to represent each queue; 
 2. condition_variables to represent clerks
 3. others.. depend on your design
 */
pthread_mutex_t econMutex;
pthread_mutex_t buisMutex;
pthread_mutex_t customerCountMutex;

pthread_cond_t econCond;
pthread_cond_t buisCond;


int main(int argc, char **argv)
{
    pthread_t custThread[10]; // TODO - what is the max? How to create dynamically?
    pthread_t clerkThread[5];

    pthread_mutex_init(&econMutex, NULL);
    pthread_mutex_init(&buisMutex, NULL);

    pthread_mutex_init(&customerCountMutex, NULL);

    pthread_cond_init(&econCond, NULL);
    pthread_cond_init(&buisCond, NULL);

    int NCustomers;
    int NClerks = 5;

    sem_init(&clerkSem, 0, NClerks);


    if(argc == 1)
    {
        printf("Input file required\n");
        exit(1);
    }
    else if(argc > 2)
    {
        printf("Too many arguments\n");
        exit(1);
    }

    FILE* custFile = fopen(argv[1],"r");
    if(custFile == NULL)
    {
        printf("Error - failed to open file\n");
        return -1;
    }
    else
    {
        printf("file opened\n");
    }

    fscanf(custFile, "%d\n", &NCustomers);

    customersLeft = NCustomers;

    printf("number of customers is %d\n\n", NCustomers);

    for(int i = 0; i < NCustomers; i++)
    {
        int user_id, class_type,  arrival_time, service_time;
        fscanf(custFile, "%d:%d,%d,%d\n", &user_id, &class_type, &arrival_time, &service_time);
        add_newCust(user_id, class_type, arrival_time, service_time);

        int* user_id_ptr = malloc(sizeof(int));
        *user_id_ptr = user_id;

        if(pthread_create(&custThread[i], NULL, &customer_entry, user_id_ptr) != 0)
        {
            printf("Error creating customer thread %d\n", i);
        }
    }

    printList();

    for(int i = 0; i < NCustomers; i++)
    {
        if(pthread_join(custThread[i], NULL) != 0)
        {
            printf("Error joining customer thread %d\n", i);
        }
    }

    pthread_mutex_destroy(&econMutex);
    pthread_mutex_destroy(&buisMutex);

    pthread_mutex_destroy(&customerCountMutex);

    pthread_cond_destroy(&econCond);
    pthread_cond_destroy(&buisCond);

    sem_close(&clerkSem);

    // calculate the average waiting time of all customers
    return 0;
}

// function entry for customer threads
void * customer_entry(void* cust_id_ptr)
{
    int cust_id = *(int*)cust_id_ptr;
    int class = getClassType(cust_id);
    int arrival_time = getArrivalTime(cust_id);
    int service_time = getServiceTime(cust_id);

    // wait for customer to arrive
    usleep(arrival_time * 100000);

    printf("A customer arrives: customer ID %2d. \n", cust_id);

    if(class == 1)
    {
        /******* Add to Buisness Queue ******/

        pthread_mutex_lock(&buisMutex);
        buis_head = addToQueue(buis_head, cust_id);
        buisQueueLength++;
        printf("A customer %d enters the Business queue with length of the queue %2d. \n Business ", cust_id, buisQueueLength);
        printQueue(buis_head);
        printf("\n");
        pthread_mutex_unlock(&buisMutex);

        /******* Get seen by clerk ******/

        // wait for a clerk
        sem_wait(&clerkSem);

        int* semVal = malloc(sizeof(int));
        if(sem_getvalue(&clerkSem, semVal) != 0)
        {
            printf("getvalue failed\n");
        }
        printf("Clerk %d awoke me! I am user %d from business and I will now sleep for %d\n\n", *semVal, cust_id, service_time);
        free(semVal);

        usleep(service_time * 100000);

        /******* Leave the airport ******/

        pthread_mutex_lock(&buisMutex);
        printf("****Customer %d leaves.****", cust_id);
        buisQueueLength--;
        buis_head = exitQueue2(buis_head, cust_id);
        printf("The Business Queue is now: ");
        printQueue(buis_head);
        printf("\n");
        pthread_mutex_unlock(&buisMutex);

        pthread_mutex_lock(&customerCountMutex);
        customersLeft--;
        //printf("There are %d customers left to serve.\n", customersLeft);
        pthread_mutex_unlock(&customerCountMutex);

        // done with clerk
        sem_post(&clerkSem);
    }
    else if(class == 0)
    {
        /******* Add to Economy Queue ******/

        pthread_mutex_lock(&econMutex);
        econ_head = addToQueue(econ_head, cust_id);
        econQueueLength++;
        printf("A customer %d enters the Economy queue with length of the queue %2d. \n Economy ", cust_id, econQueueLength);
        printQueue(econ_head);
        printf("\n");
        pthread_mutex_unlock(&econMutex);

        /******* Get seen by clerk ******/

        //pthread_mutex_lock(&buisMutex);
        printf("buisQueueLength - %d\n", buisQueueLength);
        while(buisQueueLength > 0) {} // do nothing
        //pthread_mutex_unlock(&buisMutex);

        sem_wait(&clerkSem);

        int* semVal = malloc(sizeof(int));
        if(sem_getvalue(&clerkSem, semVal) != 0)
        {
            //printf("getvalue failed\n");
        }
        printf("Clerk %d awoke me! I am user %d from economy and I will now sleep for %d\n\n", *semVal, cust_id, service_time);
        usleep(service_time * 100000);
        free(semVal);

        /******* Leave the airport ******/

        pthread_mutex_lock(&econMutex);
        printf("****Customer %d leaves.****", cust_id);
        printf("The Economy Queue is now: ");
        printQueue(econ_head);
        printf("\n");
        econQueueLength--;
        econ_head = exitQueue2(econ_head, cust_id);
        printf("The Economy Queue is now: ");
        printQueue(econ_head);
        printf("\n");
        pthread_mutex_unlock(&econMutex);

        pthread_mutex_lock(&customerCountMutex);
        customersLeft--;
        //printf("There are %d customers left to serve.\n\n", customersLeft);
        pthread_mutex_unlock(&customerCountMutex);
    }

    //pthread_cond_signal(/* The clerk awoken me */); // Notify the clerk that service is finished, it can serve another customer

    //pthread_exit(NULL);

    free(cust_id_ptr);

    return NULL;
}
