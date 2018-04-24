//
// Created by kmdai on 18-4-21.
//

#include "push_queue.h"

q_list *queue = NULL;
int32_t cancel = 1;

int init_queue() {
    queue = (q_list *) malloc(sizeof(q_list));
    if (queue) {
        queue->front = NULL;
        queue->rear = NULL;
        pthread_mutex_init(&queue->mutex, NULL);
        pthread_cond_init(&queue->cond, NULL);
//        queue->mutex = PTHREAD_MUTEX_INITIALIZER;
//        queue->cond = PTHREAD_COND_INITIALIZER;
        return 0;
    }
    return 1;
}

int empty_queue() {
    if (!queue) {
        return 1;
    }
    if (queue->front == NULL && queue->rear == NULL) {
        return 1;
    } else {
        return 0;
    }
}

int in_queue(q_node_p node) {
    if (queue) {
        pthread_mutex_lock(&queue->mutex);
        if (queue->front == NULL && queue->rear == NULL) {
            queue->front = queue->rear = node;
        } else {
            queue->rear->next = node;
            queue->rear = node;
        }
        pthread_cond_signal(&queue->cond);
        pthread_mutex_unlock(&queue->mutex);
        return 0;
    }
    return 1;
}

q_node_p out_queue() {
    q_node_p node = NULL;
    if (queue) {
        pthread_mutex_lock(&queue->mutex);
        while (empty_queue()) {
            if (cancel <= 0) {
                pthread_mutex_unlock(&queue->mutex);
                return NULL;
            }
            pthread_cond_wait(&queue->cond, &queue->mutex);
        }
        node = queue->front;
        queue->front = queue->front->next;
        if (queue->front == NULL) {
            queue->rear = NULL;
        }
        pthread_mutex_unlock(&queue->mutex);
    }
    return node;
}

void cancel_queue() {
    pthread_mutex_lock(&queue->mutex);
    cancel = 0;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

int destroy_queue() {
    pthread_mutex_lock(&queue->mutex);
    if (queue->front != NULL) {
        q_node_p node = queue->front;
        while (node) {
//            free(node->data);
            free(node);
            queue->front = queue->front->next;
            node = queue->front;
        }
        queue->front = queue->rear = NULL;
    }
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
    if (queue != NULL) {
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->cond);
        free(queue);
        queue = NULL;
    }
    return 0;
}

q_node_p create_node(char *data, int32_t size, int32_t type, uint32_t time) {
    q_node_p node = (q_node_p) malloc(sizeof(q_node) + size);
    node->data = (char *) node + sizeof(q_node);
    memcpy(node->data, data, size);
    node->time = time;
    node->size = size;
    node->type = type;
    node->next = NULL;
    return node;
}
