#ifndef ASSIGNMENT2_QUEUE_H
#define ASSIGNMENT2_QUEUE_H

typedef struct queue Queue;

struct queue{
    int user_id;
    int served;
    Queue* next;
};

Queue* addToQueue(Queue* head, int user_id);
Queue* exitQueue(Queue* head, int user_id);
void printQueue(Queue* head);
void setServed(Queue* head, int user_id);
int customerInQueue(Queue* head, int user_id);
int allServed(Queue* head);

#endif //ASSIGNMENT2_QUEUE_H
