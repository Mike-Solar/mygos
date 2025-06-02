
// platform.h

#ifndef __PLATFORM_H__
#define __PLATFORM_H__


/*/
 * QEMU RISC-V Virt machine with 16550a UART and VirtIO MMIO
 *
 * maximum number of CPUs
 * see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
/*/

#define MAXNUM_CPU 8                 // 最大 CPU 数量，QEMU virt 机器支持最多 8 个 CPU 核心
#define STACK_SIZE 1024              // 每个 CPU 的栈大小为 1024 字节（1KB）
#define LENGTH_RAM 128 * 1024 * 1024 // = 128MB
#define SIZE_REG 4                   // 每个寄存器大小为 4 字节（32 位寄存器）
#define UART0 0x10000000L            // 指定 UART0 的物理地址，内核通过它进行字符收发。


/*/
 * UART0 interrupt source
 * see https://github.com/qemu/qemu/blob/master/include/hw/riscv/virt.h
 * enum {
 *     UART0_IRQ = 10,
 *     ......
 * };
 *
 * UART0 的中断号，用于中断控制器（PLIC）识别。
 * 参考 QEMU 的硬件头文件定义：UART0_IRQ = 10
/*/
#define UART0_IRQ 10 // UART0 的中断号，用于中断控制器（PLIC）识别

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
#define VIRT_PLIC_SIZE(__num_context) (VIRT_PLIC_CONTEXT_BASE + (__num_context) * VIRT_PLIC_CONTEXT_STRIDE)

/*/
#define PLIC_BASE 0x0c000000L                                                         // 外设基地址
#define PLIC_PRIORITY(id) (PLIC_BASE + (id) * 4)                                      // 设置设备中断优先级
#define PLIC_PENDING(id) (PLIC_BASE + 0x1000 + ((id) / 32) * 4)                       // 查询设备是否有挂起中断
#define PLIC_MENABLE(hart, id) (PLIC_BASE + 0x2000 + (hart) * 0x80 + ((id) / 32) * 4) // 启用某 hart 的某个中断
#define PLIC_MTHRESHOLD(hart) (PLIC_BASE + 0x200000 + (hart) * 0x1000)                // 设置中断门槛（优先级高于此值才会中断）
#define PLIC_MCLAIM(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)                    // 获取当前正在处理的中断ID
#define PLIC_MCOMPLETE(hart) (PLIC_BASE + 0x200004 + (hart) * 0x1000)                 // 完成中断处理，写入该 ID 通知 PLIC


/*/
 * CLINT（核心本地中断控制器）模块包含与软件中断和定时器中断相关的内存映射控制寄存器和状态寄存器。
 * QEMU 的 virt 虚拟机平台复用了 SiFive 的 CLINT 配置方式。
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
 * 在复位时，每个 msip 寄存器都被清零（不触发中断）。
/*/

#define CLINT_BASE 0x2000000L                                       // CLINT 模块基地址（从这里开始访问各种中断相关寄存器）
#define CLINT_MSIP(hartid) (CLINT_BASE + 4 * (hartid))              // 每个 hart 的 msip 软件中断控制寄存器（每个 hart 占 4 字节）
#define CLINT_MTIMECMP(hartid) (CLINT_BASE + 0x4000 + 8 * (hartid)) // 定时器比较值寄存器 mtimecmp，控制何时触发定时器中断
#define CLINT_MTIME (CLINT_BASE + 0xBFF8)                           // 当前时间寄存器 mtime（记录系统自启动以来的“周期数”）
#define CLINT_TIMEBASE_FREQ 10000000                                // 时间基准频率为 10 MHz（每秒计 10,000,000 次）


#endif /* __PLATFORM_H__ */
