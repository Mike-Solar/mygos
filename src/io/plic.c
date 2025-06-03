
// plic.c

#include "io.h"

#include "platform.h"
#include "riscv.h"


void
plic_init()
{
    uint32_t hart = r_tp();

    /*/
     * 设置 UART0 的优先级。
     *
     * 每个 PLIC 中断源都可以通过写入其 32 位内存映射优先级寄存器来分配优先级。
     * QEMU-virt（与 FU540-C000 相同）支持 7 个优先级等级。
     * 优先级值为 0 表示“永不中断”，等效于禁用该中断。
     * 优先级 1 是最低的活动优先级，优先级 7 是最高的。
     * 如果有多个全局中断优先级相同，则通过中断 ID 决定优先级；ID 最小的中断优先级最高。
     *
    /*/
    *(uint32_t*)PLIC_PRIORITY(UART0_IRQ) = 1;

    /*/
     * 使能 UART0 中断。
     *
     * 每个全局中断都可以通过在使能寄存器中设置相应位来使能。
    /*/
    *(uint32_t*)PLIC_MENABLE(hart, UART0_IRQ) = (1 << (UART0_IRQ % 32));

    /*/
     * 通过设置优先级阈值，可以控制哪些中断可以被处理。
     *
     * PLIC 会屏蔽所有优先级小于等于阈值的中断。
     * 最大阈值为 7。
     * 例如，阈值为 0 时允许所有非零优先级的中断；阈值为 7 时屏蔽所有中断。
     * 注意，阈值是 PLIC 全局的，而不是针对每个中断源单独设置的。
    /*/
    *(uint32_t*)PLIC_MTHRESHOLD(hart) = 0;

    // 通过设置机器模式中断使能寄存器的外部中断位来启用机器模式外部中断。
    w_mie(r_mie() | MIE_MEIE);

    // 通过设置机器模式状态寄存器的全局中断使能位来启用机器模式全局中断。
    w_mstatus(r_mstatus() | MSTATUS_MIE);
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
    uint32_t hart = r_tp();

    *(uint32_t*)PLIC_MCOMPLETE(hart) = plic_id;
}
