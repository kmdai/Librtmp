//
// Created by kmdai on 18-4-21.
// the queue for rtmp
//

#ifndef LIBRTMP_PUSH_QUEUE_H
#define LIBRTMP_PUSH_QUEUE_H

#include <stdio.h>
#include <malloc.h>
#include <string.h>
#include <pthread.h>
#include "push_utils.h"
#ifdef __cplusplus
extern "C"
{
#endif
/**
 *
 */
#define QUEUE_MAX_LENGTH (1024 * 1024 * 1)
/**
 * sps|pps|aac
 */
#define NODE_FLAG_CODEC_CONFIG 2
/**
 * 关键帧
 */
#define NODE_FLAG_KEY_FRAME 1
typedef enum _node_tpye {
    NODE_TYPE_AUDIO = 1,
    NODE_TYPE_VIDEO
} node_type;
/**
 *p帧
 */
#define NODE_FLAG_PARTIAL_FRAME 8
typedef struct node {
    //数据
    char *data;
    uint32_t size;
    int32_t flag;
    node_type type;
    uint32_t time;
    struct node *next;
} q_node, *q_node_p;

typedef struct list {
    q_node_p front;
    q_node_p rear;
    pthread_mutex_t mutex;
    pthread_cond_t cond;
    int32_t length;
} q_list;


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

void cancel_queue();

/**
 * 创建节点
 * @param data
 * @param size
 * @param flag
 * @return
 */
q_node_p create_node(char *data, uint32_t size, node_type type, int32_t flag, uint32_t time);

#ifdef __cplusplus
};
#endif
#endif //LIBRTMP_PUSH_QUEUE_H
