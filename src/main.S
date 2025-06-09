.section .boot.text
.globl _start
_start:
    // park harts with id != 0
    csrr    t0, mhartid     // read current hart id
    mv      tp, t0          // keep CPU's hartid in its tp for later usage.
    bnez    t0, park        // if we're not on the hart 0 we park the hart

    // 将 BSS 段清零。
    la      a0, _bss_start
    la      a1, _bss_end
    bgeu    a0, a1, 2f      // 如果 a0 >= a1，说明 BSS 段已经清零完成
1:
    sw      zero, (a0)      // 把 a0 指向的 4 字节内容清零。
    addi    a0, a0, 4       // 指向下一个 4 字节的内存地址。
    bltu    a0, a1, 1b      // 如果 a0 小于 a1，则跳转回标签 1，继续清零下一个 4 字节的内存地址。

2:
    /*/
     * 在 start_kernel 结束时，schedule() 将调用 MRET 切换到第一个任务，因此在这里准备 mstatus 。
     * 注意：最好不要假设 mstatus 的初始值为零。
     * 把 mstatus.MPP 设置为 3，以便在 MRET 后仍然以机器模式运行。
     * 把 mstatus.MPIE 设置为 1，以便 MRET 将启用中断。
    /*/
	li	t0, 3 << 11 | 1 << 7
	csrs	mstatus, t0

    la sp, stack_top  # 设置栈指针
    call kernel_init  # 跳转到 C 入口

park:
    wfi
    j       park

.section .bss
.align 4
stack:
    .space 4096
stack_top: