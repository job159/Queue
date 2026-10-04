/**
 * @file    app_uart.c
 * @brief   應用 1：UART 接收緩衝 + 指令解析
 */
#include "app_uart.h"

#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "ring_buffer.h"

#define UART_RX_BUF_SIZE 64u   /* 2 的冪次 */
#define LINE_MAX         32u

static uint8_t    s_rx_storage[UART_RX_BUF_SIZE];
static RingBuffer s_rx_rb;
static uint32_t   s_overflow;

static char       s_line[LINE_MAX];
static uint32_t   s_line_len;
static bool       s_line_too_long;
static bool       s_led_on;

void uart_app_init(void)
{
    rb_init(&s_rx_rb, s_rx_storage, UART_RX_BUF_SIZE, sizeof(uint8_t));
    s_overflow = 0;
    s_line_len      = 0;
    s_line_too_long = false;
    s_led_on        = false;
}

void uart_rx_isr(uint8_t byte)
{
    if (!rb_push(&s_rx_rb, &byte)) {
        s_overflow++;           /* 緩衝區滿：丟掉新資料並計數 */
    }
}

static void execute_command(const char *cmd)
{
    printf("  [CMD] \"%s\" -> ", cmd);

    if (strcmp(cmd, "LED ON") == 0) {
        s_led_on = true;
        printf("LED 已開啟\n");
    } else if (strcmp(cmd, "LED OFF") == 0) {
        s_led_on = false;
        printf("LED 已關閉\n");
    } else if (strncmp(cmd, "ECHO ", 5) == 0) {
        printf("%s\n", cmd + 5);
    } else if (strcmp(cmd, "STATUS") == 0) {
        printf("LED=%s, rx_buf=%u/%u, overflow=%u\n",
               s_led_on ? "ON" : "OFF",
               (unsigned)rb_count(&s_rx_rb), (unsigned)UART_RX_BUF_SIZE,
               (unsigned)s_overflow);
    } else {
        printf("未知指令\n");
    }
}

void uart_process(void)
{
    uint8_t ch;

    while (rb_pop(&s_rx_rb, &ch)) {
        if (ch == '\r') continue;

        if (ch == '\n') {
            s_line[s_line_len] = '\0';
            if (s_line_too_long) {
                printf("  [CMD] 行過長 (可能遺失資料)，丟棄\n");
            } else if (s_line_len > 0) {
                execute_command(s_line);
            }
            s_line_len      = 0;
            s_line_too_long = false;
        } else if (s_line_len < LINE_MAX - 1) {
            s_line[s_line_len++] = (char)ch;
        } else {
            s_line_too_long = true;   /* 標記本行無效，等下一個 '\n' 重新同步 */
        }
    }
}

uint32_t uart_overflow_count(void)
{
    return s_overflow;
}
