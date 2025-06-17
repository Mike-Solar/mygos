### RISC-V 通用寄存器（GPR）速查表

| 编号 | 缩写  | ABI 名称                  | 中文名称              | 功能描述                                                      |
| ---- | ----- | ------------------------- | --------------------- | ------------------------------------------------------------- |
| x0   | zero  | zero                      | 常数 0                | 只读寄存器，始终为零，写入无效。                              |
| x1   | ra    | return address            | 返回地址寄存器        | 用于函数调用返回地址（`jal` 等跳转指令保存处）。              |
| x2   | sp    | stack pointer             | 栈指针                | 指向当前栈顶，函数调用、局部变量分配等操作依赖此寄存器。      |
| x3   | gp    | global pointer            | 全局指针              | 一般由编译器使用，指向全局数据段基址（RV GCC ABI）。          |
| x4   | tp    | thread pointer            | 线程指针              | 多线程环境下指向当前线程控制块，或 hart ID。                  |
| x5   | t0    | temporary 0               | 临时寄存器 0          | 用作中间运算，不需在函数调用间保留值。                        |
| x6   | t1    | temporary 1               | 临时寄存器 1          | 同上。                                                        |
| x7   | t2    | temporary 2               | 临时寄存器 2          | 同上。                                                        |
| x8   | s0/fp | saved 0/frame pointer     | 保存寄存器 0 / 帧指针 | 函数调用中保存值；当作帧指针（frame pointer）时用于栈帧管理。 |
| x9   | s1    | saved 1                   | 保存寄存器 1          | 函数调用中必须保留的值（callee-saved）。                      |
| x10  | a0    | argument 0 / return value | 参数 0 / 返回值       | 用于函数参数或返回值（前 2 个参数/返回值寄存器）。            |
| x11  | a1    | argument 1 / return value | 参数 1 / 返回值       | 同上。                                                        |
| x12  | a2    | argument 2                | 参数 2                | 函数参数传递使用（总共最多支持 8 个参数）。                   |
| x13  | a3    | argument 3                | 参数 3                | 同上。                                                        |
| x14  | a4    | argument 4                | 参数 4                | 同上。                                                        |
| x15  | a5    | argument 5                | 参数 5                | 同上。                                                        |
| x16  | a6    | argument 6                | 参数 6                | 同上。                                                        |
| x17  | a7    | argument 7                | 参数 7                | 同上。                                                        |
| x18  | s2    | saved 2                   | 保存寄存器 2          | Callee-saved 寄存器，函数调用必须保存和恢复。                 |
| x19  | s3    | saved 3                   | 保存寄存器 3          | 同上。                                                        |
| x20  | s4    | saved 4                   | 保存寄存器 4          | 同上。                                                        |
| x21  | s5    | saved 5                   | 保存寄存器 5          | 同上。                                                        |
| x22  | s6    | saved 6                   | 保存寄存器 6          | 同上。                                                        |
| x23  | s7    | saved 7                   | 保存寄存器 7          | 同上。                                                        |
| x24  | s8    | saved 8                   | 保存寄存器 8          | 同上。                                                        |
| x25  | s9    | saved 9                   | 保存寄存器 9          | 同上。                                                        |
| x26  | s10   | saved 10                  | 保存寄存器 10         | 同上。                                                        |
| x27  | s11   | saved 11                  | 保存寄存器 11         | 同上。                                                        |
| x28  | t3    | temporary 3               | 临时寄存器 3          | 用于中间结果，caller-saved。                                  |
| x29  | t4    | temporary 4               | 临时寄存器 4          | 同上。                                                        |
| x30  | t5    | temporary 5               | 临时寄存器 5          | 同上。                                                        |
| x31  | t6    | temporary 6               | 临时寄存器 6          | 同上。                                                        |

---

### RISC-V 特殊寄存器缩写全称表

| 缩写     | 类型 | 英文全称                          | 中文全称             | 功能描述                                                 |
| -------- | ---- | --------------------------------- | -------------------- | -------------------------------------------------------- |
| mhartid  | CSR  | Machine Hardware Thread ID        | 硬件线程编号         | 当前 hart（核心）的 ID，用于多核系统区分处理器。         |
| mepc     | CSR  | Machine Exception Program Counter | 异常返回地址寄存器   | 保存异常/中断发生前的 PC 值。                            |
| mscratch | CSR  | Machine Scratch Register          | 机器暂存寄存器       | 由操作系统使用，在中断处理期间保存临时数据（如上下文）。 |
| mtvec    | CSR  | Machine Trap-Vector Base Address  | 陷阱向量表地址寄存器 | 设置 trap（中断/异常）处理函数入口地址。                 |
| mstatus  | CSR  | Machine Status Register           | 机器状态寄存器       | 包含中断使能、特权级等重要状态位。                       |
| mie      | CSR  | Machine Interrupt Enable          | 中断使能寄存器       | 各类中断源的使能控制位。                                 |
| mcause   | CSR  | Machine Cause Register            | 中断/异常原因寄存器  | 指明 trap 原因，是中断还是异常及具体类型。               |
| mip      | CSR  | Machine Interrupt Pending         | 中断挂起寄存器       | 显示哪些中断正在等待处理中。                             |
| mtimecmp | MMIO | Machine Timer Compare Register    | 定时器比较寄存器     | 当 `mtime >= mtimecmp` 时触发定时器中断，由 CLINT 提供。 |
| mtime    | MMIO | Machine Timer Register            | 机器时间寄存器       | 记录自系统启动以来的计时器周期数，由 CLINT 提供。        |
