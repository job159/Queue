/**
 * @file    app_filter.h
 * @brief   應用 2：移動平均濾波器 (Moving Average Filter)
 *
 * 使用「覆蓋模式」環形緩衝區保存最近 N 筆感測值，
 * 並維護一個累加和 sum：
 *     新值進來 -> sum += 新值
 *     若已滿   -> sum -= 最舊值 (即將被覆蓋)
 * 因此每次更新都是 O(1)，不需要每次重新加總 N 筆。
 */
#ifndef APP_FILTER_H
#define APP_FILTER_H

#include <stdint.h>

#include "ring_buffer.h"

#define MA_WINDOW 8u   /* 視窗大小，2 的冪次 */

typedef struct {
    RingBuffer rb;
    int16_t    storage[MA_WINDOW];
    int32_t    sum;
} MovingAverage;

void    ma_init(MovingAverage *ma);
int16_t ma_update(MovingAverage *ma, int16_t sample);  /* 回傳目前平均值 */

#endif /* APP_FILTER_H */
