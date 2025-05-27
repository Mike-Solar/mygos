
// platform.h

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


#endif /* __PLATFORM_H__ */
