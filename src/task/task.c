
// task.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


uint8_t __attribute__((aligned(16))) task_stack[MAX_TASKS][STACK_SIZE]; // 在标准 RISC-V 调用约定中，堆栈指针 sp 始终是 16 字节对齐的
struct task_context                  task_contexts[MAX_TASKS];          // 任务上下文结构体列表
uint32_t                             _current    = -1;                  // 当前任务索引
uint32_t                             _task_count = 0;                   // 当前活动任务数


// 获取当前活动任务数
uint32_t
task_get_count()
{
    return _task_count;
}

// 获取当前任务编号
uint32_t
task_get_current()
{
    return _current;
}

// 获取当前任务的上下文指针
task_context_ptr
task_get_current_context()
{
    return &task_contexts[_current];
}

// 任务延迟
void
task_delay(volatile int count)
{
    count *= 50000;
    while(count-- > 0);
}

// 创建一个新任务，传入任务函数指针
int32_t
task_create(void (*start_routine)())
{
    // 保证任务数量不超过最大值
    if(_task_count >= MAX_TASKS) return -1;

    for(uint32_t i = 0; i < MAX_TASKS; i++) // 找到一个未使用的任务上下文
    {
        if(task_contexts[i].flags == 0)
        {
            task_contexts[i].ra    = (reg_t)(task_delete_current);        // 设置返回地址为任务删除函数
            task_contexts[i].pc    = (reg_t)(start_routine);              // 设置任务入口函数地址到 pc 寄存器
            task_contexts[i].sp    = (reg_t)(&task_stack[i][STACK_SIZE]); // 设置栈指针为任务栈的顶部
            task_contexts[i].flags = 1;                                   // 设置任务标志位为 1，表示任务已创建

            printf("Task[%d] Created: ra = %p, sp = %p, pc = %p\n", i, task_contexts[i].ra, task_contexts[i].sp, task_contexts[i].pc);

            _task_count++; // 增加活动任务数
            break;
        }
    }

    return 0;
}

// 删除指定任务
void
task_delete(task_context_ptr task_context_ptr)
{
    // 检查任务上下文指针是否有效，或者任务是否已被删除
    if(!task_context_ptr || task_context_ptr->flags == 0)
    {
        printf("Task not found!");
    }
    else
    {
        printf("Deleting Task: %p\n", task_context_ptr);

        task_context_ptr->flags = 0; // 将任务标志位设置为 0，表示任务已删除
        _task_count--;               // 减少活动任务数

        printf("Task[%d] Deleted\n", task_context_ptr - task_contexts);
    }

    // 如果当前任务是被删除的任务，触发一次任务切换，调度到其他任务
    if(r_mscratch() == (reg_t)task_context_ptr) task_yield();
}

// 删除当前任务
void
task_delete_current()
{
    task_delete(&task_contexts[_current]);
}
