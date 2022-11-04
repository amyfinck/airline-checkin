//
// Created by Amy Finck on 2022-10-19.
//

#ifndef ASSIGNMENT2_MAIN_H
#define ASSIGNMENT2_MAIN_H

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
#include <sys/time.h>

#include "linked_list.h"
#include "queue.h"

void * customer_entry(void * cust_id);
void *clerk_entry(void * clerkNum);
double getCurrentSimulationTime();
void tryPrintQueues(Queue* buis_head, Queue* econ_head);
void * clerk(void* clerk_id_ptr);

#endif //ASSIGNMENT2_MAIN_H

