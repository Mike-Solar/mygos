
// entry.S

#include "platform.h"


/*/
 * Save all General-Purpose(GP) registers to context.
 * struct context *base = &ctx_task;
 * base->ra = ra;
 * ......
 * These GP registers to be saved don't include gp
 * and tp, because they are not caller-saved or
 * callee-saved. These two registers are often used
 * for special purpose. For example, in RVOS, 'tp'
 * (aka "thread pointer") is used to store hartid,
 * which is a global value and would not be changed
 * during context-switch.
 *
 * 保存所有通用寄存器（General-Purpose, GP）到上下文结构中
 * 相当于 C 伪代码：
 * struct context *base = &ctx_task;
 * base->ra = ra;
 * ......
 * 注意：这里保存的 GP 寄存器不包括 gp 和 tp，
 * 因为它们既不是 caller-saved（调用者负责保存），也不是 callee-saved（被调用者负责保存）
 * 它们常用于特殊用途。例如在 RVOS 中，tp（线程指针）用于存储 hartid（硬件线程编号），
 * 这个值是全局固定的，在上下文切换过程中不会改变。
 *
 * we don't save t6 here, due to we have used
 * it as base, we have to save t6 in an extra step
 * outside of reg_save
 * 不在这里保存 t6，因为 t6 已被用作 base 参数，
 * 所以必须在 reg_save 之外单独保存 t6
/*/
.macro reg_save base
    sw ra,   0*SIZE_REG(\base)
    sw sp,   1*SIZE_REG(\base)
    sw t0,   4*SIZE_REG(\base)
    sw t1,   5*SIZE_REG(\base)
    sw t2,   6*SIZE_REG(\base)
    sw s0,   7*SIZE_REG(\base)
    sw s1,   8*SIZE_REG(\base)
    sw a0,   9*SIZE_REG(\base)
    sw a1,  10*SIZE_REG(\base)
    sw a2,  11*SIZE_REG(\base)
    sw a3,  12*SIZE_REG(\base)
    sw a4,  13*SIZE_REG(\base)
    sw a5,  14*SIZE_REG(\base)
    sw a6,  15*SIZE_REG(\base)
    sw a7,  16*SIZE_REG(\base)
    sw s2,  17*SIZE_REG(\base)
    sw s3,  18*SIZE_REG(\base)
    sw s4,  19*SIZE_REG(\base)
    sw s5,  20*SIZE_REG(\base)
    sw s6,  21*SIZE_REG(\base)
    sw s7,  22*SIZE_REG(\base)
    sw s8,  23*SIZE_REG(\base)
    sw s9,  24*SIZE_REG(\base)
    sw s10, 25*SIZE_REG(\base)
    sw s11, 26*SIZE_REG(\base)
    sw t3,  27*SIZE_REG(\base)
    sw t4,  28*SIZE_REG(\base)
    sw t5,  29*SIZE_REG(\base)
.endm

/*/
 * 从上下文中恢复所有通用寄存器（除了 gp 和 tp）
 * 类似于 C 伪代码：
 * struct context *base = &ctx_task;
 * ra = base->ra;
 * ......
/*/
.macro reg_restore base
    lw ra,   0*SIZE_REG(\base)
    lw sp,   1*SIZE_REG(\base)
    lw t0,   4*SIZE_REG(\base)
    lw t1,   5*SIZE_REG(\base)
    lw t2,   6*SIZE_REG(\base)
    lw s0,   7*SIZE_REG(\base)
    lw s1,   8*SIZE_REG(\base)
    lw a0,   9*SIZE_REG(\base)
    lw a1,  10*SIZE_REG(\base)
    lw a2,  11*SIZE_REG(\base)
    lw a3,  12*SIZE_REG(\base)
    lw a4,  13*SIZE_REG(\base)
    lw a5,  14*SIZE_REG(\base)
    lw a6,  15*SIZE_REG(\base)
    lw a7,  16*SIZE_REG(\base)
    lw s2,  17*SIZE_REG(\base)
    lw s3,  18*SIZE_REG(\base)
    lw s4,  19*SIZE_REG(\base)
    lw s5,  20*SIZE_REG(\base)
    lw s6,  21*SIZE_REG(\base)
    lw s7,  22*SIZE_REG(\base)
    lw s8,  23*SIZE_REG(\base)
    lw s9,  24*SIZE_REG(\base)
    lw s10, 25*SIZE_REG(\base)
    lw s11, 26*SIZE_REG(\base)
    lw t3,  27*SIZE_REG(\base)
    lw t4,  28*SIZE_REG(\base)
    lw t5,  29*SIZE_REG(\base)
    lw t6,  30*SIZE_REG(\base)
