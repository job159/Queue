/**
 * @file    queue.h
 * @brief   Queue（佇列）基礎設計 —— FIFO（先進先出）
 *
 * 本檔提供兩種實作，方便對照學習：
 *
 *   1. ArrayQueue  : 固定大小陣列 + 線性 front / rear
 *                    -> 最直觀，但會出現「假溢位」(false overflow)：
 *                       前面 dequeue 掉的空間無法再使用。
 *                       這正是下一個資料夾「環形緩衝區」要解決的問題。
 *
 *   2. LinkedQueue : 單向鏈結串列 (head 出、tail 進)
 *                    -> 容量只受記憶體限制，enqueue / dequeue 都是 O(1)。
 *
 *   enqueue  ->  [rear/tail] ... [front/head]  ->  dequeue
 */
#ifndef QUEUE_H
#define QUEUE_H

#include <stdbool.h>
#include <stddef.h>

/* ---------------------------------------------------------------------------
 * 共用回傳碼
 * ------------------------------------------------------------------------- */
typedef enum {
    QUEUE_OK = 0,
    QUEUE_ERR_FULL,     /* 佇列已滿 */
    QUEUE_ERR_EMPTY,    /* 佇列為空 */
    QUEUE_ERR_NULL,     /* 傳入 NULL 指標 */
    QUEUE_ERR_NOMEM     /* malloc 失敗 */
} QueueStatus;

const char *queue_status_str(QueueStatus st);

/* ===========================================================================
 * 1. ArrayQueue —— 線性陣列佇列
 * ========================================================================= */
#define ARRAY_QUEUE_CAPACITY 5

typedef struct {
    int data[ARRAY_QUEUE_CAPACITY];
    int front;  /* 下一個要取出的位置 */
    int rear;   /* 下一個要放入的位置 */
} ArrayQueue;

void        aq_init(ArrayQueue *q);
bool        aq_is_empty(const ArrayQueue *q);
bool        aq_is_full(const ArrayQueue *q);   /* rear 到底就算滿（假溢位） */
size_t      aq_size(const ArrayQueue *q);
QueueStatus aq_enqueue(ArrayQueue *q, int value);
QueueStatus aq_dequeue(ArrayQueue *q, int *out);
QueueStatus aq_peek(const ArrayQueue *q, int *out);
void        aq_print(const ArrayQueue *q);

/* ===========================================================================
 * 2. LinkedQueue —— 鏈結串列佇列
 * ========================================================================= */
typedef struct QueueNode {
    int               value;
    struct QueueNode *next;
} QueueNode;

typedef struct {
    QueueNode *head;    /* 出口 (front) */
    QueueNode *tail;    /* 入口 (rear)  */
    size_t     size;
} LinkedQueue;

void        lq_init(LinkedQueue *q);
void        lq_destroy(LinkedQueue *q);        /* 釋放所有節點 */
bool        lq_is_empty(const LinkedQueue *q);
size_t      lq_size(const LinkedQueue *q);
QueueStatus lq_enqueue(LinkedQueue *q, int value);
QueueStatus lq_dequeue(LinkedQueue *q, int *out);
QueueStatus lq_peek(const LinkedQueue *q, int *out);
void        lq_print(const LinkedQueue *q);

#endif /* QUEUE_H */
