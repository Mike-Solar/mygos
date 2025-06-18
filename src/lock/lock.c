
// lock.c

#include "os.h"

#include "platform.h"
#include "riscv.h"


// 直接禁用中断
// 通过修改 mstatus 寄存器的 MIE 位来实现
void
spin_lock()
{
    w_mstatus(r_mstatus() & ~MSTATUS_MIE);
}

// 恢复中断
void
spin_unlock()
{
    w_mstatus(r_mstatus() | MSTATUS_MIE);
}
