// queue.h
#ifndef QUEUE_H
#define QUEUE_H

#include "node.h" // Make sure to include the node structure

// Queue node for linked list queue
typedef struct queue_node {
    node_t* data;
    struct queue_node* next;
} queue_node_t;

// Queue structure
typedef struct {
    queue_node_t* front;
    queue_node_t* rear;
    int size;
} queue_t;

// Function declarations for queue operations
queue_t* create_queue();
void enqueue(queue_t* queue, node_t* data);
node_t* dequeue(queue_t* queue);
bool is_queue_empty(queue_t* queue);
void queue_clear(queue_t* queue);
void queue_destroy(queue_t* queue);

#endif // QUEUE_H

