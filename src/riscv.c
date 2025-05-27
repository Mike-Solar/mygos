
// riscv.c

#include "riscv.h"


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
