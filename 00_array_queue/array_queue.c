/**
 * @file    array_queue.c
 * @brief   陣列佇列實作
 */
#include "array_queue.h"

#include <stdio.h>

void aq_init(ArrayQueue *q)
{
    q->front = 0;
    q->rear  = 0;
    q->count = 0;
}

bool aq_is_empty(const ArrayQueue *q)
{
    return q->count == 0;
}

bool aq_is_full(const ArrayQueue *q)
{
    return q->count == AQ_CAPACITY;
}

int aq_size(const ArrayQueue *q)
{
    return q->count;
}

bool aq_enqueue(ArrayQueue *q, int value)
{
    if (aq_is_full(q)) return false;

    q->data[q->rear] = value;
    q->rear = (q->rear + 1) % AQ_CAPACITY;   /* 走到尾端就繞回 0 */
    q->count++;
    return true;
}

bool aq_dequeue(ArrayQueue *q, int *value)
{
    if (aq_is_empty(q)) return false;

    *value   = q->data[q->front];
    q->front = (q->front + 1) % AQ_CAPACITY;
    q->count--;
    return true;
}

bool aq_peek(const ArrayQueue *q, int *value)
{
    if (aq_is_empty(q)) return false;

    *value = q->data[q->front];
    return true;
}

void aq_print(const ArrayQueue *q)
{
    printf("  [");
    for (int i = 0; i < AQ_CAPACITY; ++i) {
        /* i 距離 front 幾格？小於 count 就是有效資料 */
        int offset = (i - q->front + AQ_CAPACITY) % AQ_CAPACITY;
        if (offset < q->count) printf("%4d", q->data[i]);
        else                   printf("   .");
    }
    printf(" ]  front=%d rear=%d count=%d%s%s\n",
           q->front, q->rear, q->count,
           aq_is_full(q) ? " FULL" : "", aq_is_empty(q) ? " EMPTY" : "");
}
