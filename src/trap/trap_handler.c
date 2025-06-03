
// trap/trap_handler.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


extern void timer_interrupt_handler(); // 定时器中断处理函数


// __attribute__((naked)) // naked 函数不需要栈帧
// 异常/中断处理函数，处理同步和异步异常/中断
reg_t
trap_handler(reg_t epc, reg_t cause)
{
    printf("Trap handler called! EPC = %p, Cause = %p\n", epc, cause);
    reg_t return_pc = epc;

    if(cause & MCAUSE_MASK_INTERRUPT)
    {
        /* Asynchronous trap - interrupt */
        switch(cause)
        {
        case MCAUSE_MACHINE_SOFTWARE_INTERRUPT:
            // 机器模式软件中断
            uart_puts("software interruption!\n");
            task_interrupt_handler();

            break;

        case MCAUSE_MACHINE_TIMER_INTERRUPT:
            // 机器模式定时器中断
            uart_puts("timer interruption!\n");
            timer_interrupt_handler();

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
