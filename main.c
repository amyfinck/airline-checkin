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

#include "main.h"
#include "linked_list.h"
#include "queue.h"

/* global variables */

int customersLeft = 0;

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

    printf("number of customers is %d\n", NCustomers);

    for(int i = 0; i < NCustomers; i++)
    {
        int user_id, class_type, service_time, arrival_time;
        fscanf(custFile, "%d:%d,%d,%d\n", &user_id, &class_type, &service_time, &arrival_time);
        add_newCust(user_id, class_type, service_time, arrival_time);

        int* user_id_ptr = malloc(sizeof(int));
        *user_id_ptr = user_id;

        if(pthread_create(&custThread[i], NULL, &customer_entry, user_id_ptr) != 0)
        {
            printf("Error creating customer thread %d\n", i);
        }
    }

    for(int i = 0; i < NClerks; i++)
    {
        int* clerk_id_ptr = malloc(sizeof(int));
        *clerk_id_ptr = i + 1;
        if(pthread_create(&clerkThread[i], NULL, &clerk_entry, clerk_id_ptr) != 0)
        {
            printf("Error creating clerk thread %d\n", i);
        }
    }

    for(int i = 0; i < NCustomers; i++)
    {
        if(pthread_join(custThread[i], NULL) != 0)
        {
            printf("Error joining customer thread %d\n", i);
        }
    }

    for(int i = 0; i < NClerks; i++)
    {
        if(pthread_join(clerkThread[i], NULL) != 0)
        {
            printf("Error joining clerk thread %d\n", i);
        }
    }

    pthread_mutex_destroy(&econMutex);
    pthread_mutex_destroy(&buisMutex);

    pthread_mutex_destroy(&customerCountMutex);

    pthread_cond_destroy(&econCond);
    pthread_cond_destroy(&buisCond);

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

    if(class == 0)
    {
        pthread_mutex_lock(&econMutex);
        econQueueLength++;
        econ_head = addToQueue(econ_head, cust_id);
        printf("Customer %d enters a queue: the queue ID %1d, and length of the queue %2d. \n", cust_id, class, econQueueLength);

        while(1)
        {
            pthread_cond_wait(&econCond, &econMutex);
            // pthread_mutex_unlock(&econMutex);
            // wait for signal on econCond
            // pthread_mutex_lock(&econMutex);
            printf("%d received\n", cust_id);

            if(econ_head != NULL && econ_head->user_id == cust_id)
            {
                /* Try to figure out which clerk awoken me, because you need to print the clerk Id information */
                /* get the current machine time; updates the overall_waiting_time*/


                printf("A clerk is seeing me! I am user %d and I will now sleep for %d\n", cust_id, service_time);
                usleep(service_time * 100000);
                econ_head = exitQueue(econ_head, cust_id);
                econQueueLength--;

                pthread_mutex_unlock(&customerCountMutex);
                customersLeft--;
                pthread_mutex_lock(&customerCountMutex);

                break;
            }
        }
        pthread_mutex_unlock(&econMutex);
    }
    else if(class == 1)
    {
        pthread_mutex_lock(&buisMutex);
        buisQueueLength++;
        buis_head = addToQueue(buis_head, cust_id);
        printf("A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", class, buisQueueLength);

        while(1)
        {
            pthread_cond_wait(&buisCond, &buisMutex);
            // pthread_mutex_unlock(&buisMutex);
            // wait for signal on buisCond
            // pthread_mutex_lock(&buisMutex);

            if(buis_head != NULL && buis_head->user_id == cust_id)
            {
                /* Try to figure out which clerk awoken me, because you need to print the clerk Id information */
                /* get the current machine time; updates the overall_waiting_time*/

                printf("A clerk has awoken me! I am user %d and I will now sleep for %d\n", cust_id, service_time);
                //fprintf(stdout, "A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", /*...*/);
                usleep(service_time * 100000);
                //fprintf(stdout, "A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", /* ... */);\
                buis_head = exitQueue(buis_head, cust_id);
                buisQueueLength--;

                pthread_mutex_unlock(&customerCountMutex);
                customersLeft--;
                printf("After customer %d leaves, there are %d customers left.\n", cust_id, customersLeft);
                pthread_mutex_lock(&customerCountMutex);

                break;
            }
        }
        pthread_mutex_unlock(&buisMutex);
    }

    //pthread_cond_signal(/* The clerk awoken me */); // Notify the clerk that service is finished, it can serve another customer

    //pthread_exit(NULL);

    free(cust_id_ptr);

    printf("\ncustomer %d returning\n\n", cust_id);

    return NULL;
}

// function entry for clerk threads
void *clerk_entry(void * clerk_id_ptr)
{
    int clerk_id = *(int*)clerk_id_ptr;
    printf("Clerk %d checking into desk\n", clerk_id);
    while(1)
    {
        if(buisQueueLength != 0)
        {
            pthread_mutex_lock(&buisMutex);
            pthread_mutex_unlock(&buisMutex);

            // TODO - the clerk needs to wait

            pthread_mutex_unlock(&customerCountMutex);
            if(customersLeft == 0) break;
            pthread_mutex_lock(&customerCountMutex);
        }
        else
        {
            pthread_mutex_lock(&econMutex);
            pthread_cond_broadcast(&econCond);
            pthread_mutex_unlock(&econMutex);

            pthread_mutex_unlock(&customerCountMutex);
            if(customersLeft == 0) break;
            pthread_mutex_lock(&customerCountMutex);
        }
    }

    printf("Clerk %d clocking out\n", clerk_id);

    free(clerk_id_ptr);

    return NULL;
}
