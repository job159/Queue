/**
 * @file    ring_buffer.h
 * @brief   環形緩衝區 (Ring Buffer / Circular Buffer)
 *
 * 設計重點
 * --------
 * 1. 不使用 malloc：儲存空間由呼叫者提供 (static 陣列)，適合嵌入式。
 * 2. 泛型：每個元素大小 item_size 由 init 指定，可放 uint8_t、int、struct...
 * 3. 容量必須是 2 的冪次 (2, 4, 8, 16 ...)，
 *    索引用 `& mask` 取代 `% capacity`，速度快。
 * 4. head / tail 是「一直遞增」的 32-bit 計數器（不在寫入時取餘數）：
 *        count = head - tail      (unsigned 溢位後相減仍正確)
 *        空    : head == tail
 *        滿    : head - tail == capacity
 *    -> 不需要犧牲一格來分辨空/滿，容量 100% 可用。
 * 5. 單一生產者 / 單一消費者 (SPSC) 安全：
 *    生產者只改 head，消費者只改 tail，
 *    例如 ISR 寫入、main loop 讀取時不需要關中斷。
 *    （rb_push_overwrite 會由生產者改動 tail，不屬於 SPSC 安全範圍）
 *
 *          tail(讀)                head(寫)
 *            v                       v
 *    +---+---+---+---+---+---+---+---+
 *    |   |   | A | B | C | D | E |   |   <- 寫到尾端後繞回 index 0
 *    +---+---+---+---+---+---+---+---+
 */
#ifndef RING_BUFFER_H
#define RING_BUFFER_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

typedef struct {
    uint8_t          *buf;        /* 使用者提供的儲存空間            */
    size_t            item_size;  /* 每個元素的 byte 數              */
    uint32_t          capacity;   /* 可存放的元素數 (2 的冪次)       */
    uint32_t          mask;       /* capacity - 1                    */
    volatile uint32_t head;       /* 寫入計數 (生產者)               */
    volatile uint32_t tail;       /* 讀取計數 (消費者)               */
} RingBuffer;

/**
 * @brief 初始化
 * @param storage   至少 capacity * item_size bytes 的空間
 * @param capacity  元素個數，必須是 2 的冪次
 * @return false 表示參數錯誤
 */
bool     rb_init(RingBuffer *rb, void *storage, uint32_t capacity, size_t item_size);
void     rb_reset(RingBuffer *rb);

uint32_t rb_count(const RingBuffer *rb);     /* 目前元素數 */
uint32_t rb_free(const RingBuffer *rb);      /* 剩餘空間   */
bool     rb_is_empty(const RingBuffer *rb);
bool     rb_is_full(const RingBuffer *rb);

/* ---- 單筆操作 ---- */
bool     rb_push(RingBuffer *rb, const void *item);            /* 滿了回傳 false      */
void     rb_push_overwrite(RingBuffer *rb, const void *item);  /* 滿了就覆蓋最舊資料  */
bool     rb_pop(RingBuffer *rb, void *item);                   /* 空的回傳 false      */
bool     rb_peek(const RingBuffer *rb, uint32_t index, void *item); /* index 0 = 最舊 */

/* ---- 批次操作 (最多兩次 memcpy) ---- */
uint32_t rb_write(RingBuffer *rb, const void *items, uint32_t n); /* 回傳實際寫入數 */
uint32_t rb_read(RingBuffer *rb, void *items, uint32_t n);        /* 回傳實際讀出數 */
uint32_t rb_discard(RingBuffer *rb, uint32_t n);                  /* 丟棄最舊 n 筆   */

#endif /* RING_BUFFER_H */
