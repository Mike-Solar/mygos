
// platform.h

/*/

| 缩写         | 英文全称                                    | 中文全称                 | 中文解释                                                          |
| ------------ | ------------------------------------------- | ------------------------ | ----------------------------------------------------------------- |
| **CLINT**    | Core Local Interruptor                      | 核心本地中断控制器       | 管理软件中断和定时器中断的硬件模块，针对每个 CPU 核（hart）独立。 |
| **PLIC**     | Platform-Level Interrupt Controller         | 平台级中断控制器         | 管理外设硬件中断并分发给各个 CPU 核。                             |
| **MSIP**     | Machine Software Interrupt Pending          | 机器模式软件中断挂起位   | `mip` 寄存器中的一位，表示该 hart 的软件中断是否挂起。            |
| **MTIMECMP** | Machine Timer Compare Register              | 机器模式定时器比较寄存器 | 控制定时器中断何时触发，当 `mtime >= mtimecmp` 时触发。           |
| **MTIME**    | Machine Timer Register                      | 机器模式时间寄存器       | 记录自系统启动以来的时钟周期数。                                  |
| **MSIE**     | Machine Software Interrupt Enable           | 机器模式软件中断使能     | `mie` 寄存器中的一位，用于开启/关闭软件中断。                     |
| **MTIE**     | Machine Timer Interrupt Enable              | 机器模式定时器中断使能   | `mie` 中的某位，控制是否响应定时器中断。                          |
| **MEIE**     | Machine External Interrupt Enable           | 机器模式外部中断使能     | `mie` 中的某位，控制是否响应外部中断（如 UART）。                 |
| **UART**     | Universal Asynchronous Receiver-Transmitter | 通用异步收发器           | 一个串口通信控制器，用于终端输入输出。                            |
| **IRQ**      | Interrupt Request                           | 中断请求                 | 外部设备向处理器发出的一种中断信号。                              |
| **WARL**     | Write-Any Read-Legal                        | 可写任意值、只读合法值   | RISC-V 的寄存器类型，写时可以任意值，读出的是被接受的合法值。     |
| **CSR**      | Control and Status Register                 | 控制与状态寄存器         | 包含各种控制和状态信息的特殊寄存器集合。                          |
| **MIP**      | Machine Interrupt Pending                   | 机器中断挂起寄存器       | 记录哪些中断正在等待处理。                                        |
| **MIE**      | Machine Interrupt Enable                    | 机器中断使能寄存器       | 控制哪些中断允许被处理。                                          |
| **MTVEC**    | Machine Trap-Vector Base Address            | 机器模式陷阱向量基地址   | 设置中断或异常时跳转的处理函数地址。                              |
| **MSCRATCH** | Machine Scratch Register                    | 机器模式暂存寄存器       | 用于中断时保存中间数据（由软件使用）。                            |
| **MEPC**     | Machine Exception Program Counter           | 机器异常程序计数器       | 异常发生时，记录被中断的指令地址。                                |
| **MCAUSE**   | Machine Cause Register                      | 机器异常原因寄存器       | 保存异常或中断的原因。                                            |
| **MSTATUS**  | Machine Status Register                     | 机器状态寄存器           | 控制处理器模式、中断权限等重要状态。                              |
| **HART**     | Hardware Thread                             | 硬件线程（处理器核）     | 每个 hart 表示一个独立的执行单元，相当于“核”。                    |
| **QEMU**     | Quick Emulator                              | 快速模拟器               | 支持模拟多种架构，包括 RISC-V 的虚拟机。                          |

/*/

#ifndef __PLATFORM_H__
#define __PLATFORM_H__

/*/
 * QEMU RISC-V Virt machine with 16550a UART and VirtIO MMIO
 *
 * maximum number of CPUs
 * see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
/*/

// #define VIRT_CPUS_MAX 8
#define MAXNUM_CPU 8

/* used in os.ld */
#define LENGTH_RAM 128 * 1024 * 1024 // = 128MB

/*/

MemoryMap
see https://github.com/qemu/qemu/blob/master/hw/riscv/virt.c, virt_memmap[]

| 地址范围     | 设备        | 描述                                |
| ------------ | ----------- | ----------------------------------- |
| `0x00001000` | boot ROM    | QEMU 提供的启动 ROM                 |
| `0x02000000` | CLINT       | Core Local Interrupt Controller     |
| `0x0C000000` | PLIC        | Platform Level Interrupt Controller |
| `0x10000000` | UART0       | 串口 UART（输出调试信息）           |
| `0x10001000` | virtio disk | 虚拟磁盘                            |
| `0x80000000` | DRAM base   | 内核加载的起始地址                  |

/*/

/* This machine puts UART registers here in physical memory. */
/* 指定 UART0 的物理地址，内核通过它进行字符收发。*/
#define UART0 0x10000000L


/*
 * UART0 interrupt source
 * see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
 * enum {
 *     UART0_IRQ = 10,
 *     ......
 * };
 *
 * UART0 的中断号，用于中断控制器（PLIC）识别。
 * 参考 QEMU 的硬件头文件定义：UART0_IRQ = 10
 */
#define UART0_IRQ 10

