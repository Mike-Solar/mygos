
// entry.S

#define LOAD        lw      // 定义加载指令：LOAD 表示 lw（Load Word）
#define STORE       sw      // 定义存储指令：STORE 表示 sw（Store Word）
#define SIZE_REG    4       // 每个寄存器大小为 4 字节（32 位寄存器）

// Save all General-Purpose(GP) registers to context.
// struct context *base = &ctx_task;
// base->ra = ra;
// ......
// These GP registers to be saved don't include gp
// and tp, because they are not caller-saved or
// callee-saved. These two registers are often used
// for special purpose. For example, in RVOS, 'tp'
// (aka "thread pointer") is used to store hartid,
// which is a global value and would not be changed
// during context-switch.
//
// 保存所有通用寄存器（General-Purpose, GP）到上下文结构中
// 相当于 C 伪代码：
// struct context *base = &ctx_task;
// base->ra = ra;
// ......
// 注意：这里保存的 GP 寄存器不包括 gp 和 tp，
// 因为它们既不是 caller-saved（调用者负责保存），也不是 callee-saved（被调用者负责保存）
// 它们常用于特殊用途。例如在 RVOS 中，tp（线程指针）用于存储 hartid（硬件线程编号），
// 这个值是全局固定的，在上下文切换过程中不会改变。
.macro reg_save base
	STORE ra,   0*SIZE_REG(\base)
	STORE sp,   1*SIZE_REG(\base)
	STORE t0,   4*SIZE_REG(\base)
	STORE t1,   5*SIZE_REG(\base)
	STORE t2,   6*SIZE_REG(\base)
	STORE s0,   7*SIZE_REG(\base)
	STORE s1,   8*SIZE_REG(\base)
	STORE a0,   9*SIZE_REG(\base)
	STORE a1,  10*SIZE_REG(\base)
	STORE a2,  11*SIZE_REG(\base)
	STORE a3,  12*SIZE_REG(\base)
	STORE a4,  13*SIZE_REG(\base)
	STORE a5,  14*SIZE_REG(\base)
	STORE a6,  15*SIZE_REG(\base)
	STORE a7,  16*SIZE_REG(\base)
	STORE s2,  17*SIZE_REG(\base)
	STORE s3,  18*SIZE_REG(\base)
	STORE s4,  19*SIZE_REG(\base)
	STORE s5,  20*SIZE_REG(\base)
	STORE s6,  21*SIZE_REG(\base)
	STORE s7,  22*SIZE_REG(\base)
	STORE s8,  23*SIZE_REG(\base)
	STORE s9,  24*SIZE_REG(\base)
	STORE s10, 25*SIZE_REG(\base)
	STORE s11, 26*SIZE_REG(\base)
	STORE t3,  27*SIZE_REG(\base)
	STORE t4,  28*SIZE_REG(\base)
	STORE t5,  29*SIZE_REG(\base)
	// we don't save t6 here, due to we have used
	// it as base, we have to save t6 in an extra step
	// outside of reg_save
    // 不在这里保存 t6，因为 t6 已被用作 base 参数，
    // 所以必须在 reg_save 之外单独保存 t6
.endm


