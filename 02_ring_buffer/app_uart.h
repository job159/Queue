/**
 * @file    app_uart.h
 * @brief   應用 1：UART 接收緩衝 + 指令解析
 *
 * 典型嵌入式架構：
 *
 *   [UART 硬體] --中斷--> uart_rx_isr()  --push-->  RingBuffer  --pop-->  uart_process()
 *                         (生產者，越短越好)                              (消費者，main loop)
 *
 * ISR 只負責把 byte 丟進環形緩衝區，耗時的字串解析留給 main loop。
 * 支援指令（以 '\n' 結尾）：
 *   LED ON / LED OFF / ECHO <text> / STATUS
 */
#ifndef APP_UART_H
#define APP_UART_H

#include <stdint.h>

void     uart_app_init(void);
void     uart_rx_isr(uint8_t byte);   /* 模擬：UART 收到 1 byte 時的中斷 */
void     uart_process(void);          /* 在 main loop 中呼叫               */
uint32_t uart_overflow_count(void);   /* 緩衝區滿而遺失的 byte 數          */

#endif /* APP_UART_H */
