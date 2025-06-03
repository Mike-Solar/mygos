
// kernel.c

#include "os.h"


extern void uart_init();  // 初始化串口
extern void plic_init();  // 初始化外部中断控制器
extern void page_init();  // 初始化分页
extern void sched_init(); // 初始化调度器
extern void trap_init();  // 初始化异常/中断处理
extern void timer_init(); // 初始化定时器
extern void os_main();    // 创建任务


void
start_kernel()
{
    uart_init();
    uart_puts("Hello, RVOS!\n");

    plic_init();
    page_init();
    sched_init();
    trap_init();
    timer_init();

    os_main();

    schedule();

    uart_puts("Would not go here!\n");
    while(1); // stop here!
}
