//
// Created by kmdai on 18-4-21.
//

#include "push_queue.h"

int init_queue() {
    queue = (q_list *) malloc(sizeof(q_list));
    if (queue) {
        return 0;
    }
    return 1;
}

int empty_queue() {
    if (queue->front == NULL || queue->rear == NULL) {
        return 0;
    } else {
        return 1;
    }
}

int in_queue(q_node_p node) {
    if (queue) {
        if (queue->front == NULL && queue->rear == NULL) {
            queue->front = queue->rear = node;
        } else {
            queue->rear = node;
        }
        return 0;
    }
    return 1;
}

q_node_p out_queue() {
    q_node_p node;
    if (queue->front == NULL) {
        return NULL;
    }
    node = queue->front;
    queue->front = queue->front->next;
    return node;
}

int destroy_queue() {
    if (queue->front != NULL) {
        q_node_p node = queue->front;
        while (node) {
            free(node->data);
            free(node);
            queue->front = queue->front->next;
            node = queue->front;
        }
        queue->front = queue->rear = NULL;
    }
    free(queue);
}

q_node_p create_node(__uint8_t *data, __int32_t size) {
    q_node_p node = (q_node_p) malloc(sizeof(q_node) + size);
    node->data = (__uint8_t *) node + sizeof(q_node);
    memcpy(node->data, data, size);
    return node;
}
