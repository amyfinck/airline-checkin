#include "main.h"

/* global variables */

int customersLeft = 0;
int startTime;

int clerkState[] = {1, 1, 1, 1, 1};

sem_t customerSem;

Queue *econ_head = NULL;
Queue *buis_head = NULL;

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

    fscanf(custFile, "%d\n", &NCustomers);
    customersLeft = NCustomers;

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

    free(custThread);

    printf("All jobs done...\n");

    printAverageWaitingTime();
    printBusinessWaitingTime();
    printEconomyWaitingTime();

    return 0;
}

void * customer_entry(void* cust_id_ptr)
{
    int cust_id = *(int*)cust_id_ptr;
    int class = getClassType(cust_id);
    int arrival_time = getArrivalTime(cust_id);

    // wait for customer to arrive
    usleep(arrival_time * 100000);

    double cur_simulation_secs = getCurrentSimulationTime();
    printf("A customer arrives: customer ID %d\n", cust_id);

    if(class == 1)
    {
        /******* Add to Business Queue ******/
        pthread_mutex_lock(&queuesMutex);
        buis_head = addToQueue(buis_head, cust_id);
        buisQueueLength++;
        printf("A customer enters a queue: the queue ID 1 and length of the queue %d.\n", buisQueueLength);
        pthread_mutex_unlock(&queuesMutex);
        sem_post(&customerSem);
    }
    else if(class == 0)
    {
        /******* Add to Economy Queue ******/
        pthread_mutex_lock(&queuesMutex);
        econ_head = addToQueue(econ_head, cust_id);
        econQueueLength++;
        printf("A customer enters a queue: the queue ID 0 and length of the queue %d.\n", econQueueLength);
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

    init_secs = (start_time.tv_sec + (double) start_time.tv_usec / 1000000);

    gettimeofday(&cur_time, NULL);
    cur_secs = (cur_time.tv_sec + (double) cur_time.tv_usec / 1000000);

    return cur_secs - init_secs;
}

void * clerk(void* clerk_id_ptr)
{
    int clerk_id = *(int*)clerk_id_ptr;
    int cust_id;
    int service_time;
    double cur_simulation_secs;

    while(1)
    {
        pthread_mutex_lock(&customerCountMutex);
        if(customersLeft == -1)
        {
            pthread_mutex_unlock(&customerCountMutex);
            break;
        }
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
            buis_head = exitQueue(buis_head, cust_id);

            pthread_mutex_lock(&customerCountMutex);
            customersLeft--;
            buisQueueLength--;
            pthread_mutex_unlock(&customerCountMutex);
        }
        else if(econ_head != NULL)
        {
            cust_id = econ_head->user_id;
            service_time = getServiceTime(cust_id);
            econ_head = exitQueue(econ_head, cust_id);

            pthread_mutex_lock(&customerCountMutex);
            customersLeft--;
            econQueueLength--;
            pthread_mutex_unlock(&customerCountMutex);
        }
        else
        {
            // there is nobody in either queue
            pthread_mutex_lock(&customerCountMutex);
            if(customersLeft == -1)
            {
                pthread_mutex_unlock(&customerCountMutex);
                pthread_mutex_unlock(&queuesMutex);
                break;
            }
            else
            {
                printf("Error - nobody in queue but last one has not been served\n");
                pthread_mutex_unlock(&customerCountMutex);
                pthread_mutex_unlock(&queuesMutex);
                break;
            }
        }

        cur_simulation_secs = getCurrentSimulationTime();
        printf("A clerk starts serving a customer: start time %0.2f, the customer ID %d, the clerk ID %d.\n", cur_simulation_secs, cust_id, clerk_id + 1);
        pthread_mutex_unlock(&queuesMutex);

        /********Serve customer***********/

        cur_simulation_secs = getCurrentSimulationTime();
        setClerkStartTime(cust_id, cur_simulation_secs);

        usleep(service_time * 100000);

        cur_simulation_secs = getCurrentSimulationTime();
        printf("-->>> A clerk finishes serving a customer: end time %0.2f, the customer ID %d, the clerk ID %d.\n", cur_simulation_secs, cust_id, clerk_id + 1);

        /********See if that was the last one**********/
        pthread_mutex_lock(&customerCountMutex);
        if(customersLeft == -1)
        {
            pthread_mutex_unlock(&customerCountMutex);
            break;
        }
        int custs_left = customersLeft;
        pthread_mutex_unlock(&customerCountMutex);

        if(custs_left == 0)
        {
            pthread_mutex_lock(&customerCountMutex);
            customersLeft = -1;
            pthread_mutex_unlock(&customerCountMutex);

            // This was the last customer - tear down all other clerks in waiting state
            pthread_mutex_lock(&clerkStateMutex);
            for(int i = 0; i < 5; i++)
            {
                if(clerkState[i] == 0)
                {
                    sem_post(&customerSem);
                }
            }
            pthread_mutex_unlock(&clerkStateMutex);
        }
    } // end while

    free(clerk_id_ptr);
    return NULL;
}