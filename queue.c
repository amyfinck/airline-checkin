#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

Queue* addToQueue(Queue* head, int user_id)
{
    Queue* new_cust = (Queue*) malloc(sizeof(Queue));

    new_cust->user_id = user_id;
    new_cust->served = 0;
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
    printf("in exitqueue\n");
    Queue* temp;
    temp = head;

    printQueue(head);
    printf("done\n");

    if(head->user_id == user_id)
    {
        printf("head\n");
        head = head->next;
    }
    else
    {
        printf("in else\n");
        do
        {
            printf("chec\n");
            if (temp->next->user_id == user_id)
            {
                printf("if \n");
                temp->next = temp->next->next;
                printf("here\n");
            }
            printf("seg\n");
            temp = temp->next;
            printf("fault\n");
        }
        while (temp != NULL);
    }
    printf("returning from queue\n");
    return head;
}

Queue* exitQueue2(Queue* head, int user_id)
{
    Queue* temp;
    temp = head;

    if(head->user_id == user_id)
    {
        return head->next;

    }
    while(temp != NULL && temp->next != NULL)
    {
        if(temp->next->user_id == user_id)
        {
            temp->next = temp->next->next;
        }
    }
    printf("returning from queue\n");
    return head;
}

void printQueue(Queue* head)
{
    int count = 0;
    Queue* temp = head;
    
    while(temp != NULL)
    {
        printf("%d ", temp->user_id);
        count++;
        temp = temp->next;
    }
    //printf("Total customers: %d\n", count);
}

void setServed(Queue* head, int user_id)
{
    Queue* temp = head;

    while(temp != NULL)
    {
        if(temp->user_id == user_id)
        {
            temp->served = 1;
        }
        temp = temp->next;
    }
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

int allServed(Queue* head)
{
    Queue* temp = head;

    while(temp != NULL)
    {
        if(temp->served == 1)
        {
            return true;
        }
        temp = temp->next;
    }
    return false;
}
