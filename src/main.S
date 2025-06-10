.section .text.entry
.globl _start
_start:
    la sp, stack_top  # 设置栈指针
    la t1, early_page_dir
    li t2, 4096
    srli t1, t1, 30       # Sv39中，1GB大页对应VPN[2]索引（右移30位）
    slli t1, t1, 3        # 计算页表项偏移（每个PTE占8字节）
    add t2, t2, t1        # 得到目标页表项的物理地址
    li a2, 0x80200000 >> 2
    ori a2, a2, (PTE_V | PTE_R | PTE_W | PTE_X)  # 设置标志位
    sd a2, (t2)          # 将页表项写入内存

    srli t1, t1, 12  # satp的PPN字段需要页表物理地址右移12位
    li t2, SATP_MODE_SV39    # Sv39模式
    or t1, t1, t2
    csrw satp, t1            # 写入satp寄存器
    sfence.vma               # 刷新TLB

    call kernel_init  # 跳转到 C 入口
.section .bss
.align 4
stack:
    .space 4096
stack_top: