
// utils.h

#ifndef __UTILS_H__
#define __UTILS_H__

#include "mygos.h"


#define DELAY 1000

static inline void
print_create_task(reg_t id, reg_t pc, const char* name)
{
    printf("====== Task Created: %p (%s) ======\n", id, name);
    printf("PC = 0x%08x\n", pc);
}

static inline void
print_delete_task(reg_t id, const char* name)
{
    printf("====== Task Deleted: %p (%s) ======\n", id, name);
}


enum colors
{
    COLOR_RED     = 31,
    COLOR_GREEN   = 32,
    COLOR_YELLOW  = 33,
    COLOR_BLUE    = 34,
    COLOR_MAGENTA = 35,
    COLOR_CYAN    = 36,
    COLOR_WHITE   = 37
};

enum print_style
{
    PRINT_STYLE_NORMAL     = 0,  // 普通样式
    PRINT_STYLE_BOLD       = 1,  // 粗体样式
    PRINT_STYLE_ITALIC     = 2,  // 斜体样式（部分终端不支持）
    PRINT_STYLE_BLINK      = 3,  // 闪烁样式（部分终端不支持）
    PRINT_STYLE_UNDERLINE  = 4,  // 下划线样式
    PRINT_STYLE_FG_BLACK   = 30, // 黑色前景色
    PRINT_STYLE_FG_RED     = 31, // 红色前景色
    PRINT_STYLE_FG_GREEN   = 32, // 绿色前景色
    PRINT_STYLE_FG_YELLOW  = 33, // 黄色前景色
    PRINT_STYLE_FG_BLUE    = 34, // 蓝色前景色
    PRINT_STYLE_FG_MAGENTA = 35, // 品红色前景色
    PRINT_STYLE_FG_CYAN    = 36, // 青色前景色
    PRINT_STYLE_FG_WHITE   = 37, // 白色前景色
};

// 设置打印颜色和样式
static inline void
set_print_style(enum colors color, enum print_style style)
{
    printf("\033[%d;%dm", style, color);
}

// 重置打印颜色和样式
static inline void
reset_print_style()
{
    printf("\033[0m");
}


#endif // __UTILS_H__
