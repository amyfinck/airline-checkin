#include "main.h"

/* global variables */

int customersLeft = 0;
int startTime;

int clerkState[] = {1, 1, 1, 1, 1};

sem_t customerSem;

Queue *econ_head = NULL;
Queue *buis_head = NULL;

//struct timeval init_time; // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.
//double overall_waiting_time; //A global variable to add up the overall waiting time for all customers, every customer add their own waiting time to this variable, mutex_lock is necessary.
//int queue_length[NQUEUE];// variable stores the real-time queue length information; mutex_lock needed
int econQueueLength = 0;
int buisQueueLength = 0;
int customerCount;

pthread_mutex_t queuesMutex;
pthread_mutex_t customerCountMutex;
pthread_mutex_t clerkStateMutex;

static struct timeval start_time; // simulation start time

int main(int argc, char **argv)
{
    gettimeofday(&start_time, NULL); // record simulation start time

    pthread_mutex_init(&queuesMutex, NULL);
    pthread_mutex_init(&customerCountMutex, NULL);
    pthread_mutex_init(&clerkStateMutex, NULL);

    int NCustomers;
    int NClerks = 5;

    sem_init(&customerSem, 0, 0);

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

    pthread_t* custThread = malloc(sizeof(pthread_t)*NCustomers);
    if(custThread == NULL) printf("ERROR - malloc failed\n");
    pthread_t clerkThread[5];

    startTime = clock();

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

    for(int i = 0; i < NClerks; i++)
    {
        int clerk_id = i;

        int* clerk_id_ptr = malloc(sizeof(int));
        *clerk_id_ptr = clerk_id;

        if(pthread_create(&clerkThread[i], NULL, &clerk, clerk_id_ptr) != 0)
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

    for(int i = 0; i < NClerks; i++)
    {
        if(pthread_join(clerkThread[i], NULL) != 0)
        {
            printf("Error joining clerk thread %d\n", i);
        }
    }

    pthread_mutex_destroy(&queuesMutex);
    pthread_mutex_destroy(&customerCountMutex);

    sem_close(&customerSem);

    // calculate the average waiting time of all customers
    return 0;
}

// function entry for customer threads
void * customer_entry(void* cust_id_ptr)
{
    int cust_id = *(int*)cust_id_ptr;
    int class = getClassType(cust_id);
    int arrival_time = getArrivalTime(cust_id);

    // wait for customer to arrive
    usleep(arrival_time * 100000);

    double cur_simulation_secs = getCurrentSimulationTime();
    printf("%d: A customer arrives: customer ID %d", (int)(cur_simulation_secs * 10), cust_id);

    if(class == 1)
    {
        /******* Add to Business Queue ******/
        pthread_mutex_lock(&queuesMutex);
        buis_head = addToQueue(buis_head, cust_id);
        cur_simulation_secs = getCurrentSimulationTime();
        buisQueueLength++;
        printf("%d: A customer enters a queue: the queue ID 1 and length of the queue %d", (int)(cur_simulation_secs * 10), buisQueueLength);
        printQueue(econ_head);
        printf("| ");
        printQueue(buis_head);
        printf("\n");
        pthread_mutex_unlock(&queuesMutex);
        sem_post(&customerSem);
    }
    else if(class == 0)
    {
        /******* Add to Economy Queue ******/
        pthread_mutex_lock(&queuesMutex);
        econ_head = addToQueue(econ_head, cust_id);
        cur_simulation_secs = getCurrentSimulationTime();
        econQueueLength++;
        printf("%d: A customer %d enters the Economy queue | ", (int)(cur_simulation_secs * 10), cust_id);
        printQueue(econ_head);
        printf("| ");
        printQueue(buis_head);
        printf("\n");
        pthread_mutex_unlock(&queuesMutex);
        sem_post(&customerSem);
    }
    free(cust_id_ptr);
    return NULL;
}

double getCurrentSimulationTime()
{
    struct timeval cur_time;
    double cur_secs, init_secs;

    //pthread_mutex_lock(&start_time_mtex); you may need a lock here
    init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);
    //pthread_mutex_unlock(&start_time_mtex);

    gettimeofday(&cur_time, NULL);
    cur_secs = (cur_time.tv_sec + (double) cur_time.tv_usec / 1000000);

    return cur_secs - init_secs;
}

void * clerk(void* clerk_id_ptr)
{
    int clerk_id = *(int*)clerk_id_ptr;
    int cust_id;
    int service_time;

    while(1)
    {
        pthread_mutex_lock(&customerCountMutex);
        if(customersLeft == -1) break;
        pthread_mutex_unlock(&customerCountMutex);

        pthread_mutex_lock(&clerkStateMutex);
        clerkState[clerk_id] = 0;
        pthread_mutex_unlock(&clerkStateMutex);

        sem_wait(&customerSem);

        /*******Get next customer in line**********/
        pthread_mutex_lock(&clerkStateMutex);
        clerkState[clerk_id] = 1;
        pthread_mutex_unlock(&clerkStateMutex);

        pthread_mutex_lock(&queuesMutex);
        if(buis_head != NULL)
        {
            cust_id = buis_head->user_id;
            service_time = getServiceTime(cust_id);
            exitQueue(buis_head, cust_id);

            pthread_mutex_lock(&customerCountMutex);
            customersLeft--;
            pthread_mutex_unlock(&customerCountMutex);
        }
        else if(econ_head != NULL)
        {
            cust_id = econ_head->user_id;
            service_time = getServiceTime(cust_id);
            exitQueue(econ_head, cust_id);

            pthread_mutex_lock(&customerCountMutex);
            customersLeft--;
            pthread_mutex_unlock(&customerCountMutex);

        }
        else
        {
            // there is nobody in either queue
            if(customersLeft == -1) break;
            else
            {
                printf("Nobody in queue but last one has not been served\n");
                break;
            }
        }
        pthread_mutex_unlock(&queuesMutex);

        /********Serve customer***********/

        int cur_simulation_secs = getCurrentSimulationTime();
        printf("%d: I am clerk %d and I have grabbed customer %d. I will now sleep for %d.\n", (int)(cur_simulation_secs * 10), clerk_id + 1, cust_id, service_time);
        usleep(service_time * 100000);
        cur_simulation_secs = getCurrentSimulationTime();
        printf("%d: Clerk %d finished with %d. They will now EXIT.\n", (int)(cur_simulation_secs * 10), clerk_id + 1, cust_id);

        /********See if that was the last one**********/
        pthread_mutex_lock(&customerCountMutex);
        if(customersLeft == -1) break;
        customersLeft--;
        int custs_left = customersLeft;
        pthread_mutex_lock(&customerCountMutex);

        if(custs_left == 0)
        {
            pthread_mutex_lock(&customerCountMutex);
            customersLeft = -1;
            pthread_mutex_unlock(&customerCountMutex);

            // This was the last customer - tear down all other clerks in waiting state
            pthread_mutex_lock(&clerkStateMutex);
            for(int i = 0; i < 5; i++)
            {
                // if clerk is in waiting state
                if(clerkState[i] == 0)
                {
                    // send a sem post to break out of waiting state
                    sem_post(&customerSem);
                }
            }
            pthread_mutex_unlock(&clerkStateMutex);
        }
        else if(customersLeft == -1) break;
    }

    free(clerk_id_ptr);
    return NULL;
}