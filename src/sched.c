
// sched.c

#include "task.h"

#include "platform.h"
#include "riscv.h"


/* defined in entry.S */
extern void switch_to(struct task_context* next_task);


static uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE]; // 在标准 RISC-V 调用约定中，堆栈指针 sp 始终是 16 字节对齐的
struct task_context                         task_contexts[MAX_TASKS];          // 任务上下文结构体列表
static uint32_t                             _current    = -1;                  // 当前任务索引
static uint32_t                             _task_count = 0;                   // 当前活动任务数


// 初始化调度器
void
sched_init()
{
    w_mscratch(0);             // 将 0 写入 mscratch 寄存器，说明任务上下文还没有被初始化
    w_mie(r_mie() | MIE_MSIE); // 启用机器模式软件中断
}

// 任务轮转调度
void
schedule()
{
    if(_task_count <= 0)
    {
        panic("No active task to run!");
        return;
    }

    do
    {
        _current = (_current + 1) % MAX_TASKS; // 实现循环调度，如果超出最后一个就会回到第一个
    } while(task_contexts[_current].flags == 0); // 如果当前任务的标志位为 0，表示任务未创建，则继续寻找下一个任务

    switch_to(&(task_contexts[_current])); // 切换任务
}

// 任务主动让出 CPU，允许其他任务运行
void
task_yield()
{
    /* trigger a machine-level software interrupt */
    *(uint32_t*)CLINT_MSIP(r_mhartid()) = 1; // 通过设置 msip 寄存器的 MSIP 位来触发机器模式软件中断
}

// 创建一个新任务，传入任务函数指针
int32_t
task_create(void (*start_routine)(uint32_t))
{
    // 保证任务数量不超过最大值
    if(_task_count >= MAX_TASKS) return -1;

    for(uint32_t i = 0; i < MAX_TASKS; i++)
    {
        if(task_contexts[i].flags == 0) // 找到一个未使用的任务上下文
        {

            task_contexts[i].pc    = (reg_t)(start_routine);              // 设置返回地址为任务入口函数
            task_contexts[i].sp    = (reg_t)(&task_stack[i][STACK_SIZE]); // 设置栈指针为任务栈的顶部
            task_contexts[i].a0    = i;                                   // 将任务 ID 传递给任务入口函数，作为第一个参数
            task_contexts[i].flags = 1;                                   // 设置任务标志位为 1，表示任务已创建

            printf("Task[%d] Created: sp = 0x%08x, pc = 0x%08x\n", i, task_contexts[i].sp, task_contexts[i].pc);

            _task_count++; // 增加活动任务数
            break;
        }
    }

    return 0;
}

// 删除指定任务
void
task_delete(uint32_t task_id)
{
    if(task_id < 0 || task_id >= MAX_TASKS) // 检查任务 ID 是否有效
    {
        panic("Invalid task ID!");
        return;
    }
    else if(task_contexts[task_id].flags == 0) // 检查任务是否存在
    {
        panic("Task not found!");
        return;
    }

    task_contexts[task_id].flags = 0; // 将任务标志位设置为 0，表示任务已删除
    _task_count--;                    // 减少活动任务数

    printf("Task[%d] Deleted\n", task_id);

    task_yield(); // 触发一次任务切换，确保删除的任务不会被调度到
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
