
// plic.c

#include "io.h"

#include "platform.h"
#include "riscv.h"


void
plic_init()
{
    uint32_t hart = r_tp();

    *(uint32_t*)PLIC_PRIORITY(UART0_IRQ)      = 1;                       // 设置 UART0 的优先级
    *(uint32_t*)PLIC_MENABLE(hart, UART0_IRQ) = (1 << (UART0_IRQ % 32)); // 使能 UART0 中断
    *(uint32_t*)PLIC_MTHRESHOLD(hart)         = 0;                       // 设置优先级阈值

    w_mie(r_mie() | MIE_MEIE);                                           // 启用机器模式外部中断
    w_mstatus(r_mstatus() | MSTATUS_MIE);                                // 启用机器模式全局中断
}

/*/
 * 描述：
 *	查询 PLIC，获取当前应当响应的中断。
 *	通过读取 claim 寄存器执行中断声明操作，
 *	返回最高优先级的待处理中断 ID，如果没有待处理中断则返回 0。
 *	成功声明后会自动清除对应中断源的 pending 位。
 * 返回值：
 *	最高优先级的待处理中断 ID，如果没有则返回 0。
/*/
uint32_t
plic_claim()
{
    uint32_t hart = r_tp();
    return *(uint32_t*)PLIC_MCLAIM(hart);
}

/*/
 * 描述：
 *	向 complete 寄存器写入 claim 得到的中断 ID（irq），通知 PLIC 我们已处理完该中断。
 *	PLIC 不会检查完成 ID 是否与上次声明 ID 相同。
 *	如果完成 ID 不属于当前目标已使能的中断源，则该完成操作会被静默忽略。
 * 返回值：无
/*/
void
plic_complete(uint32_t plic_id)
{
    uint32_t hart                    = r_tp();
    *(uint32_t*)PLIC_MCOMPLETE(hart) = plic_id;
}
