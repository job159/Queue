/**
 * @file    main.c
 * @brief   環形緩衝區 Demo + 應用
 *
 * 編譯：gcc -Wall -Wextra -std=c99 -o ring_demo main.c ring_buffer.c app_uart.c app_filter.c
 */
#include <stdio.h>
#include <string.h>

#include "app_filter.h"
#include "app_uart.h"
#include "ring_buffer.h"

/* 印出 int 環形緩衝區的實體陣列內容，觀察繞回 (wrap-around) */
static void print_int_rb(const RingBuffer *rb)
{
    const int *arr = (const int *)rb->buf;
    uint32_t   h   = rb->head & rb->mask;
    uint32_t   t   = rb->tail & rb->mask;

    printf("  [");
    for (uint32_t i = 0; i < rb->capacity; ++i) {
        /* 判斷 index i 是否在有效資料區間內 */
        uint32_t offset = (i - t) & rb->mask;
        bool     valid  = offset < rb_count(rb);
        if (valid) printf("%4d", arr[i]);
        else       printf("   .");
    }
    printf(" ]  tail=%u head=%u count=%u%s%s\n",
           (unsigned)t, (unsigned)h, (unsigned)rb_count(rb),
           rb_is_full(rb) ? " FULL" : "", rb_is_empty(rb) ? " EMPTY" : "");
}

/* ===========================================================================
 * Demo 0：基本操作 —— 對照 01_queue_basic 的「假溢位」
 * ========================================================================= */
static void demo_basic(void)
{
    printf("========== 0. 基本操作 (容量 8) ==========\n");

    static int storage[8];
    RingBuffer rb;
    rb_init(&rb, storage, 8, sizeof(int));

    for (int i = 1; i <= 8; ++i) rb_push(&rb, &(int){i * 10});
    print_int_rb(&rb);

    int v = 90;
    printf("滿了再 push 90 -> %s\n", rb_push(&rb, &v) ? "成功" : "失敗(FULL)");

    for (int i = 0; i < 3; ++i) {
        rb_pop(&rb, &v);
        printf("pop -> %d\n", v);
    }
    print_int_rb(&rb);

    printf("push 90, 100, 110 (寫到尾端後繞回 index 0，沒有假溢位)\n");
    for (int i = 9; i <= 11; ++i) rb_push(&rb, &(int){i * 10});
    print_int_rb(&rb);

    printf("rb_push_overwrite 120 (覆蓋最舊的 40)\n");
    rb_push_overwrite(&rb, &(int){120});
    print_int_rb(&rb);

    int out[8];
    uint32_t n = rb_read(&rb, out, 5);
    printf("rb_read 5 筆 ->");
    for (uint32_t i = 0; i < n; ++i) printf(" %d", out[i]);
    printf("\n");
    print_int_rb(&rb);
    printf("\n");
}

/* ===========================================================================
 * Demo 1：UART 接收 + 指令解析
 * ========================================================================= */
static void feed_uart(const char *s)
{
    /* 模擬硬體一個 byte 一個 byte 觸發中斷 */
    for (size_t i = 0; s[i] != '\0'; ++i) uart_rx_isr((uint8_t)s[i]);
}

static void demo_uart(void)
{
    printf("========== 1. 應用：UART 接收緩衝 ==========\n");
    uart_app_init();

    /* 情況 1：資料一次收進多行，main loop 之後才處理 */
    feed_uart("LED ON\r\nECHO hello ring buffer\r\n");
    printf("main loop 處理：\n");
    uart_process();

    /* 情況 2：一行資料被拆成兩次收到 (封包分段) */
    feed_uart("STA");
    uart_process();          /* 尚未收到 '\n'，不會執行 */
    feed_uart("TUS\n");
    uart_process();

    /* 情況 3：main loop 太久沒處理 -> 緩衝區溢位 */
    printf("連續灌入 100 bytes 而不處理...\n");
    for (int i = 0; i < 100; ++i) uart_rx_isr('x');
    uart_rx_isr('\n');
    printf("  overflow = %u bytes\n", (unsigned)uart_overflow_count());
    uart_process();
    /* 結尾的 '\n' 也在溢位時遺失了，傳送端先送一個 '\n' 重新同步 */
    feed_uart("\nLED OFF\n");
    uart_process();
    printf("\n");
}

/* ===========================================================================
 * Demo 2：移動平均濾波
 * ========================================================================= */
static void demo_filter(void)
{
    printf("========== 2. 應用：移動平均濾波 (視窗 %u) ==========\n",
           (unsigned)MA_WINDOW);

    MovingAverage ma;
    ma_init(&ma);

    /* 模擬含雜訊的感測器讀值，基準約 100，中間有一個尖波 300 */
    const int16_t samples[] = {98, 103, 97, 101, 300, 99, 102, 100,
                               96, 104, 99, 101, 98, 102};

    printf("  原始值  平均值\n");
    for (size_t i = 0; i < sizeof(samples) / sizeof(samples[0]); ++i) {
        int16_t avg = ma_update(&ma, samples[i]);
        printf("  %5d   %5d\n", samples[i], avg);
    }
    printf("\n");
}

/* ===========================================================================
 * Demo 3：事件佇列 (存放 struct)
 * ========================================================================= */
typedef enum { EVT_BUTTON, EVT_TIMER, EVT_SENSOR } EventType;

typedef struct {
    EventType type;
    int       param;
} Event;

static const char *event_name(EventType t)
{
    switch (t) {
    case EVT_BUTTON: return "BUTTON";
    case EVT_TIMER:  return "TIMER";
    case EVT_SENSOR: return "SENSOR";
    default:         return "?";
    }
}

static void demo_event_queue(void)
{
    printf("========== 3. 應用：事件佇列 (元素是 struct) ==========\n");

    static Event storage[16];
    RingBuffer   evq;
    rb_init(&evq, storage, 16, sizeof(Event));

    /* 各個中斷 / 模組丟事件進來 */
    rb_push(&evq, &(Event){EVT_BUTTON, 1});
    rb_push(&evq, &(Event){EVT_TIMER, 1000});
    rb_push(&evq, &(Event){EVT_SENSOR, 25});
    rb_push(&evq, &(Event){EVT_BUTTON, 2});

    Event first;
    rb_peek(&evq, 0, &first);
    printf("  待處理事件 %u 個，最早的是 %s\n",
           (unsigned)rb_count(&evq), event_name(first.type));

    /* main loop：依序處理 */
    Event e;
    while (rb_pop(&evq, &e)) {
        printf("  處理事件 %-6s param=%d\n", event_name(e.type), e.param);
    }
}

int main(void)
{
    demo_basic();
    demo_uart();
    demo_filter();
    demo_event_queue();
    return 0;
}
