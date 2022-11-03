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

//Queue* exitQueue(Queue* head, int user_id)
//{
//    Queue* temp;
//    temp = head;
//
//    printQueue(head);
//    printf("done\n");
//
//    if(head->user_id == user_id)
//    {
//        printf("head\n");
//        head = head->next;
//    }
//    else
//    {
//        printf("in else\n");
//        do
//        {
//            printf("chec\n");
//            if (temp->next->user_id == user_id)
//            {
//                printf("if \n");
//                temp->next = temp->next->next;
//                printf("here\n");
//            }
//            printf("seg\n");
//            temp = temp->next;
//            printf("fault\n");
//        }
//        while (temp != NULL);
//    }
//    return head;
//}

Queue* exitQueue(Queue* head, int user_id)
{
    Queue* temp;
    temp = head;
    printf("BEGIN EXIT QUEUE, starting with head %d and removing %d\n", head->user_id, user_id);

    if(head->user_id == user_id)
    {
        printf("HEAD matches user_d so removing %d from queue - ", user_id);
        if(head->next == NULL)
        {
            printf("The queue is now empty\n");
        }
        else
        {
            printf("The new head is now %d\n", head->next->user_id);
            printQueue(head->next);
        }
        return head->next;
    }
    printf("Okay so %d is not at the front of the queue, but we are trying to remove it!!\n", user_id);
//    while(temp != NULL && temp->next != NULL)
//    {
//        if(temp->next->user_id == user_id)
//        {
//            temp->next = temp->next->next;
//        }
//    }
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
