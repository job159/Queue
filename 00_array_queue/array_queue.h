/**
 * @file    array_queue.h
 * @brief   陣列佇列 (Array Queue) —— 最入門的教科書版本
 *
 * 用一個固定大小的陣列 + 三個變數實作 FIFO：
 *
 *     front : 下一個要取出 (dequeue) 的位置
 *     rear  : 下一個要放入 (enqueue) 的位置
 *     count : 目前元素個數 (用來分辨「空」與「滿」)
 *
 * 索引走到陣列尾端時用 `% CAPACITY` 繞回 0，
 * 所以前面空出來的位置可以重複使用，不會有假溢位。
 *
 *   enqueue(10,20,30) 後 dequeue 一次：
 *
 *     index:   0    1    2    3    4
 *            +----+----+----+----+----+
 *            |    | 20 | 30 |    |    |
 *            +----+----+----+----+----+
 *                   ^         ^
 *                 front      rear        count = 2
 *
 * 學習路線：00 陣列佇列 -> 01 線性陣列 vs 鏈結串列 -> 02 環形緩衝區 (泛型 + 應用)
 */
#ifndef ARRAY_QUEUE_H
#define ARRAY_QUEUE_H

#include <stdbool.h>

#define AQ_CAPACITY 5

typedef struct {
    int data[AQ_CAPACITY];
    int front;
    int rear;
    int count;
} ArrayQueue;

void aq_init(ArrayQueue *q);
bool aq_is_empty(const ArrayQueue *q);
bool aq_is_full(const ArrayQueue *q);
int  aq_size(const ArrayQueue *q);

bool aq_enqueue(ArrayQueue *q, int value);   /* 滿了回傳 false */
bool aq_dequeue(ArrayQueue *q, int *value);  /* 空的回傳 false */
bool aq_peek(const ArrayQueue *q, int *value);

void aq_print(const ArrayQueue *q);

#endif /* ARRAY_QUEUE_H */
