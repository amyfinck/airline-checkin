#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "linked_list.h"

CustomerInfo *head = NULL;

void add_newCust(int user_id, int class_type, int service_time, int arrival_time)
{
    CustomerInfo* new_cust = (CustomerInfo*) malloc(sizeof(CustomerInfo));

    new_cust->user_id = user_id;
    new_cust->class_type = class_type;
    new_cust->service_time = service_time;
    new_cust->arrival_time = arrival_time;
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
        printf("customer %d - Class:%d, service time: %d, arrival time: %d\n", temp->user_id, temp->class_type, temp->service_time, temp->arrival_time);
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