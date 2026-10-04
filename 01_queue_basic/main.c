/**
 * @file    main.c
 * @brief   Queue 基礎設計 Demo
 *
 * 編譯：gcc -Wall -Wextra -std=c99 -o queue_demo main.c queue.c
 */
#include <stdio.h>

#include "queue.h"

static void demo_array_queue(void)
{
    printf("========== 1. ArrayQueue (線性陣列) ==========\n");

    ArrayQueue q;
    aq_init(&q);
    int v;

    for (int i = 1; i <= 5; ++i) {
        aq_enqueue(&q, i * 10);
    }
    aq_print(&q);

    printf("再 enqueue 60 -> %s\n", queue_status_str(aq_enqueue(&q, 60)));

    aq_dequeue(&q, &v); printf("dequeue -> %d\n", v);
    aq_dequeue(&q, &v); printf("dequeue -> %d\n", v);
    aq_print(&q);

    /* 前面已空出 2 格，但 rear 已到底 -> 仍然 FULL，這就是「假溢位」 */
    printf("前面有空位，但 enqueue 70 -> %s  <-- 假溢位!\n",
           queue_status_str(aq_enqueue(&q, 70)));

    while (aq_dequeue(&q, &v) == QUEUE_OK) {
        printf("dequeue -> %d\n", v);
    }
    printf("空佇列 dequeue -> %s\n", queue_status_str(aq_dequeue(&q, &v)));
    aq_print(&q);
    printf("\n");
}

static void demo_linked_queue(void)
{
    printf("========== 2. LinkedQueue (鏈結串列) ==========\n");

    LinkedQueue q;
    lq_init(&q);
    int v;

    for (int i = 1; i <= 5; ++i) {
        lq_enqueue(&q, i * 100);
    }
    lq_print(&q);

    lq_peek(&q, &v);
    printf("peek -> %d (不移除)\n", v);

    lq_dequeue(&q, &v); printf("dequeue -> %d\n", v);
    lq_dequeue(&q, &v); printf("dequeue -> %d\n", v);
    lq_enqueue(&q, 600);
    lq_enqueue(&q, 700);
    lq_print(&q);

    lq_destroy(&q);
    printf("destroy 後 size = %zu\n\n", lq_size(&q));
}

/* 應用小例子：印表機工作排程 (FIFO 依序處理) */
static void demo_print_jobs(void)
{
    printf("========== 3. 應用：印表機工作排程 ==========\n");

    LinkedQueue jobs;
    lq_init(&jobs);

    int job_pages[] = {3, 1, 5, 2};
    for (size_t i = 0; i < sizeof(job_pages) / sizeof(job_pages[0]); ++i) {
        lq_enqueue(&jobs, job_pages[i]);
        printf("送出工作 #%zu (%d 頁)\n", i + 1, job_pages[i]);
    }

    int pages, id = 1;
    while (lq_dequeue(&jobs, &pages) == QUEUE_OK) {
        printf("列印工作 #%d：%d 頁 ... 完成 (剩 %zu 個工作)\n",
               id++, pages, lq_size(&jobs));
    }
    lq_destroy(&jobs);
}

int main(void)
{
    demo_array_queue();
    demo_linked_queue();
    demo_print_jobs();
    return 0;
}
