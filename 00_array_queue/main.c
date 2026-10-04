/**
 * @file    main.c
 * @brief   陣列佇列 Demo
 *
 * 編譯：gcc -Wall -Wextra -std=c99 -o array_queue_demo main.c array_queue.c
 */
#include <stdio.h>

#include "array_queue.h"

int main(void)
{
    ArrayQueue q;
    int v;

    aq_init(&q);
    printf("初始化 (容量 %d)\n", AQ_CAPACITY);
    aq_print(&q);

    printf("\nenqueue 10, 20, 30, 40, 50\n");
    for (int i = 1; i <= 5; ++i) aq_enqueue(&q, i * 10);
    aq_print(&q);

    printf("\n滿了再 enqueue 60 -> %s\n", aq_enqueue(&q, 60) ? "成功" : "失敗 (FULL)");

    aq_peek(&q, &v);
    printf("\npeek -> %d (只看不取)\n", v);

    printf("\ndequeue 兩次\n");
    aq_dequeue(&q, &v); printf("  取出 %d\n", v);
    aq_dequeue(&q, &v); printf("  取出 %d\n", v);
    aq_print(&q);

    printf("\nenqueue 60, 70 (rear 繞回 index 0，重複利用前面的空位)\n");
    aq_enqueue(&q, 60);
    aq_enqueue(&q, 70);
    aq_print(&q);

    printf("\n全部 dequeue (依照 FIFO 順序)\n");
    while (aq_dequeue(&q, &v)) {
        printf("  取出 %d\n", v);
    }
    aq_print(&q);

    printf("\n空的再 dequeue -> %s\n", aq_dequeue(&q, &v) ? "成功" : "失敗 (EMPTY)");
    return 0;
}
