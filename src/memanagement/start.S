
# 引入平台定义头文件（通常包含 MAXNUM_CPU 等常量）
#include "platform.h"       

    .equ    STACK_SIZE, 1024 # 每个 hart（硬件线程）的栈大小为 1024 字节
    .global _start           # 声明全局符号 _start，作为程序入口
    .text

_start:
    # 将 hart id 不等于 0 的核挂起（仅 hart 0 执行内核初始化）
    csrr    t0, mhartid      # 从 CSR 寄存器中读取当前 hart（核）编号到 t0
    mv      tp, t0           # 将 hart id 存入 tp 寄存器，供后续 C 程序使用
    bnez    t0, park         # 如果 hart id 不是 0（t0 ≠ 0），跳转到 park，挂起该核

    # 将 BSS 段中的所有字节清零（未初始化全局变量）
    la      a0, _bss_start   # 加载 BSS 段起始地址到 a0
    la      a1, _bss_end     # 加载 BSS 段结束地址到 a1
    bgeu    a0, a1, 2f       # 如果起始地址 ≥ 结束地址，跳转到标签 2，跳过清零过程

1:
    sw      zero, (a0)       # 将 0 写入当前地址
    addi    a0, a0, 4        # a0 加 4（按 4 字节单位向后移动）
    bltu    a0, a1, 1b       # 如果 a0 < a1，继续循环清零

2:
    # 设置当前 hart 的栈指针（sp）
    # 注意：RISC-V 的栈是向下增长的
    slli    t0, t0, 10       # 将 hart id 左移 10 位，相当于乘以 1024（栈大小）
    la      sp, stacks + STACK_SIZE  # 加载第一个栈空间的末尾地址（作为基础 sp）
    add     sp, sp, t0       # 加上偏移，得到当前 hart 的专属栈顶

    j       start_kernel     # 跳转执行 C 函数 start_kernel（初始化内核）

park:
    wfi                     # 等待中断（Wait For Interrupt），降低功耗挂起
    j       park            # 自旋等待：再次跳转到 park 标签

.balign 16 # 按照 RISC-V 调用约定，栈指针 sp 必须保持 16 字节对齐
stacks:
    .skip   STACK_SIZE * MAXNUM_CPU  # 为每个 hart 分配 STACK_SIZE 大小的栈空间

    .end                     # 文件结束标志
