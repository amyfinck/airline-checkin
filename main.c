

#include "main.h"


/* global variables */

int customersLeft = 0;
int startTime;

int clerkAvailable[] = {1, 1, 1, 1, 1};

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

static struct timeval start_time; // simulation start time

int main(int argc, char **argv)
{
    gettimeofday(&start_time, NULL); // record simulation start time

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
    int clerk;

    // wait for customer to arrive
    usleep(arrival_time * 100000);

    double cur_simulation_secs = getCurrentSimulationTime();
    printf("%d: A customer arrives: customer ID %2d", (int)(cur_simulation_secs * 10), cust_id);
    tryPrintQueues(buis_head, econ_head);

    if(class == 1)
    {
        /******* Add to Buisness Queue ******/

        pthread_mutex_lock(&buisMutex);
        buis_head = addToQueue(buis_head, cust_id);
        buisQueueLength++;
	    cur_simulation_secs = getCurrentSimulationTime();

        //A customer enters a queue: the queue ID 1, and length of the queue  1.
        printf("%d: A customer enters a queue: the queue ID 1 and length of the queue %d", (int)(cur_simulation_secs * 10), buisQueueLength);
        if(pthread_mutex_trylock(&econMutex) == 0)
        {
            printQueue(econ_head);
            pthread_mutex_unlock(&econMutex);
        }
    	printf("| ");
    	printQueue(buis_head);
    	printf("\n");
        pthread_mutex_unlock(&buisMutex);

        /******* Get seen by clerk ******/

        // wait for a clerk
        sem_wait(&clerkSem);

        pthread_mutex_lock(&buisMutex);
        buisQueueLength--;
        buis_head = exitQueue(buis_head, cust_id);
        pthread_mutex_unlock(&buisMutex);

	    cur_simulation_secs = getCurrentSimulationTime();
	    clerk = getClerk();

        //A clerk starts serving a customer: start time 0.20, the customer ID  1, the clerk ID 2.
        printf("A clerk starts serving a customer: start time %d, the customer ID %d, the clerk ID %d", (int)(cur_simulation_secs * 10), cust_id, clerk);

        usleep(service_time * 100000);

        /******* Leave the airport ******/

        // TODO - add clerk mutex
        clerkAvailable[clerk - 1] = 1;

        pthread_mutex_lock(&buisMutex);
	    cur_simulation_secs = getCurrentSimulationTime();
        printf("%d: ****Customer %d leaves.**** | ", (int)(cur_simulation_secs * 10), cust_id);
        printQueue(econ_head);
        printf("| ");
        printQueue(buis_head);
        printf("\n");
        pthread_mutex_unlock(&buisMutex);

        pthread_mutex_lock(&customerCountMutex);
        customersLeft--;
        printf("There are %d customers left to serve.\n", customersLeft);
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
	    cur_simulation_secs = getCurrentSimulationTime();
        printf("%d: A customer %d enters the Economy queue | ", (int)(cur_simulation_secs * 10), cust_id);
        printQueue(econ_head);
        printf("| ");
        if(pthread_mutex_trylock(&buisMutex) == 0)
        {
            printQueue(buis_head);
            pthread_mutex_unlock(&buisMutex);
        }
        printf("\n");
        pthread_mutex_unlock(&econMutex);

        /******* Get seen by clerk ******/

        // TODO - this is not in a mutex!
        while(buis_head != NULL) {} // do nothing 

	    sem_wait(&clerkSem);

	    cur_simulation_secs = getCurrentSimulationTime();
	    clerk = getClerk();
        printf("%d: Clerk %d awoke me! I am user %d from economy and I will now sleep for %d | ", (int)(cur_simulation_secs * 10), clerk, cust_id, service_time);
        tryPrintQueues(buis_head, econ_head);

        pthread_mutex_lock(&econMutex);
        econQueueLength--;
        econ_head = exitQueue(econ_head, cust_id);
        pthread_mutex_unlock(&econMutex);

        usleep(service_time * 100000);
	    clerkAvailable[clerk - 1] = 1;
	    sem_post(&clerkSem);

        /******* Leave the airport ******/

        pthread_mutex_lock(&econMutex);
	    cur_simulation_secs = getCurrentSimulationTime();
        printf("%d: ****Customer %d leaves.**** | ", (int)(cur_simulation_secs * 10), cust_id);
        printQueue(econ_head);
        printf("| ");
        if(pthread_mutex_trylock(&buisMutex) == 0)
        {
            printQueue(buis_head);
            pthread_mutex_unlock(&buisMutex);
        }
        printQueue(buis_head);
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

int getClerk()
{
    for(int i = 0; i < 5; i++)
    {
	if(clerkAvailable[i] == 1)
        {
            clerkAvailable[i] = 0;
	    return i + 1;
        }
    }
    return -1;
}

void tryPrintQueues(Queue* buis_head, Queue* econ_head)
{
    if(pthread_mutex_trylock(&buisMutex) == 0)
    {
        if(pthread_mutex_trylock(&econMutex) == 0)
        {
            printQueue(econ_head);
            printf("| ");
            printQueue(buis_head);
            printf("\n");
            pthread_mutex_unlock(&econMutex);
        }
        else
        {
            printf("\n");
        }
        pthread_mutex_unlock(&buisMutex);
    }
    else printf("\n");
}

