/**
 * @file    app_filter.c
 * @brief   應用 2：移動平均濾波器
 */
#include "app_filter.h"

void ma_init(MovingAverage *ma)
{
    rb_init(&ma->rb, ma->storage, MA_WINDOW, sizeof(int16_t));
    ma->sum = 0;
}

int16_t ma_update(MovingAverage *ma, int16_t sample)
{
    if (rb_is_full(&ma->rb)) {
        int16_t oldest;
        rb_peek(&ma->rb, 0, &oldest);
        ma->sum -= oldest;
    }
    rb_push_overwrite(&ma->rb, &sample);
    ma->sum += sample;

    return (int16_t)(ma->sum / (int32_t)rb_count(&ma->rb));
}
