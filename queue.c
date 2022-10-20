#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

Queue* addToQueue(Queue* head, int user_id)
{
    Queue* new_cust = (Queue*) malloc(sizeof(Queue));

    new_cust->user_id = user_id;
    new_cust->next = NULL;

    if(head == NULL)
    {
        head = new_cust;
    }
    else
    {
        Queue* temp = head;
        while(temp->next != NULL)
        {
            temp = temp->next;
        }
        temp->next = new_cust;
    }

    return head;
}

Queue* exitQueue(Queue* head, int user_id)
{
    Queue* temp;
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
    return head;
}

void printQueue(Queue* head)
{
    int count = 0;
    Queue* temp = head;

    while(temp != NULL)
    {
        printf("customer %d\n", temp->user_id);
        count++;
        temp = temp->next;
    }
    printf("Total customers: %d\n", count);
}

int customerInQueue(Queue* head, int user_id)
{
    Queue* temp = head;

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