
// sched.c

#include "task.h"

#include "platform.h"
#include "riscv.h"


extern uint8_t             task_stack[MAX_TASKS][STACK_SIZE];
extern struct task_context task_contexts[MAX_TASKS];
extern uint32_t            _current;
extern uint32_t            _task_count;


// 任务轮转调度
extern void switch_to(task_context_ptr next_task); // 切换到下一个任务的上下文
__attribute__((naked))                             // naked 函数不需要栈帧
void
schedule()
{
    if(_task_count <= 0)
    {
        panic("No active task to run!");
        return;
    }

    do _current = (_current + 1) % MAX_TASKS; // 实现循环调度，如果超出最后一个就会回到第一个
    while(task_contexts[_current].flags == 0); // 如果当前任务的标志位为 0，表示任务未创建，则继续寻找下一个任务

    switch_to(&(task_contexts[_current])); // 切换任务
}

// 任务主动让出 CPU，允许其他任务运行
void
task_yield()
{
    /* trigger a machine-level software interrupt */
    *(uint32_t*)CLINT_MSIP(r_mhartid()) = 1; // 通过设置 msip 寄存器的 MSIP 位来触发机器模式软件中断
}

// 初始化调度器、开始任务调度
void
sched_init()
{
    w_mscratch(0);             // 将 0 写入 mscratch 寄存器，说明任务上下文还没有被初始化
    w_mie(r_mie() | MIE_MSIE); // 启用机器模式软件中断

    schedule();                // 调用调度函数，开始任务调度
}
