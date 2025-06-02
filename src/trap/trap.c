
// trap.c

#include "os.h"


extern void trap_vector(void);
extern void uart_isr(void);


void
trap_init()
{
    // set the trap-vector base-address for machine-mode
    w_mtvec((reg_t)trap_vector);
}

void
external_interrupt_handler()
{
    // 返回中断源号
    int irq = plic_claim();

    // 处理中断
    if(irq == UART0_IRQ) uart_isr();
    else if(irq) printf("unexpected interrupt irq = %d\n", irq);

    // 将中断标记为已响应
    if(irq) plic_complete(irq);
}


void
trap_test()
{
    /*/
     * Synchronous exception code = 7
     * Store/AMO access fault
    /*/
    *(int*)0x00000000 = 100;

    /*/
     * Synchronous exception code = 5
     * Load access fault
    /*/
    // int a = *(int*)0x00000000;

    uart_puts("Yeah! I'm return back from trap!\n");
}
