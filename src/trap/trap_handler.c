
// trap/trap_handler.c

#include "os.h"

#include "platform.h"
#include "riscv.h"
#include "typedefs.h"

extern void schedule();                   // 任务轮转调度函数
extern void task_interrupt_handler();     // 任务切换处理函数，处理机器模式软件中断
extern void external_interrupt_handler(); // 外部中断处理函数，处理来自 PLIC 的中断请求
extern void timer_interrupt_handler();    // 定时器中断处理函数


// __attribute__((naked)) // naked 函数不需要栈帧
// 异常/中断处理函数，处理同步和异步异常/中断
reg_t
trap_handler(reg_t epc, reg_t cause)
{
    printk("Trap handler called! EPC = %p, Cause = %p\n", epc, cause);
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
            schedule();

            break;

        case MCAUSE_MACHINE_TIMER_INTERRUPT:
            // 机器模式定时器中断
            uart_puts("timer interruption!\n");
            timer_interrupt_handler();
            schedule();

            break;

        case MCAUSE_MACHINE_EXTERNAL_INTERRUPT:
            // 机器模式外部中断
            uart_puts("external interruption!\n");
            external_interrupt_handler();

            break;

        default:
            printk("Unknown async exception! Code = %ld\n", cause & MCAUSE_MASK_ECODE);

            break;
        }
    }
    else
    {
        /* Synchronous trap - exception */
        printk("Sync exceptions! Code = %ld\n", cause);
        printk("Exception occurred at address: %p\n", epc);
        panic("出现异常，摆了！");
        // return_pc += 4;
    }

    printk("Trap handler finished! Returning to: %p\n", return_pc);
    return return_pc;
}
