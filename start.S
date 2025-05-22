.section .text
.global _start

_start:
    li a0, 'H'
    li a7, 0x100  # write syscall（QEMU SBI模拟）
    ecall

    li a0, 'i'
    li a7, 0x100
    ecall

1:  j 1b  # 死循环
