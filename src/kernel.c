
// kernel.c

#include "os.h"

/*/
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 *
 * 以下函数应该只在这里调用一次，
 * 因此只在这里声明一次，也不应该在文件os.h中包含。
/*/

extern void uart_init(void);  // 初始化 uart
extern void page_init(void);  // 初始化分页
extern void sched_init(void); // 初始化调度器
extern void schedule(void);   // 开始调度
extern void trap_init(void);  // 初始化异常/中断处理
extern void plic_init(void);  // 初始化外部中断处理
extern void os_main(void);    // 创建任务


void
start_kernel(void)
{
    uart_init();
    uart_puts("Hello, RVOS!\n");

    page_init();
    sched_init();
    trap_init();
    plic_init();

    os_main();

    schedule();

    uart_puts("Would not go here!\n");
    while(1); // stop here!
}