.endm

.macro STORE_T6 from
    sw t6, 30*SIZE_REG(\from)     // 将 t6 保存到上下文结构体中的 t6 位置
.endm

.macro LOAD_MEPC to from
    lw \to, 31*SIZE_REG(\from)     // 从上下文结构体中加载 mepc 到指定寄存器
.endm


.text


/*/
 * Something to note about save/restore:
 * - We use mscratch to hold a pointer to context of current task
 * - We use t6 as the 'base' for reg_save/reg_restore, because it is the
 *   very bottom register (x31) and would not be overwritten during loading.
 *   Note: CSRs(mscratch) can not be used as 'base' due to load/restore
 *   instruction only accept general purpose registers.
 *
 * 保存/恢复过程需要注意的事项：
 * - 使用 CSR 寄存器 mscratch 存放 **当前任务上下文的指针**
 * - 使用 t6 作为 reg_save/reg_restore 的 base，因为 t6 是编号最高的寄存器（x31），在加载期间不会被覆盖
 * - 注意：mscratch 不能被当作 base 来直接进行 load/store 操作，因为这些指令只能使用通用寄存器
/*/

// void switch_to(struct context *next);
// 这里默认上一个任务的上下文已经保存完毕
// 参数：a0 = 下一个任务的上下文结构体指针
.globl switch_to
.balign 4
switch_to:
	csrw	    mscratch, a0        // 把 mscratch 指向目标上下文结构体

    LOAD_MEPC   a1, a0              // 从下一个任务的上下文结构体中加载 mepc 到 a1
    csrw	    mepc, a1            // 写入 mepc 准备跳转到这个地址

    mv          t6, a0              // 使用 t6 指向下一个任务的上下文
    reg_restore t6                  // 恢复下一个任务的所有通用寄存器

    mret                            //mret 表示返回到 mepc 指示的地址

// end of switch_to


/*/
 * interrupts and exceptions while in machine mode come here.
 * the trap vector base address must always be aligned on a 4-byte boundary
 *
 * 中断和异常处理函数，位于机器模式下
 * trap 向量基地址必须始终对齐到 4 字节边界
/*/
.globl trap_vector
.balign 4
trap_vector:

    // 保存当前任务的寄存器上下文到 mscratch
    csrrw       t6, mscratch, t6    // 交换 t6 和 mscratch 的值
    reg_save    t6                  // 保存当前任务的寄存器上下文到 t6 指向的地址
    mv          t5, t6              // t5 指向当前任务上下文
    csrr        t6, mscratch        // 从 mscratch 中读取 t6
    sw          t6, 30*SIZE_REG(t5) // 将 t6 保存到上下文中的 t6 位置
	csrr	    a0, mepc            // 获取异常发生时的程序计数器（mepc）：保存异常/中断发生时要返回的指令地址
	sw	        a0, 31*SIZE_REG(t5) // 存到上下文结构体中
    csrw        mscratch, t5        // 将 mscratch 恢复为当前任务的上下文指针


    // 在 trap.c 中调用 C 语言的异常处理函数
    csrr	    a0, mepc
    csrr        a1, mcause          // 获取异常原因
    call        trap_handler        // 调用异常处理函数
    csrw        mepc, a0            // trap_handler 会通过 a0 返回返回地址，地址保存在异常发生时的程序计数器 mepc


    // 恢复寄存器上下文
    csrr        t6, mscratch        // 重新加载 mscratch 中的上下文指针
    reg_restore t6                  // 恢复上下文


    // 返回到发生异常之前的代码执行位置
    mret

// end of trap_vector


.end
