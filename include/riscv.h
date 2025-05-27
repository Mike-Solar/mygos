
// riscv.h
// ref: https://github.com/mit-pdos/xv6-riscv/blob/riscv/kernel/riscv.h

/*/

## ✅ RISC-V 特殊寄存器缩写全称表

| 缩写       | 类型 | 英文全称                          | 中文解释                          | 功能描述                                                                                     |
| ---------- | ---- | --------------------------------- | --------------------------------- | -------------------------------------------------------------------------------------------- |
| `tp`       | GPR  | Thread Pointer                    | 线程指针（常用于存 hartid）       | 线程指针（Thread Pointer）。在 RVOS 中用来表示当前 hart 的编号（`hartid`）。常用于多核调度。 |
| ---------- | ---- | --------------------------------- | --------------------------------- | -------------------------------------------------------------------------------------------- |
| `mhartid`  | CSR  | Machine Hardware Thread ID        | 机器模式硬件线程编号（CPU 核 ID） | 当前硬件线程 ID（即 CPU 核心编号）。每个 hart 在启动时都会从这个寄存器中读取自己的编号。     |
| `mepc`     | CSR  | Machine Exception Program Counter | 机器模式异常程序计数器            | **机器模式异常程序计数器**，保存异常/中断发生时要返回的指令地址。                            |
| `mscratch` | CSR  | Machine Scratch Register          | 机器模式临时寄存器                | 提供一个供操作系统使用的临时寄存器，常在异常/中断上下文切换时用作保存当前任务的上下文指针。  |
| `mtvec`    | CSR  | Machine Trap-Vector Base Address  | 机器模式陷阱向量表基地址          | 机器模式中断向量表的地址。系统初始化时设置，指定中断处理函数的入口地址。                     |
| ---------- | ---- | --------------------------------- | --------------------------------- | -------------------------------------------------------------------------------------------- |
| `mstatus`  | CSR  | Machine Status Register           | 机器模式状态寄存器                | **机器模式状态寄存器**，包含当前特权级、全局中断使能等重要状态标志。                         |
| `mie`      | CSR  | Machine Interrupt Enable          | 机器模式中断使能寄存器            | 机器模式中断使能寄存器，用于控制各类中断的使能状态。                                         |
| `mcause`   | CSR  | Machine Cause Register            | 机器模式异常/中断原因寄存器       | 保存导致中断/异常的原因编码。用于判断进入 trap 的原因。                                      |

---


## ✅ `mstatus` 内部位名缩写一览

| 位名称 | 英文全称                             | 含义说明                              |
| ------ | ------------------------------------ | ------------------------------------- |
| `MPP`  | Machine Previous Privilege           | 中断前的特权级（00 U, 01 S, 11 M）    |
| `SPP`  | Supervisor Previous Privilege        | 中断前的监督级特权（如果进入 S 模式） |
| `MPIE` | Machine Previous Interrupt Enable    | 中断前机器模式中断是否启用            |
| `SPIE` | Supervisor Previous Interrupt Enable | 中断前监督模式中断是否启用            |
| `UPIE` | User Previous Interrupt Enable       | 中断前用户模式中断是否启用            |
| `MIE`  | Machine Interrupt Enable             | 当前是否启用机器模式中断              |
| `SIE`  | Supervisor Interrupt Enable          | 启用监督模式中断                      |
| `UIE`  | User Interrupt Enable                | 启用用户模式中断                      |

---

## ✅ `mie` 各类中断控制位说明

| 宏定义     | 英文全称                          | 含义说明           |
| ---------- | --------------------------------- | ------------------ |
| `MIE_MEIE` | Machine External Interrupt Enable | 机器外部中断使能   |
| `MIE_MTIE` | Machine Timer Interrupt Enable    | 机器定时器中断使能 |
| `MIE_MSIE` | Machine Software Interrupt Enable | 机器软件中断使能   |

---

## ✅ `mcause` 掩码宏定义说明

| 宏名                    | 含义                               |
| ----------------------- | ---------------------------------- |
| `MCAUSE_MASK_INTERRUPT` | 高位掩码（第 31 位）表示是否为中断 |
| `MCAUSE_MASK_ECODE`     | 低 31 位掩码，表示异常或中断原因码 |

/*/

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
#define MIE_MEIE (1 << 11) // external
#define MIE_MTIE (1 << 7)  // timer
#define MIE_MSIE (1 << 3)  // software

/* Machine-mode Cause Masks */
// 机器模式异常/中断原因寄存器的掩码宏定义
#define MCAUSE_MASK_INTERRUPT (reg_t)0x80000000
#define MCAUSE_MASK_ECODE (reg_t)0x7FFFFFFF


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
