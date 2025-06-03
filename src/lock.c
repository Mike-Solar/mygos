
// lock.c

#include "os.h"
#include "platform.h"
#include "riscv.h"


int
spin_lock()
{
    // 直接禁用中断
    // 通过修改 mstatus 寄存器的 MIE 位来实现
    w_mstatus(r_mstatus() & ~MSTATUS_MIE);
    return 0;
}

int
spin_unlock()
{
    // 恢复中断
    w_mstatus(r_mstatus() | MSTATUS_MIE);
    return 0;
}
