
// sched.c

#include "os.h"

/* defined in entry.S */
extern void switch_to(struct context* next);

#define MAX_TASKS 10
#define STACK_SIZE 1024

/*/
 * In the standard RISC-V calling convention, the stack pointer sp is always 16-byte aligned.
 * 在标准 RISC-V 调用约定中，堆栈指针 sp 始终是 16 字节对齐的。
 * __attribute__((aligned(16)))：GCC 扩展语法，表示让这个数组在内存中按16字节对齐。
/*/
uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE];

// 任务上下文结构体列表
struct context ctx_tasks[MAX_TASKS];

/*/
 * _top is used to mark the max available position of ctx_tasks
 * _current is used to point to the context of current task
 *
 * -top 用来标记任务上下文的最大可用位置
 * -current 用来指向当前任务的上下文
/*/
static int _top     = 0;
static int _current = -1;


/*/
 * 用于向 RISC-V 架构的 mscratch 寄存器写入一个值。
 * mscratch 是 RISC-V 架构下的一个机器模式控制与状态寄存器，常用于操作系统内核保存和恢复任务切换时的临时数据。
/*/
static void
w_mscratch(reg_t x)
{
    asm volatile("csrw mscratch, %0" : : "r"(x));
}

// 初始化调度器
void
sched_init()
{
    w_mscratch(0); // 将 0 写入 mscratch 寄存器，说明任务上下文还没有被初始化
}

/*/
 * implement a simple cycle FIFO schedular
 * 轮转调度器的实现
/*/
void
schedule()
{
    if(_top <= 0)
    {
        panic("Num of task should be greater than zero!");
        return;
    }

    _current = (_current + 1) % _top;  // 实现循环调度，如果超出最后一个就会回到第一个
    switch_to(&(ctx_tasks[_current])); // 切换任务
}

/*/
 * DESCRIPTION
 * 	task_yield()  causes the calling task to relinquish the CPU and a new task gets to run.
 *  这个函数会导致调用的任务放弃 CPU，新的任务将开始运行。
/*/
void
task_yield()
{
    schedule();
}

/*/
 * DESCRIPTION
 * 	Create a task.
 * 	- start_routine: task routine entry （任务例程入口）
 * RETURN VALUE
 * 	0: success
 * 	-1: if error occurred
/*/
int
task_create(void (*start_routine)(void))
{
    if(_top < MAX_TASKS)
    {
        ctx_tasks[_top].sp = (reg_t)&task_stack[_top][STACK_SIZE];
        ctx_tasks[_top].ra = (reg_t)start_routine;

        _top++;
        return 0;
    }
    else return -1;
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
