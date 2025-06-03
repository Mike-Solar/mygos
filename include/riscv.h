
// riscv.h
// ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/riscv.h

#ifndef __RISCV_H__
#define __RISCV_H__

#include "types.h"


/*/ Machine Status Register, mstatus /*/
// 机器模式状态寄存器，包含当前特权级、全局中断使能等重要状态标志。
#define MSTATUS_MPP (3 << 11) // | MPP  | Machine Previous Privilege           | 上次特权级（M 模式中） | 表示陷入中断前的特权级。                               |
#define MSTATUS_SPP (1 << 8)  // | SPP  | Supervisor Previous Privilege        | 上次特权级（S 模式中） | 表示陷入中断前的特权级。                               |
#define MSTATUS_FS (3 << 13)  // | FS   | Floating-point Status                | 浮点状态               | 表示当前浮点单元的状态（禁用、初始、已启用）。         |
#define MSTATUS_MPIE (1 << 7) // | MPIE | Machine Previous Interrupt Enable    | 上次中断使能（M 模式） | 表示陷入中断前的全局中断使能状态。                     |
#define MSTATUS_SPIE (1 << 5) // | SPIE | Supervisor Previous Interrupt Enable | 上次中断使能（S 模式） | 表示陷入中断前的全局中断使能状态。                     |
#define MSTATUS_UPIE (1 << 4) // | UPIE | User Previous Interrupt Enable       | 上次中断使能（U 模式） | 表示陷入中断前的全局中断使能状态。                     |
#define MSTATUS_MIE (1 << 3)  // | MIE  | Machine Interrupt Enable             | 机器模式中断使能       | 表示当前的全局中断使能状态。                           |
#define MSTATUS_SIE (1 << 1)  // | SIE  | Supervisor Interrupt Enable          | 监督模式中断使能       | 表示当前的全局中断使能状态。                           |
#define MSTATUS_UIE (1 << 0)  // | UIE  | User Interrupt Enable                | 用户模式中断使能       | 表示当前的全局中断使能状态。                           |

/* Machine-mode Interrupt Enable */
// 机器模式中断使能寄存器，控制各类中断的使能状态。
#define MIE_MEIE (1 << 11) // external 机器外部中断使能
#define MIE_MTIE (1 << 7)  // timer 机器定时器中断使能
#define MIE_MSIE (1 << 3)  // software 机器软件中断使能

/* Machine-mode Cause Masks */
// 机器模式 异常/中断 原因寄存器的掩码宏定义
#define MCAUSE_MASK_INTERRUPT (reg_t)0x80000000 // 高位掩码（第 31 位）表示是否为中断
#define MCAUSE_MASK_ECODE (reg_t)0x7FFFFFFF     // 低 31 位掩码，表示异常或中断原因码

// 中断原因码
#define MCAUSE_INSTRUCTION_ADDRESS_MISALIGNED 0             // 指令地址未对齐
#define MCAUSE_INSTRUCTION_ACCESS_FAULT 1                   // 指令访问故障
#define MCAUSE_ILLEGAL_INSTRUCTION 2                        // 非法指令
#define MCAUSE_BREAKPOINT 3                                 // 断点
#define MCAUSE_LOAD_ADDRESS_MISALIGNED 4                    // 读地址未对齐
#define MCAUSE_LOAD_ACCESS_FAULT 5                          // 加载访问故障
#define MCAUSE_STORE_ADDRESS_MISALIGNED 6                   // 写地址未对齐
#define MCAUSE_STORE_ACCESS_FAULT 7                         // 写访问故障
#define MCAUSE_ENVIRONMENT_CALL_FROM_UMODE 8                // U 模式发出的系统调用（ecall）
#define MCAUSE_ENVIRONMENT_CALL_FROM_SMODE 9                // S 模式发出的系统调用（ecall）
#define MCAUSE_ENVIRONMENT_CALL_FROM_MMODE 11               // M 模式发出的系统调用（ecall）
#define MCAUSE_INSTRUCTION_PAGE_FAULT 12                    // 指令页错误（Sv32）
#define MCAUSE_LOAD_PAGE_FAULT 13                           // 加载页错误
#define MCAUSE_STORE_PAGE_FAULT 15                          // 存储页错误
#define MCAUSE_MACHINE_SOFTWARE_INTERRUPT (0x80000000 + 3)  // 软件中断（由 MSIP 触发）
#define MCAUSE_MACHINE_TIMER_INTERRUPT (0x80000000 + 7)     // 定时器中断（mtimecmp）
#define MCAUSE_MACHINE_EXTERNAL_INTERRUPT (0x80000000 + 11) // 外部中断（PLIC）


/*/ 读取和写入寄存器的内联函数 /*/

static inline reg_t r_tp();              // 读取 线程指针寄存器
static inline reg_t r_mhartid();         // 读取 硬件线程 ID 寄存器
static inline reg_t r_mcause();          // 读取 机器模式异常/中断原因寄存器

static inline reg_t r_mscratch();        // 读取 机器模式临时寄存器
static inline void  w_mscratch(reg_t x); // 写入 机器模式临时寄存器

static inline void w_mtvec(reg_t x);     // 写入 机器模式中断向量表基地址

static inline reg_t r_mstatus();         // 读取 机器模式状态寄存器
static inline void  w_mstatus(reg_t x);  // 写入 机器模式状态寄存器

static inline reg_t r_mie();             // 读取 机器模式中断使能寄存器
static inline void  w_mie(reg_t x);      // 写入 机器模式中断使能寄存器

static inline reg_t r_mepc();            // 读取 机器模式异常程序计数器
static inline void  w_mepc(reg_t x);     // 写入 机器模式异常程序计数器


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

// 读取 机器模式临时寄存器
static inline reg_t
r_mscratch()
{
    reg_t x;
    asm volatile("csrr %0, mscratch" : "=r"(x));
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
