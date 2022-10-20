#ifndef ASSIGNMENT2_QUEUE_H
#define ASSIGNMENT2_QUEUE_H

typedef struct queue Queue;

struct queue{
    int user_id;
    Queue* next;
};

Queue* addToQueue(Queue* head, int user_id);
Queue* exitQueue(Queue* head, int user_id);
void printQueue(Queue* head);
int customerInQueue(Queue* head, int user_id);

#endif //ASSIGNMENT2_QUEUE_H
