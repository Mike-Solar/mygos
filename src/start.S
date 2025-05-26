
# start.S

#include "platform.h"


.equ    STACK_SIZE, 1024    # size of each hart's stack is 1024 bytes
.global _start              # 系统入口
.text
_start:
    # park harts with id != 0
    csrr    t0, mhartid     # read current hart id
    mv      tp, t0          # keep CPU's hartid in its tp for later usage.
    bnez    t0, park        # if we're not on the hart 0 we park the hart

    # Set all bytes in the BSS section to zero.
    # 这段 RISC-V 汇编代码的作用是为 BSS 段清零。
    # BSS 段（Block Started by Symbol）用于存放未初始化的全局变量和静态变量。
    # 这些变量在程序启动时应被自动初始化为零。
    la      a0, _bss_start
    la      a1, _bss_end
    bgeu    a0, a1, 2f

1:
    sw      zero, (a0)      # 将寄存器 zero（恒为 0）的值存储到 a0 指向的内存地址处，相当于把当前地址的 4 字节内容清零。
    addi    a0, a0, 4       # 将 a0 寄存器的值加 4，指向下一个 4 字节的内存地址。
    bltu    a0, a1, 1b      # 如果 a0 小于 a1，则跳转回标签 1，继续清零下一个 4 字节的内存地址。

2:
    # Setup stacks, the stack grows from bottom to top, so we put the
    # stack pointer to the very end of the stack range.
    # 设置栈空间，栈 **从高地址向低地址增长** ，所以我们把 sp 指向栈空间的末尾
    slli    t0, t0, 10              # shift left the hart id by 1024
    la      sp, stacks + STACK_SIZE # 将初始堆栈指针设置为第一个堆栈空间的末尾
    add     sp, sp, t0              # 根据 hart id 偏移到对应的 hart 的堆栈空间

    j       start_kernel            # hart 0 jump to c

park:
    wfi
    j       park

# In the standard RISC-V calling convention, the stack pointer sp is always 16-byte aligned.
# 在标准 RISC-V 调用约定中，堆栈指针 sp 始终是 16 字节对齐的。
.balign 16
stacks:
.skip STACK_SIZE * MAXNUM_CPU       # allocate space for all the harts stacks # 为所有 hart 分配堆栈空间

.end                                # End of file


# 1: 是一个临时标签，数字可以是 0~9。
# 1b 表示“向后（backward）查找最近的 1: 标签”，即跳转到上面最近的 1: 标签。
# 1f 表示“向前（forward） 查找最近的 1: 标签”，即跳转到下面最近的 1: 标签。