/*/

This machine puts platform-level interrupt controller (PLIC) here.
Here only list PLIC registers in Machine mode.
see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h

#define VIRT_PLIC_HART_CONFIG "MS"
#define VIRT_PLIC_NUM_SOURCES 127
#define VIRT_PLIC_NUM_PRIORITIES 7
#define VIRT_PLIC_PRIORITY_BASE 0x00
#define VIRT_PLIC_PENDING_BASE 0x1000
#define VIRT_PLIC_ENABLE_BASE 0x2000
#define VIRT_PLIC_ENABLE_STRIDE 0x80
#define VIRT_PLIC_CONTEXT_BASE 0x200000
#define VIRT_PLIC_CONTEXT_STRIDE 0x1000
#define VIRT_PLIC_SIZE(__num_context) \
    (VIRT_PLIC_CONTEXT_BASE + (__num_context) * VIRT_PLIC_CONTEXT_STRIDE)

| 宏名                     | 说明                                   |
| ------------------------ | -------------------------------------- |
| `PLIC_PRIORITY(id)`      | 设置设备中断优先级                     |
| `PLIC_PENDING(id)`       | 查询设备是否有挂起中断                 |
| `PLIC_MENABLE(hart, id)` | 启用某 hart 的某个中断                 |
| `PLIC_MTHRESHOLD(hart)`  | 设置中断门槛（优先级高于此值才会中断） |
| `PLIC_MCLAIM(hart)`      | 获取当前正在处理的中断 ID              |
| `PLIC_MCOMPLETE(hart)`   | 完成中断处理，写入该 ID 通知 PLIC      |

/*/
#define PLIC_BASE 0x0c000000L
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)
#define PLIC_MENABLE(hart, id) (PLIC_BASE + 0x2000 + (hart) * 0x80 + ((id) / 32) * 4)
#define PLIC_MTHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)
#define PLIC_MCOMPLETE(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)


/*/
 * The Core Local Interruptor (CLINT) block holds memory-mapped control and status registers associated with software and timer interrupts.
 * QEMU-virt reuses sifive configuration for CLINT.
 * see https://gitee.com/qemu/qemu/blob/master/include/hw/riscv/sifive_clint.h
 *
 * enum {
 * 	SIFIVE_SIP_BASE     = 0x0,      // 软件中断寄存器的基地址偏移量
 * 	SIFIVE_TIMECMP_BASE = 0x4000,   // 定时中断触发比较值（mtimecmp）
 * 	SIFIVE_TIME_BASE    = 0xBFF8    // 当前系统时间（mtime）
 * };
 *
 * enum {
 * 	SIFIVE_CLINT_TIMEBASE_FREQ = 10000000 // CLINT 的时间基准频率是 10MHz（1 秒 = 10⁷ 个计时周期）。
 * };
 *
 * Notice:
 * The machine-level MSIP bit of mip register are written by accesses to memory-mapped control registers, which are used by remote harts to provide machine-mode inter-processor interrupts.
 * For QEMU-virt machine, Each msip register is a 32-bit wide WARL register
 * where the upper 31 bits are tied to 0. The least significant bit is
 * reflected in the MSIP bit of the mip CSR. We can write msip to generate
 * machine-mode software interrupts. A pending machine-level software
 * interrupt can be cleared by writing 0 to the MSIP bit in mip.
 * On reset, each msip register is cleared to zero.
 *
 *
 * CLINT（核心本地中断控制器）模块包含与软件中断和定时器中断相关的内存映射控制寄存器和状态寄存器。
 * QEMU 的 virt 虚拟机平台复用了 SiFive 的 CLINT 配置方式。
 *
 * 注意：
 * mip 寄存器中的 machine-level 软件中断位（MSIP） 是通过访问内存映射的控制寄存器写入的。
 * 这些寄存器允许不同的 hart（核）之间互相发送机器模式下的中断（也就是 IPI：Inter-Processor Interrupt）。
 *
 * 在 QEMU-virt 机器中，每个 msip 寄存器是一个 32 位宽的 WARL 类型寄存器（Write-Any Read-Legal，可写任意值但只识别合法值）。
 * 其中 高 31 位固定为 0，仅最低位有效。
 * 最低位与 mip 寄存器中的 MSIP 位一一对应。
 *
 * 我们可以通过写入 msip 来产生机器模式的软件中断。
 * 一个挂起的软件中断可以通过将 msip 写为 0 来清除 mip 中的 MSIP 位。
 * 在复位时，每个 msip 寄存器都被清零（不触发中断）     。
/*/

#define CLINT_BASE 0x2000000L                                       // CLINT 模块基地址（从这里开始访问各种中断相关寄存器）
#define CLINT_MSIP(hartid) (CLINT_BASE + 4 * (hartid))              // 每个 hart 的 msip 软件中断控制寄存器（每个 hart 占 4 字节）
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + 8 * (hartid)) // 定时器比较值寄存器 mtimecmp，控制何时触发定时器中断
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)                           // 当前时间寄存器 mtime（记录系统自启动以来的“周期数”）
#define CLINT_TIMEBASE_FREQ 10000000                                // 时间基准频率为 10 MHz（每秒计 10,000,000 次）


#endif /* __PLATFORM_H__ */
