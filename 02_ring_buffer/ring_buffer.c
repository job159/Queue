/**
 * @file    ring_buffer.c
 * @brief   環形緩衝區實作
 */
#include "ring_buffer.h"

#include <string.h>

/* 位置 (計數器) -> 實際陣列中的 byte 指標 */
static inline uint8_t *slot(const RingBuffer *rb, uint32_t pos)
{
    return rb->buf + (size_t)(pos & rb->mask) * rb->item_size;
}

static inline bool is_power_of_two(uint32_t x)
{
    return x != 0 && (x & (x - 1)) == 0;
}

/*
 * 將 n 個元素從 src 拷貝到環形空間 pos 開始處。
 * 若跨越陣列尾端，拆成兩段 memcpy。
 */
static void copy_in(RingBuffer *rb, uint32_t pos, const uint8_t *src, uint32_t n)
{
    uint32_t idx   = pos & rb->mask;
    uint32_t first = rb->capacity - idx;          /* 到陣列尾端前可放幾個 */
    if (first > n) first = n;

    memcpy(rb->buf + (size_t)idx * rb->item_size, src, (size_t)first * rb->item_size);
    memcpy(rb->buf, src + (size_t)first * rb->item_size,
           (size_t)(n - first) * rb->item_size);
}

static void copy_out(const RingBuffer *rb, uint32_t pos, uint8_t *dst, uint32_t n)
{
    uint32_t idx   = pos & rb->mask;
    uint32_t first = rb->capacity - idx;
    if (first > n) first = n;

    memcpy(dst, rb->buf + (size_t)idx * rb->item_size, (size_t)first * rb->item_size);
    memcpy(dst + (size_t)first * rb->item_size, rb->buf,
           (size_t)(n - first) * rb->item_size);
}

/* ------------------------------------------------------------------------- */

bool rb_init(RingBuffer *rb, void *storage, uint32_t capacity, size_t item_size)
{
    if (rb == NULL || storage == NULL || item_size == 0) return false;
    if (!is_power_of_two(capacity) || capacity > 0x80000000u) return false;

    rb->buf       = (uint8_t *)storage;
    rb->item_size = item_size;
    rb->capacity  = capacity;
    rb->mask      = capacity - 1;
    rb->head      = 0;
    rb->tail      = 0;
    return true;
}

void rb_reset(RingBuffer *rb)
{
    rb->head = 0;
    rb->tail = 0;
}

uint32_t rb_count(const RingBuffer *rb)
{
    return rb->head - rb->tail;   /* unsigned 溢位相減仍正確 */
}

uint32_t rb_free(const RingBuffer *rb)
{
    return rb->capacity - rb_count(rb);
}

bool rb_is_empty(const RingBuffer *rb)
{
    return rb->head == rb->tail;
}

bool rb_is_full(const RingBuffer *rb)
{
    return rb_count(rb) == rb->capacity;
}

bool rb_push(RingBuffer *rb, const void *item)
{
    if (rb_is_full(rb)) return false;

    memcpy(slot(rb, rb->head), item, rb->item_size);
    rb->head++;                   /* 先寫資料，再更新 head (SPSC 的關鍵順序) */
    return true;
}

void rb_push_overwrite(RingBuffer *rb, const void *item)
{
    if (rb_is_full(rb)) {
        rb->tail++;               /* 丟掉最舊的一筆 */
    }
    memcpy(slot(rb, rb->head), item, rb->item_size);
    rb->head++;
}

bool rb_pop(RingBuffer *rb, void *item)
{
    if (rb_is_empty(rb)) return false;

    memcpy(item, slot(rb, rb->tail), rb->item_size);
    rb->tail++;                   /* 先讀資料，再更新 tail */
    return true;
}

bool rb_peek(const RingBuffer *rb, uint32_t index, void *item)
{
    if (index >= rb_count(rb)) return false;

    memcpy(item, slot(rb, rb->tail + index), rb->item_size);
    return true;
}

uint32_t rb_write(RingBuffer *rb, const void *items, uint32_t n)
{
    uint32_t space = rb_free(rb);
    if (n > space) n = space;
    if (n == 0) return 0;

    copy_in(rb, rb->head, (const uint8_t *)items, n);
    rb->head += n;
    return n;
}

uint32_t rb_read(RingBuffer *rb, void *items, uint32_t n)
{
    uint32_t avail = rb_count(rb);
    if (n > avail) n = avail;
    if (n == 0) return 0;

    copy_out(rb, rb->tail, (uint8_t *)items, n);
    rb->tail += n;
    return n;
}

uint32_t rb_discard(RingBuffer *rb, uint32_t n)
{
    uint32_t avail = rb_count(rb);
    if (n > avail) n = avail;
    rb->tail += n;
    return n;
}
