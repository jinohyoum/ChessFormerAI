// queue.c
#include <stdlib.h>
#include <stdio.h>
#include "queue.h"

// Create a new queue and return a pointer to it
queue_t* create_queue() {
    queue_t* queue = (queue_t*)malloc(sizeof(queue_t));
    queue->front = NULL;
    queue->rear = NULL;
    queue->size = 0;
    return queue;
}

// Enqueue a new node into the queue
void enqueue(queue_t* queue, node_t* data) {
    queue_node_t* new_node = (queue_node_t*)malloc(sizeof(queue_node_t));
    new_node->data = data;
    new_node->next = NULL;
    
    if (queue->rear == NULL) {
        // Queue is empty, so both front and rear point to the new node
        queue->front = new_node;
        queue->rear = new_node;
    } else {
        // Append the new node at the rear and update rear pointer
        queue->rear->next = new_node;
        queue->rear = new_node;
    }
    
    queue->size++;
}

// Dequeue the front node from the queue and return its data
node_t* dequeue(queue_t* queue) {
    if (queue->front == NULL) {
        // Queue is empty, return NULL
        return NULL;
    }
    
    queue_node_t* temp = queue->front;
    node_t* data = temp->data;
    
    queue->front = queue->front->next;
    
    if (queue->front == NULL) {
        // If front becomes NULL, then the queue is now empty
        queue->rear = NULL;
    }
    
    free(temp);
    queue->size--;
    
    return data;
}

// Check if the queue is empty
bool is_queue_empty(queue_t* queue) {
    return queue->front == NULL;
}





