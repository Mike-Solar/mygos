
// tasks/input.c

#include "io.h"
#include "os.h"


char input_buffer[64] = { 0 }; // 输入缓冲区
int  index            = 0;     // 缓冲区索引
void
test_input()
{
    spin_lock(); // 获取锁

    // 输出提示信息
    uart_puts("Please enter a character:\n");

    // 等待输入字符

    char input_char = 0;
    while(1)
    {
        while((input_char = uart_getc()) == 0); // 阻塞等待接收字符

        switch(input_char)
        {
        case 0x0000000d:                                 // 如果输入的是换行符
            input_buffer[index] = '\0';                  // 结束字符串
            printf("\nYou entered: %s\n", input_buffer); // 输出输入的内容
            index = 0;                                   // 重置索引
            printf("Please enter a character:\n");       // 再次提示输入
            break;
        case 0x00000008:                                 // 如果输入的是退格符
            if(index > 0)                                // 如果索引大于 0
            {
                index--;                                 // 索引减 1
                uart_putc('\b');                         // 输出退格符
                uart_putc(' ');                          // 输出空格覆盖原字符
                uart_putc('\b');                         // 再输出退格符回到原位置
            }
            break;
        default:                                    // 其他字符
            if(index < sizeof(input_buffer) - 1)    // 如果索引小于缓冲区大小
            {
                input_buffer[index++] = input_char; // 将字符存入缓冲区并增加索引
                uart_putc(input_char);              // 输出接收到的字符
            }
            else
            {
                uart_puts("\nBuffer full! Please press Enter to continue.\n");
                while(uart_getc() != '\n'); // 等待用户按下回车键
                index           = 0;        // 重置索引
                input_buffer[0] = '\0';     // 清空缓冲区
            }
            break;
        }
    }

    printf("Input test completed!\n");

    spin_unlock(); // 释放锁
}
