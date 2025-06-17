
// uart.c

#include "io.h"
#include "platform.h"
#include "types.h"


// 初始化 UART，配置波特率和数据格式
void
uart_init()
{
    // 禁用中断。
    // 这一步是为了确保在配置 UART 时不会有中断干扰。
    uart_write_reg(IER, 0x00);

    /*/
     * Setting baud rate.
     *
     * Just a demo here if we care about the divisor, but for our purpose [QEMU-virt], this doesn't really do anything.
     *
     * Notice that the divisor register DLL (divisor latch least) and DLM (divisor latch most) have the same base address as the receiver/transmitter and the interrupt enable register.
     * To change what the base address points to, we open the "divisor latch" by writing 1 into the Divisor Latch Access Bit (DLAB), which is bit index 7 of the Line Control Register (LCR).
     * Regarding the baud rate value, see [1] "BAUD RATE GENERATOR PROGRAMMING TABLE".
     * We use 38.4K when 1.8432 MHZ crystal, so the corresponding value is 3.
     * And due to the divisor register is two bytes (16 bits), so we need to split the value of 3(0x0003) into two bytes, DLL stores the low byte, DLM stores the high byte.
     *
     * 设置波特率。
     *
     * 如果我们关心除数，这里只是一个演示，但对于我们的目的 [QEMU-virt]，这实际上并没有做任何事情。
     *
     * 请注意，除数寄存器 DLL （除数锁存器最小） 和 DLM （除数锁存器最大） 与接收器/发送器和中断使能寄存器具有相同的基址。
     * 为了改变基址指向的内容，我们通过将 1 写入除数锁存器访问位 （DLAB） 来打开“除数锁存器”，DLAB 是线路控制寄存器 （LCR） 的位索引 7。
     * 关于波特率值，请参见 [1] “波特率生成器编程表”。
     * 当 1.8432 MHZ 晶体时，我们使用 38.4K，因此对应的值为 3。
     * 并且由于除数寄存器是两个字节（16 位），所以我们需要将 3（0x0003） 的值分成两个字节，DLL 存储低字节，DLM 存储高字节。
    /*/
    uint8_t lcr = uart_read_reg(LCR);    // 读取线路控制寄存器（LCR）的当前值
    uart_write_reg(LCR, lcr | (1 << 7)); // 打开除数锁存器访问位（DLAB），允许我们设置波特率
    uart_write_reg(DLL, 0x03);           // 设置除数锁存器低字节（DLL），波特率值的低字节为 0x03
    uart_write_reg(DLM, 0x00);           // 设置除数锁存器高字节（DLM），波特率值的高字节为 0x00

    /*/
     * Continue setting the asynchronous data communication format.
     * - number of the word length: 8 bits
     * - number of stop bits：1 bit when word length is 8 bits
     * - no parity
     * - no break control
     * - disabled baud latch
     *
     * 继续设置异步数据通信格式。
     * - 字长：8 bit
     * - 停止位：1 bit（当字长为 8 bit 时）
     * - 无奇偶校验
     * - 无断开控制
     * - 禁用波特率锁存器
     *
     * 继续设置异步数据通信格式。
    /*/
    lcr = 0;
    uart_write_reg(LCR, lcr | (3 << 0));

    // 启用接收中断
    uint8_t ier = uart_read_reg(IER);
    uart_write_reg(IER, ier | (1 << 0));
}

// 输出一个字符到串口（阻塞，等待发送缓冲区空）
void
uart_putc(char ch)
{
    while((uart_read_reg(LSR) & LSR_TX_IDLE) == 0);
    uart_write_reg(THR, ch);
}

// 输出字符串到串口（逐字符发送）
void
uart_puts(char* s)
{
    while(*s) uart_putc(*s++);
}

// 接收一个字符（阻塞，直到接收缓冲区有数据）
char
uart_getc()
{
    while(0 == (uart_read_reg(LSR) & LSR_RX_READY));
    return uart_read_reg(RHR);
}
