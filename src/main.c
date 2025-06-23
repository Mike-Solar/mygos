
// main.c

#include "device_tree_parser.h"
#include <page.h>
extern void uart_init();         // 初始化串口
extern void plic_init();         // 初始化外部中断控制器
extern void page_init();         // 初始化分页
extern void trap_init();         // 初始化异常/中断处理
extern void timer_init();        // 初始化定时器
extern void create_user_tasks(); // 创建任务
extern void sched_init();        // 初始化调度器、开始任务调度

extern void uart_puts(char* s);  // 输出字符串到串口（逐字符发送）
pte_t kernel_pagetable[PAGE_SIZE / sizeof(pte_t)];
pte_t early_pt_2[PAGE_SIZE / sizeof(pte_t)]__attribute__((section(".early_page")));
pte_t early_pt_1[PAGE_SIZE / sizeof(pte_t)]__attribute__((section(".early_page")));
pte_t early_pt_0[PAGE_SIZE / sizeof(pte_t)][PAGE_SIZE / sizeof(pte_t)] __attribute__((section(".early_page")));

void
start_kernel(int hart_id, void *dtd)
{
    parse_device_tree(dtd);
    uart_init();
    uart_puts("Hello, It's MyGOS!!!!!\n");

    plic_init();
    page_init();
    trap_init();
    timer_init();

    create_user_tasks();

    sched_init();

    uart_puts("Would not go here!\n");
    while(1); // stop here!
}
