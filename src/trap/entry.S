
// entry.S

#include "platform.h"


/*/
 * 保存所有通用寄存器（General-Purpose, GP）到上下文结构中
 * 
 * 注意：这里保存的 GP 寄存器不包括 gp 和 tp，
 * 因为它们既不是 caller-saved（调用者负责保存），也不是 callee-saved（被调用者负责保存）
 * 它们常用于特殊用途。例如在 RVOS 中，tp（线程指针）用于存储 hart_id（硬件线程编号），
 * 这个值是全局固定的，在上下文切换过程中不会改变。
 *
 * 不在这里保存 t6，因为 t6 已被用作 base 参数，
 * 所以必须在 SAVE_REG 之外单独保存 t6
/*/
.macro SAVE_REG base
    sw ra,   TASK_CONTEXT_RA_OFFSET(\base)
    sw sp,   TASK_CONTEXT_SP_OFFSET(\base)
    sw t0,   TASK_CONTEXT_T0_OFFSET(\base)
    sw t1,   TASK_CONTEXT_T1_OFFSET(\base)
    sw t2,   TASK_CONTEXT_T2_OFFSET(\base)
    sw s0,   TASK_CONTEXT_S0_OFFSET(\base)
    sw s1,   TASK_CONTEXT_S1_OFFSET(\base)
    sw a0,   TASK_CONTEXT_A0_OFFSET(\base)
    sw a1,   TASK_CONTEXT_A1_OFFSET(\base)
    sw a2,   TASK_CONTEXT_A2_OFFSET(\base)
    sw a3,   TASK_CONTEXT_A3_OFFSET(\base)
    sw a4,   TASK_CONTEXT_A4_OFFSET(\base)
    sw a5,   TASK_CONTEXT_A5_OFFSET(\base)
    sw a6,   TASK_CONTEXT_A6_OFFSET(\base)
    sw a7,   TASK_CONTEXT_A7_OFFSET(\base)
    sw s2,   TASK_CONTEXT_S2_OFFSET(\base)
    sw s3,   TASK_CONTEXT_S3_OFFSET(\base)
    sw s4,   TASK_CONTEXT_S4_OFFSET(\base)
    sw s5,   TASK_CONTEXT_S5_OFFSET(\base)
    sw s6,   TASK_CONTEXT_S6_OFFSET(\base)
    sw s7,   TASK_CONTEXT_S7_OFFSET(\base)
    sw s8,   TASK_CONTEXT_S8_OFFSET(\base)
    sw s9,   TASK_CONTEXT_S9_OFFSET(\base)
    sw s10,  TASK_CONTEXT_S10_OFFSET(\base)
    sw s11,  TASK_CONTEXT_S11_OFFSET(\base)
    sw t3,   TASK_CONTEXT_T3_OFFSET(\base)
    sw t4,   TASK_CONTEXT_T4_OFFSET(\base)
    sw t5,   TASK_CONTEXT_T5_OFFSET(\base)
.endm

