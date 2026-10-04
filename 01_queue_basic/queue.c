/**
 * @file    queue.c
 * @brief   Queue 基礎設計實作
 */
#include "queue.h"

#include <stdio.h>
#include <stdlib.h>

const char *queue_status_str(QueueStatus st)
{
    switch (st) {
    case QUEUE_OK:        return "OK";
    case QUEUE_ERR_FULL:  return "FULL";
    case QUEUE_ERR_EMPTY: return "EMPTY";
    case QUEUE_ERR_NULL:  return "NULL";
    case QUEUE_ERR_NOMEM: return "NOMEM";
    default:              return "UNKNOWN";
    }
}

/* ===========================================================================
 * 1. ArrayQueue
 * ========================================================================= */
void aq_init(ArrayQueue *q)
{
    if (q == NULL) return;
    q->front = 0;
    q->rear  = 0;
}

bool aq_is_empty(const ArrayQueue *q)
{
    return q->front == q->rear;
}

bool aq_is_full(const ArrayQueue *q)
{
    /* 注意：只看 rear 是否到底。即使前面有空位也會判定為滿 -> 假溢位 */
    return q->rear == ARRAY_QUEUE_CAPACITY;
}

size_t aq_size(const ArrayQueue *q)
{
    return (size_t)(q->rear - q->front);
}

QueueStatus aq_enqueue(ArrayQueue *q, int value)
{
    if (q == NULL)     return QUEUE_ERR_NULL;
    if (aq_is_full(q)) return QUEUE_ERR_FULL;

    q->data[q->rear++] = value;
    return QUEUE_OK;
}

QueueStatus aq_dequeue(ArrayQueue *q, int *out)
{
    if (q == NULL || out == NULL) return QUEUE_ERR_NULL;
    if (aq_is_empty(q))           return QUEUE_ERR_EMPTY;

    *out = q->data[q->front++];

    /* 小優化：佇列清空時把索引歸零，可以「部分」緩解假溢位 */
    if (aq_is_empty(q)) {
        q->front = 0;
        q->rear  = 0;
    }
    return QUEUE_OK;
}

QueueStatus aq_peek(const ArrayQueue *q, int *out)
{
    if (q == NULL || out == NULL) return QUEUE_ERR_NULL;
    if (aq_is_empty(q))           return QUEUE_ERR_EMPTY;

    *out = q->data[q->front];
    return QUEUE_OK;
}

void aq_print(const ArrayQueue *q)
{
    printf("ArrayQueue [front=%d rear=%d size=%zu] : ",
           q->front, q->rear, aq_size(q));
    for (int i = 0; i < ARRAY_QUEUE_CAPACITY; ++i) {
        if (i >= q->front && i < q->rear)
            printf("%3d ", q->data[i]);
        else
            printf("  . ");
    }
    printf("\n");
}

/* ===========================================================================
 * 2. LinkedQueue
 * ========================================================================= */
void lq_init(LinkedQueue *q)
{
    if (q == NULL) return;
    q->head = NULL;
    q->tail = NULL;
    q->size = 0;
}

void lq_destroy(LinkedQueue *q)
{
    if (q == NULL) return;
    QueueNode *cur = q->head;
    while (cur != NULL) {
        QueueNode *next = cur->next;
        free(cur);
        cur = next;
    }
    lq_init(q);
}

bool lq_is_empty(const LinkedQueue *q)
{
    return q->size == 0;
}

size_t lq_size(const LinkedQueue *q)
{
    return q->size;
}

QueueStatus lq_enqueue(LinkedQueue *q, int value)
{
    if (q == NULL) return QUEUE_ERR_NULL;

    QueueNode *node = (QueueNode *)malloc(sizeof(*node));
    if (node == NULL) return QUEUE_ERR_NOMEM;
    node->value = value;
    node->next  = NULL;

    if (q->tail == NULL) {          /* 空佇列：head 和 tail 指向同一個節點 */
        q->head = node;
    } else {
        q->tail->next = node;
    }
    q->tail = node;
    q->size++;
    return QUEUE_OK;
}

QueueStatus lq_dequeue(LinkedQueue *q, int *out)
{
    if (q == NULL || out == NULL) return QUEUE_ERR_NULL;
    if (lq_is_empty(q))           return QUEUE_ERR_EMPTY;

    QueueNode *node = q->head;
    *out    = node->value;
    q->head = node->next;
    if (q->head == NULL) {          /* 取完最後一個，tail 也要清掉 */
        q->tail = NULL;
    }
    free(node);
    q->size--;
    return QUEUE_OK;
}

QueueStatus lq_peek(const LinkedQueue *q, int *out)
{
    if (q == NULL || out == NULL) return QUEUE_ERR_NULL;
    if (lq_is_empty(q))           return QUEUE_ERR_EMPTY;

    *out = q->head->value;
    return QUEUE_OK;
}

void lq_print(const LinkedQueue *q)
{
    printf("LinkedQueue [size=%zu] : head -> ", q->size);
    for (const QueueNode *n = q->head; n != NULL; n = n->next) {
        printf("%d -> ", n->value);
    }
    printf("NULL\n");
}