// 从上下文中恢复所有通用寄存器（除了 gp 和 tp）
// 类似于 C 伪代码：
// struct context *base = &ctx_task;
// ra = base->ra;
// ......
.macro reg_restore base
	LOAD ra,   0*SIZE_REG(\base)
	LOAD sp,   1*SIZE_REG(\base)
	LOAD t0,   4*SIZE_REG(\base)
	LOAD t1,   5*SIZE_REG(\base)
	LOAD t2,   6*SIZE_REG(\base)
	LOAD s0,   7*SIZE_REG(\base)
	LOAD s1,   8*SIZE_REG(\base)
	LOAD a0,   9*SIZE_REG(\base)
	LOAD a1,  10*SIZE_REG(\base)
	LOAD a2,  11*SIZE_REG(\base)
	LOAD a3,  12*SIZE_REG(\base)
	LOAD a4,  13*SIZE_REG(\base)
	LOAD a5,  14*SIZE_REG(\base)
	LOAD a6,  15*SIZE_REG(\base)
	LOAD a7,  16*SIZE_REG(\base)
	LOAD s2,  17*SIZE_REG(\base)
	LOAD s3,  18*SIZE_REG(\base)
	LOAD s4,  19*SIZE_REG(\base)
	LOAD s5,  20*SIZE_REG(\base)
	LOAD s6,  21*SIZE_REG(\base)
	LOAD s7,  22*SIZE_REG(\base)
	LOAD s8,  23*SIZE_REG(\base)
	LOAD s9,  24*SIZE_REG(\base)
	LOAD s10, 25*SIZE_REG(\base)
	LOAD s11, 26*SIZE_REG(\base)
	LOAD t3,  27*SIZE_REG(\base)
	LOAD t4,  28*SIZE_REG(\base)
	LOAD t5,  29*SIZE_REG(\base)
	LOAD t6,  30*SIZE_REG(\base)
.endm

// Something to note about save/restore:
// - We use mscratch to hold a pointer to context of current task
// - We use t6 as the 'base' for reg_save/reg_restore, because it is the
//   very bottom register (x31) and would not be overwritten during loading.
//   Note: CSRs(mscratch) can not be used as 'base' due to load/restore
//   instruction only accept general purpose registers.
//
// 保存/恢复过程需要注意的事项：
// - 使用 CSR 寄存器 mscratch 存放当前任务上下文的指针
// - 使用 t6 作为 reg_save/reg_restore 的 base，因为 t6 是编号最高的寄存器（x31），在加载期间不会被覆盖
// - 注意：mscratch 不能被当作 base 来直接进行 load/store 操作，因为这些指令只能使用通用寄存器

.text

// void switch_to(struct context *next);
// 参数：a0 = 下一个任务的上下文结构体指针
.globl switch_to
.balign 4
switch_to:
    csrrw   t6, mscratch, t6        // 交换 t6 与 mscratch 的值：把原本 t6 的值暂时保存在 mscratch 中，等会换回来
    beqz    t6, 1f                  // 如果 t6 为 0，说明是第一次调用 switch_to（来自 sched_init），这是一个特殊情况，不需要保存当前上下文

    // 保存当前任务的寄存器上下文到 t6 指向的地址
    reg_save t6

    // 保存实际的 t6 寄存器（它刚才被交换到 mscratch 中）
    mv      t5, t6                  // t5 也指向当前任务上下文
    csrr    t6, mscratch            // 把 t6 的原值（mscratch 中）读回来
    STORE   t6, 30*SIZE_REG(t5)     // 把 t6 保存到上下文中的 t6 位置

1:
    // 切换 mscratch，使其指向下一个任务的上下文
    csrw    mscratch, a0            // a0就是参数，表示一个 context 结构体的地址
    mv      t6, a0                  // 使用 t6 指向下一个任务的上下文

    // 恢复下一个任务的所有通用寄存器
    reg_restore t6

    // 执行真正的上下文切换（ret = 返回到下一个任务）
    ret

# interrupts and exceptions while in machine mode come here.
.globl trap_vector
# the trap vector base address must always be aligned on a 4-byte boundary
.balign 4
trap_vector:
	# save context(registers).
	csrrw	t6, mscratch, t6	# swap t6 and mscratch
	reg_save t6

	# Save the actual t6 register, which we swapped into
	# mscratch
	mv	t5, t6			# t5 points to the context of current task
	csrr	t6, mscratch		# read t6 back from mscratch
	STORE	t6, 30*SIZE_REG(t5)	# save t6 with t5 as base

	# Restore the context pointer into mscratch
	csrw	mscratch, t5

	# call the C trap handler in trap.c
	csrr	a0, mepc
	csrr	a1, mcause
	call	trap_handler // 调用异常处理函数

	# trap_handler will return the return address via a0.
	csrw	mepc, a0

	# restore context(registers).
	csrr	t6, mscratch
	reg_restore t6

	# return to whatever we were doing before trap.
	mret


.end