/*/
 * 从上下文中恢复所有通用寄存器（除了 gp 和 tp）
/*/
.macro LOAD_REG base
    lw ra,   TASK_CONTEXT_RA_OFFSET(\base)
    lw sp,   TASK_CONTEXT_SP_OFFSET(\base)
    lw t0,   TASK_CONTEXT_T0_OFFSET(\base)
    lw t1,   TASK_CONTEXT_T1_OFFSET(\base)
    lw t2,   TASK_CONTEXT_T2_OFFSET(\base)
    lw s0,   TASK_CONTEXT_S0_OFFSET(\base)
    lw s1,   TASK_CONTEXT_S1_OFFSET(\base)
    lw a0,   TASK_CONTEXT_A0_OFFSET(\base)
    lw a1,   TASK_CONTEXT_A1_OFFSET(\base)
    lw a2,   TASK_CONTEXT_A2_OFFSET(\base)
    lw a3,   TASK_CONTEXT_A3_OFFSET(\base)
    lw a4,   TASK_CONTEXT_A4_OFFSET(\base)
    lw a5,   TASK_CONTEXT_A5_OFFSET(\base)
    lw a6,   TASK_CONTEXT_A6_OFFSET(\base)
    lw a7,   TASK_CONTEXT_A7_OFFSET(\base)
    lw s2,   TASK_CONTEXT_S2_OFFSET(\base)
    lw s3,   TASK_CONTEXT_S3_OFFSET(\base)
    lw s4,   TASK_CONTEXT_S4_OFFSET(\base)
    lw s5,   TASK_CONTEXT_S5_OFFSET(\base)
    lw s6,   TASK_CONTEXT_S6_OFFSET(\base)
    lw s7,   TASK_CONTEXT_S7_OFFSET(\base)
    lw s8,   TASK_CONTEXT_S8_OFFSET(\base)
    lw s9,   TASK_CONTEXT_S9_OFFSET(\base)
    lw s10,  TASK_CONTEXT_S10_OFFSET(\base)
    lw s11,  TASK_CONTEXT_S11_OFFSET(\base)
    lw t3,   TASK_CONTEXT_T3_OFFSET(\base)
    lw t4,   TASK_CONTEXT_T4_OFFSET(\base)
    lw t5,   TASK_CONTEXT_T5_OFFSET(\base)
    lw t6,   TASK_CONTEXT_T6_OFFSET(\base)
.endm

.macro STORE_T6 from
    sw t6,  TASK_CONTEXT_T6_OFFSET(\from)  // 将 t6 保存到上下文结构体中的 t6 位置
.endm

.macro LOAD_MEPC to from
    lw \to,  TASK_CONTEXT_PC_OFFSET(\from) // 从上下文结构体中加载 mepc 到指定寄存器
.endm


.text


/*/
 * 保存/恢复过程需要注意的事项：
 * - 使用 CSR 寄存器 mscratch 存放 **当前任务上下文的指针**
 * - 使用 t6 作为 SAVE_REG/LOAD_REG 的 base，因为 t6 是编号最高的寄存器（x31），在加载期间不会被覆盖
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
    LOAD_REG    t6                  // 恢复下一个任务的所有通用寄存器

    mret                            //mret 表示返回到 mepc 指示的地址

// end of switch_to


/*/
 * 中断和异常处理函数位于机器模式下
 * trap 向量基地址必须始终对齐到 4 字节边界
/*/
.globl trap_vector
.balign 4
trap_vector:

    // 保存当前任务的寄存器上下文到 mscratch
    csrrw       t6, mscratch, t6                // 交换 t6 和 mscratch 的值
    SAVE_REG    t6                              // 保存当前任务的寄存器上下文到 t6 指向的地址
    mv          t5, t6                          // t5 指向当前任务上下文
    csrr        t6, mscratch                    // 从 mscratch 中读取 t6
    sw          t6,  TASK_CONTEXT_T6_OFFSET(t5) // 将 t6 保存到上下文中的 t6 位置
	csrr	    a0, mepc                        // 获取异常发生时的程序计数器（mepc）：保存异常/中断发生时要返回的指令地址
	sw	        a0,  TASK_CONTEXT_PC_OFFSET(t5) // 存到上下文结构体中
    csrw        mscratch, t5                    // 将 mscratch 恢复为当前任务的上下文指针


    // 在 trap.c 中调用 C 语言的异常处理函数
    csrr	    a0, mepc            // 获取陷入发生时的程序计数器（mepc）
    csrr        a1, mcause          // 获取陷入原因
    call        trap_handler        // 调用陷入处理函数
    
    csrw        mepc, a0            // trap_handler 会通过 a0 返回返回地址，地址保存在异常发生时的程序计数器 mepc
    csrr        t6, mscratch        // 重新加载 mscratch 中的上下文指针
    LOAD_REG    t6                  // 恢复上下文
    
    mret                            // 返回到发生异常之前的代码执行位置

// end of trap_vector


.end
