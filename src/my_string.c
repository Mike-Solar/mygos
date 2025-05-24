
// string.c


#include "string.h"

void
kstrcpy(char* dest, const char* src)
{
    // 循环复制字符，直到遇到字符串结束符'\0'
    while((*dest++ = *src++));
}

void
kstrcat(char* dest, const char* src)
{
    // 找到目标字符串的末尾
    while(*dest) dest++;

    // 连接源字符串到目标字符串
    while((*dest++ = *src++));
}

int
kstrcmp(const char* str1, const char* str2)
{
    // 循环比较字符，直到遇到字符串结束符'\0'
    while(*str1 && (*str1 == *str2))
    {
        str1++;
        str2++;
    }

    // 返回两个字符串的差值
    return *(unsigned char*)str1 - *(unsigned char*)str2;
}

int
kstrlen(const char* str)
{
    int len = 0;

    // 循环计算字符串长度
    while(*str++) len++;

    return len;
}
