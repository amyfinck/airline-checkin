#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

CustomerInfo *head = NULL;

void add_newCust(int user_id, int class_type, int arrival_time, int service_time)
{
    CustomerInfo* new_cust = (CustomerInfo*) malloc(sizeof(CustomerInfo));

    new_cust->user_id = user_id;
    new_cust->class_type = class_type;
    new_cust->arrival_time = arrival_time;
    new_cust->service_time = service_time;
    new_cust->next = NULL;

    if(head == NULL)
    {
        head = new_cust;
    }
    else
    {
        CustomerInfo* temp = head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_cust;
    }
}

void deleteCust(int user_id)
{
    CustomerInfo* temp;
    temp = head;

    if(head->user_id == user_id)
    {
        head = head->next;
    }
    else
    {
        do
        {
            if (temp->next->user_id == user_id)
            {
                temp->next = temp->next->next;
            }
            temp = temp->next;
        }
        while (temp != NULL);
    }
}

void printList()
{
    int count = 0;
    CustomerInfo* temp = head;

    while(temp != NULL)
    {
        printf("customer %d - Class:%d, arrival time: %d, service time: %d, \n", temp->user_id, temp->class_type, temp->arrival_time, temp->service_time);
        count++;
        temp = temp->next;
    }
    printf("Total customers: %d\n", count);
}

int customerExists(int user_id)
{
    CustomerInfo* temp = head;

    while(temp != NULL)
    {
        if(temp->user_id == user_id)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}

int getClassType(int user_id)
{
    CustomerInfo* temp = head;

    while(temp != NULL)
    {
        if(temp->user_id == user_id)
        {
            return temp->class_type;
        }
        temp = temp->next;
    }
    return -1;
}

int getServiceTime(int user_id)
{
    CustomerInfo* temp = head;

    while(temp != NULL)
    {
        if(temp->user_id == user_id)
        {
            return temp->service_time;
        }
        temp = temp->next;
    }
    return -1;
}

int getArrivalTime(int user_id)
{
    CustomerInfo* temp = head;

    while(temp != NULL)
    {
        if(temp->user_id == user_id)
        {
            return temp->arrival_time;
        }
        temp = temp->next;
    }
    return -1;
}

void setClerkStartTime(int user_id, float cur_simulation_secs)
{
    CustomerInfo* temp = head;

    while(temp != NULL)
    {
        if(temp->user_id == user_id)
        {
            temp->clerk_start_time = cur_simulation_secs;
            return;
        }
        temp = temp->next;
    }
}

void printAverageWaitingTime()
{
    CustomerInfo* temp = head;
    int count = 0;
    float sum = 0;

    while(temp != NULL)
    {
        count++;
        sum += ((temp->clerk_start_time) - (float)(temp->arrival_time / 10));
        temp = temp->next;
    }
    printf("The average waiting time for all customers in the system is: %.2f seconds. \n", sum / count);
}

void printBusinessWaitingTime()
{
    CustomerInfo* temp = head;
    int count = 0;
    float sum = 0;

    while(temp != NULL)
    {
        if(temp->class_type == 1)
        {
            count++;
            sum += ((temp->clerk_start_time) - (float)(temp->arrival_time / 10));
        }
        temp = temp->next;
    }
    printf("The average waiting time for all business-class customers is: %.2f seconds. \n", sum / count);
}

void printEconomyWaitingTime()
{
    CustomerInfo* temp = head;
    int count = 0;
    float sum = 0;

    while(temp != NULL)
    {
        if(temp->class_type == 0)
        {
            count++;
            sum += ((temp->clerk_start_time) - (float)(temp->arrival_time / 10));
        }
        temp = temp->next;
    }
    printf("The average waiting time for all economy-class customers is: %.2f seconds. \n", sum / count);
}