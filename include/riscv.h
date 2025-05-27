
// riscv.h
// ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/riscv.h

#ifndef __RISCV_H__
#define __RISCV_H__


#include "types.h"


/*/ Machine Status Register, mstatus /*/
// 机器模式状态寄存器，包含当前特权级、全局中断使能等重要状态标志。
#define MSTATUS_MPP (3 << 11) // 0b 00000000 00000000 00011000 00000000
#define MSTATUS_SPP (1 << 8)  // 0b 00000000 00000000 00000001 00000000
#define MSTATUS_MPIE (1 << 7) // 0b 00000000 00000000 00000000 10000000
#define MSTATUS_SPIE (1 << 5) // 0b 00000000 00000000 00000000 00100000
#define MSTATUS_UPIE (1 << 4) // 0b 00000000 00000000 00000000 00010000
#define MSTATUS_MIE (1 << 3)  // 0b 00000000 00000000 00000000 00001000
#define MSTATUS_SIE (1 << 1)  // 0b 00000000 00000000 00000000 00000010
#define MSTATUS_UIE (1 << 0)  // 0b 00000000 00000000 00000000 00000001

/* Machine-mode Interrupt Enable */
// 机器模式中断使能寄存器，控制各类中断的使能状态。
#define MIE_MEIE (1 << 11) // external 机器外部中断使能
#define MIE_MTIE (1 << 7)  // timer 机器定时器中断使能
#define MIE_MSIE (1 << 3)  // software 机器软件中断使能

/* Machine-mode Cause Masks */
// 机器模式 异常/中断 原因寄存器的掩码宏定义
#define MCAUSE_MASK_INTERRUPT (reg_t)0x80000000 // 高位掩码（第 31 位）表示是否为中断
#define MCAUSE_MASK_ECODE (reg_t)0x7FFFFFFF     // 低 31 位掩码，表示异常或中断原因码


/*/ 读取和写入寄存器的内联函数 /*/

static inline reg_t r_tp();             // 读取 线程指针寄存器
static inline reg_t r_mhartid();        // 读取 硬件线程 ID 寄存器
static inline reg_t r_mcause();         // 读取 机器模式异常/中断原因寄存器

static inline void w_mscratch(reg_t x); // 写入 机器模式临时寄存器
static inline void w_mtvec(reg_t x);    // 写入 机器模式中断向量表基地址

static inline reg_t r_mstatus();        // 读取 机器模式状态寄存器
static inline void  w_mstatus(reg_t x); // 写入 机器模式状态寄存器

static inline reg_t r_mie();            // 读取 机器模式中断使能寄存器
static inline void  w_mie(reg_t x);     // 写入 机器模式中断使能寄存器

static inline reg_t r_mepc();           // 读取 机器模式异常程序计数器
static inline void  w_mepc(reg_t x);    // 写入 机器模式异常程序计数器


/*/ 函数实现 /*/

// 读取线程指针寄存器
static inline reg_t
r_tp()
{
    reg_t x;
    asm volatile("mv %0, tp" : "=r"(x));
    return x;
}

// 读取 硬件线程 ID 寄存器
static inline reg_t
r_mhartid()
{
    reg_t x;
    asm volatile("csrr %0, mhartid" : "=r"(x));
    return x;
}

// 读取 机器模式异常/中断原因寄存器
static inline reg_t
r_mcause()
{
    reg_t x;
    asm volatile("csrr %0, mcause" : "=r"(x));
    return x;
}

// 写入 机器模式临时寄存器
static inline void
w_mscratch(reg_t x)
{
    asm volatile("csrw mscratch, %0" : : "r"(x));
}

// 写入 机器模式中断向量表基地址
static inline void
w_mtvec(reg_t x)
{
    asm volatile("csrw mtvec, %0" : : "r"(x));
}

// 读取 机器模式状态寄存器
static inline reg_t
r_mstatus()
{
    reg_t x;
    asm volatile("csrr %0, mstatus" : "=r"(x));
    return x;
}

// 写入 机器模式状态寄存器
static inline void
w_mstatus(reg_t x)
{
    asm volatile("csrw mstatus, %0" : : "r"(x));
}

// 读取 机器模式中断使能寄存器
static inline reg_t
r_mie()
{
    reg_t x;
    asm volatile("csrr %0, mie" : "=r"(x));
    return x;
}

// 写入 机器模式中断使能寄存器
static inline void
w_mie(reg_t x)
{
    asm volatile("csrw mie, %0" : : "r"(x));
}

// 读取 机器模式异常程序计数器
static inline reg_t
r_mepc()
{
    reg_t x;
    asm volatile("csrr %0, mepc" : "=r"(x));
    return x;
}

// 写入 机器模式异常程序计数器
static inline void
w_mepc(reg_t x)
{
    asm volatile("csrw mepc, %0" : : "r"(x));
}


#endif /* __RISCV_H__ */
