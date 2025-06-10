.section .text.entry
.globl _start
_start:
    la sp, stack_top  # 设置栈指针
    call kernel_init  # 跳转到 C 入口
.section .bss
.align 4
stack:
    .space 4096
stack_top: