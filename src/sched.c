
// sched.c

#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context* next);


#define STACK_SIZE 1024

/*/
 * In the standard RISC-V calling convention, the stack pointer sp is always 16-byte aligned.
 * 在标准 RISC-V 调用约定中，堆栈指针 sp 始终是 16 字节对齐的。
 * __attribute__((aligned(16)))：GCC 扩展语法，表示让这个数组在内存中按16字节对齐。
/*/

uint8_t __attribute__((aligned(16))) task_stack[STACK_SIZE];

struct context ctx_task; // 任务上下文

/*/
 * 用于向 RISC-V 架构的 mscratch 寄存器写入一个值。
 * mscratch 是 RISC-V 架构下的一个机器模式控制与状态寄存器，常用于操作系统内核保存和恢复任务切换时的临时数据。
/*/
static void
w_mscratch(reg_t x)
{
    asm volatile("csrw mscratch, %0" : : "r"(x));
}

/*/
 * a very rough implementation, just to consume the cpu
 * 一个非常粗糙的实现，只是为了消耗 CPU
/*/
void
task_delay(volatile int count)
{
    count *= 50000;
    while(count-- > 0);
}

// 用户任务0
void
user_task0(void)
{
    uart_puts("Task 0: Created!\n");
    while(1)
    {
        uart_puts("Task 0: Running...\n");
        task_delay(1000);
    }
}

// 初始化调度器
void
sched_init()
{
    w_mscratch(0);                                // 将 0 写入 mscratch 寄存器，说明任务上下文还没有被初始化

    ctx_task.sp = (reg_t)&task_stack[STACK_SIZE]; // 设置栈指针为任务栈的顶部
    ctx_task.ra = (reg_t)user_task0;              // 设置返回地址为用户任务0的入口地址
}

// 切换到下一个任务
void
schedule()
{
    struct context* next = &ctx_task;
    switch_to(next);
}
