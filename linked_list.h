#ifndef _LINKEDLIST_H_
#define _LINKEDLIST_H_

typedef struct customer_info CustomerInfo;

struct customer_info{ /// use this struct to record the customer information read from customers.txt
    int user_id;
    int class_type;
    int service_time;
    int arrival_time;
    CustomerInfo* next;
};

void add_newCust(int user_id, int class_type, int arrival_time, int service_time);
void deleteCust(int user_id);
void printList();
int customerExists(int user_id);
int getClassType(int user_id);
int getServiceTime(int user_id);
int getArrivalTime(int user_id);

#endif
