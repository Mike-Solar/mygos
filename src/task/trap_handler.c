
// task/trap_handler.c

#include "platform.h"
#include "riscv.h"


// 任务切换处理函数，处理机器模式软件中断
void
task_interrupt_handler()
{
    *(uint32_t*)CLINT_MSIP(r_mhartid()) = 0; // 通过清除 mip 中的 MSIP 位来确认软中断。
}
