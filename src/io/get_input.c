
// io/get_input.c

#include "io.h"


#define ENTER_KEY 0x0000000d     // 回车键 ASCII 码
#define BACKSPACE_KEY 0x0000007f // 退格键 ASCII 码


void
get_input(char* input_buffer, int buffer_size)
{
    spin_lock();   // 获取锁

    int index = 0; // 缓冲区索引

    char input_char = 0;
    do
    {
        input_char = uart_getc(); // 阻塞等待接收字符

        switch(input_char)
        {
        case ENTER_KEY:                 // 如果输入的是换行符
            input_buffer[index] = '\0'; // 结束字符串
            index               = 0;    // 重置索引
            break;

        case BACKSPACE_KEY:      // 如果输入的是退格符
            if(index > 0)        // 如果索引大于 0
            {
                index--;         // 索引减 1
                uart_putc('\b'); // 输出退格符
                uart_putc(' ');  // 输出空格覆盖原字符
                uart_putc('\b'); // 再输出退格符回到原位置
            }
            break;

        default:                                    // 其他字符
            if(index < buffer_size - 1)             // 如果索引小于缓冲区大小
            {
                input_buffer[index++] = input_char; // 将字符存入缓冲区并增加索引
                uart_putc(input_char);              // 输出接收到的字符
            }
            else
            {
                uart_puts("\nBuffer full!\n");
                index           = 0;    // 重置索引
                input_buffer[0] = '\0'; // 清空缓冲区
            }
            break;
        }
    } while(input_char != ENTER_KEY); // 循环直到输入换行符

    uart_putc('\n');
    spin_unlock(); // 释放锁
}
