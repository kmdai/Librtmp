//
// Created by kmdai on 18-4-21.
// the queue for rtmp
//

#ifndef LIBRTMP_PUSH_QUEUE_H
#define LIBRTMP_PUSH_QUEUE_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>

#define QUEUW_LENGTH 50

typedef struct node {
    //数据
    __uint8_t *data;
    __int32_t size;
    __int32_t type;
    struct node *next;
} q_node, *q_node_p;

typedef struct list {
    q_node_p front;
    q_node_p rear;
} q_list;

q_list *queue;

/**
 * 初始化
 */
int init_queue();

int empty_queue();

/**
 * 进队
 * @param node
 * @return
 */
int in_queue(q_node *node);

/**
 * 取数据
 * @return
 */
q_node_p out_queue();

/**
 * 销毁
 * @return
 */
int destroy_queue();

/**
 * 创建节点
 * @param data
 * @param size
 * @return
 */
q_node_p create_node(__uint8_t *data, __int32_t size);

#endif //LIBRTMP_PUSH_QUEUE_H
