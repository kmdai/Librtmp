//
// Created by kmdai on 18-4-21.
//

#include "push_queue.h"

q_list *queue = NULL;
int32_t cancel = 0;

int init_queue() {
    queue = (q_list *) malloc(sizeof(q_list));
    cancel = 0;
    if (queue) {
        queue->front = NULL;
        queue->rear = NULL;
        pthread_mutex_init(&queue->mutex, NULL);
        pthread_cond_init(&queue->cond, NULL);
        queue->length = 0;
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

void check_length() {
    if (queue->length >= QUEUE_MAX_LENGTH) {
        q_node_p node = queue->front;
        int before = queue->length;
        while (node != NULL && (node->flag != NODE_FLAG_KEY_FRAME ||
                                queue->length >= QUEUE_MAX_LENGTH)) {
            queue->front = node->next;
            queue->length -= node->size;
            free(node);
            node = queue->front;
        }
        SRS_LOGE("free queue before%d,ofter:%d", before, queue->length);
    }
}

int in_queue(q_node_p node) {
    if (queue && !cancel) {
        pthread_mutex_lock(&queue->mutex);
        if (queue->front == NULL && queue->rear == NULL) {
            queue->front = queue->rear = node;
        } else {
            check_length();
            queue->rear->next = node;
            queue->rear = node;
        }
        queue->length += node->size;
        pthread_cond_signal(&queue->cond);
        pthread_mutex_unlock(&queue->mutex);
        return 0;
    }
    free(node);
    return 1;
}


q_node_p out_queue() {
    q_node_p node = NULL;
    if (queue) {
        pthread_mutex_lock(&queue->mutex);
        while (empty_queue()) {
            if (cancel) {
                pthread_mutex_unlock(&queue->mutex);
                destroy_queue();
                return node;
            }
            pthread_cond_wait(&queue->cond, &queue->mutex);
        }
        node = queue->front;
        if (node == NULL) {
            SRS_LOGE("queue->front==NULL");
        }
        queue->front = queue->front->next;
        if (queue->front == NULL) {
            queue->rear = NULL;
        }
        queue->length -= node->size;
        pthread_mutex_unlock(&queue->mutex);
    }
    return node;
}

void cancel_queue() {
    pthread_mutex_lock(&queue->mutex);
    cancel = 1;
    pthread_cond_signal(&queue->cond);
    pthread_mutex_unlock(&queue->mutex);
}

int destroy_queue() {
    if (queue->front != NULL) {
        q_node_p node = queue->front;
        while (node) {
            queue->length -= node->size;
            free(node);
            queue->front = queue->front->next;
            node = queue->front;
        }
        queue->front = queue->rear = NULL;
    }
    if (queue != NULL) {
        pthread_mutex_destroy(&queue->mutex);
        pthread_cond_destroy(&queue->cond);
        free(queue);
        queue = NULL;
    }
    return 0;
}

q_node_p create_node(char *data, uint32_t size, node_type type, int32_t flag, uint32_t time) {
    q_node_p node = (q_node_p) malloc(sizeof(q_node) + size);
    node->data = (char *) node + sizeof(q_node);
    memcpy(node->data, data, size);
    node->time = time;
    node->size = size;
    node->type = type;
    node->flag = flag;
    node->next = NULL;
    return node;
}
