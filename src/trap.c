
// trap.c

#include "os.h"


extern void trap_vector(void);
extern void uart_isr(void);
extern void timer_handler(void);
extern void schedule(void);


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


__attribute__((naked)) // naked 函数不需要栈帧
reg_t
trap_handler(reg_t epc, reg_t cause)
{
    reg_t return_pc = epc;

    if(cause & MCAUSE_MASK_INTERRUPT)
    {
        /* Asynchronous trap - interrupt */
        switch(cause)
        {
        case MCAUSE_MACHINE_SOFTWARE_INTERRUPT:
            // 机器模式软件中断
            uart_puts("software interruption!\n");
            *(uint32_t*)CLINT_MSIP(r_mhartid()) = 0; // 通过清除 mip 中的 MSIP 位来确认软中断。
            schedule();

            break;

        case MCAUSE_MACHINE_TIMER_INTERRUPT:
            // 机器模式定时器中断
            uart_puts("timer interruption!\n");
            timer_handler();

            break;

        case MCAUSE_MACHINE_EXTERNAL_INTERRUPT:
            // 机器模式外部中断
            uart_puts("external interruption!\n");
            external_interrupt_handler();

            break;

        default:
            printf("Unknown async exception! Code = %ld\n", cause & MCAUSE_MASK_ECODE);

            break;
        }
    }
    else
    {
        /* Synchronous trap - exception */
        printf("Sync exceptions! Code = %ld\n", cause);
        printf("Exception occurred at address: %p\n", epc);
        panic("出现异常，摆了！");
        // return_pc += 4;
    }

    printf("Trap handler finished! Returning to: %p\n", return_pc);
    return return_pc;
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
