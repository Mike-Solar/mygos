
// trap.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


extern void trap_vector(void); // 异常/中断向量表入口函数，处理各种异常和中断
void
trap_init()
{
    // 设置机器模式的异常/中断向量表基地址
    w_mtvec((reg_t)trap_vector);
}
