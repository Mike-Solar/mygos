
// io/trap_handler.c

#include "os.h"

#include "io.h"

#include "platform.h"
#include "riscv.h"


// 外部中断处理函数
void
external_interrupt_handler()
{
    // 返回中断源号
    uint32_t irq = plic_claim();

    // 处理中断
    if(irq == UART0_IRQ)
    {
        char ch = (char)uart_getc();

        printf("Received character: (0x%02x)\n", ch);
    }
    else if(irq) printf("unexpected interrupt irq = %d\n", irq);

    // 将中断标记为已响应
    if(irq) plic_complete(irq);
}
