
// tasks/input.c

#include "mygos.h"


void
test_input()
{
    static char input_buffer[64] = { 0 }; // 输入缓冲区

    printf("=== Input test started!\n");
    printf("Please enter some text (max 63 characters): ");

    get_input(input_buffer, sizeof(input_buffer)); // 获取用户输入

    printf("You entered: %s\n", input_buffer);     // 输出输入的内容

    printf("=== Input test completed!\n");
}


void
test_input_print()
{
    spin_lock(); // 获取锁

    uart_puts("Please enter a character:\n");

    while(1)
    {
        char input_char = 0;

        input_char = uart_getc(); // 阻塞等待接收字符

        printf("You entered: '%c', ASCII: %d\n", input_char, input_char);
    }

    spin_unlock(); // 释放锁
}