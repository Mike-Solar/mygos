
// kernel.c

#include "os.h"

/*/
 * Following functions SHOULD be called ONLY ONE time here,
 * so just declared here ONCE and NOT included in file os.h.
 *
 * 以下函数应该只在这里调用一次，
 * 因此只在这里声明一次，也不应该在文件os.h中包含。
/*/

extern void uart_init(void);
extern void page_init(void);
extern void sched_init(void);
extern void schedule(void);
extern void os_main(void);

void
start_kernel(void)
{
    uart_init();
    uart_puts("Hello, RVOS!\n");

    page_init();

    sched_init();

    os_main();

    schedule();

    uart_puts("Would not go here!\n");
    while(1); // stop here!
}
