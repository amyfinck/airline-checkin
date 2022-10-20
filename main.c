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


/* global variables */

//struct timeval init_time; // use this variable to record the simulation start time; No need to use mutex_lock when reading this variable since the value would not be changed by thread once the initial time was set.
//double overall_waiting_time; //A global variable to add up the overall waiting time for all customers, every customer add their own waiting time to this variable, mutex_lock is necessary.
//int queue_length[NQUEUE];// variable stores the real-time queue length information; mutex_lock needed

/* Other global variable may include: 
 1. condition_variables (and the corresponding mutex_lock) to represent each queue; 
 2. condition_variables to represent clerks
 3. others.. depend on your design
 */

int main(int argc, char **argv)
{
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

    int NClerks = 5;
    int NCustomers = 8;

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

    // initialize all the condition variable and thread lock will be used

    /** Read customer information from txt file and store them in the structure you created

        1. Allocate memory(array, link list etc.) to store the customer information.
        2. File operation: fopen fread getline/gets/fread ..., store information in data structure you created

    */

    //create clerk thread (optional)
    for(int i = 0; i < NClerks; i++)
    {
        //pthread_create(&clerkId[i], NULL, clerk_entry, (void *)&clerk_info[i]); // clerk_info: passing the clerk information (e.g., clerk ID) to clerk thread
    }

    //create customer thread
    for(int i = 0; i < NCustomers; i++)
    {
        //pthread_create(&customId[i], NULL, customer_entry, (void *)&custom_info[i]); //custom_info: passing the customer information (e.g., customer ID, arrival time, service time, etc.) to customer thread
    }
    // wait for all customer threads to terminate
//    forEach customer thread{
//            pthread_join(...);
//    }
    // destroy mutex & condition variable (optional)

    // calculate the average waiting time of all customers
    return 0;
}

// function entry for customer threads

void * customer_entry(void * cus_info){

    //struct customer_info * p_myInfo = (struct info_node *) cus_info;

    //usleep(/* the arrival time of this customer */);

    //fprintf(stdout, "A customer arrives: customer ID %2d. \n", p_myInfo->user_id);

    /* Enqueue operation: get into either business queue or economy queue by using p_myInfo->class_type*/

    //pthread_mutex_lock(/* mutexLock of selected queue */);

    //fprintf(stdout, "A customer enters a queue: the queue ID %1d, and length of the queue %2d. \n", /*...*/);

    /* updates the queue_length, mutex_lock needed */

    //pthread_cond_wait(/* cond_var of selected queue */, /* mutexLock of selected queue */);
    //Now pthread_cond_wait returned, customer was awoken by one of the clerks


    //pthread_mutex_unlock(/*mutexLock of selected queue*/); //unlock mutex_lock such that other customers can enter into the queue

    /* Try to figure out which clerk awoken me, because you need to print the clerk Id information */

    /* get the current machine time; updates the overall_waiting_time*/

    //fprintf(stdout, "A clerk starts serving a customer: start time %.2f, the customer ID %2d, the clerk ID %1d. \n", /*...*/);

    //usleep(/* as long as the service time of this customer */);

    /* get the current machine time; */
    //fprintf(stdout, "A clerk finishes serving a customer: end time %.2f, the customer ID %2d, the clerk ID %1d. \n", /* ... */);\

    //pthread_cond_signal(/* The clerk awoken me */); // Notify the clerk that service is finished, it can serve another customer

    //pthread_exit(NULL);

    return NULL;
}

// function entry for clerk threads
void *clerk_entry(void * clerkNum){

    //while(1){
        // clerk is idle now

        /* selected_queue_ID = Select the queue based on the priority and current customers number */

        //pthread_mutex_lock(/* mutexLock of the selected queue */);
        //pthread_cond_broadcast(/* cond_var of the selected queue */); // Awake the customer (the one enter into the queue first) from the longest queue (notice the customer he can get served now)
        //pthread_mutex_unlock(/* mutexLock of the selected queue */);

        //pthread_cond_wait(); // wait the customer to finish its service, clerk busy
    //}

    //pthread_exit(NULL);

    return NULL;
}
