
// memory/print_info.c

#include "memory.h"

#include "utils.h"


extern ptr_t    _alloc_start;        // 实际的堆起始地址
extern ptr_t    _alloc_end;          // 实际的堆结束地址
extern uint32_t _num_pages;          // 可分配的页面数量
extern ptr_t    _heap_start_aligned; // 对齐后的堆起始地址
extern uint32_t num_reserved_pages;  // 保留的页面数量


void
print_memory_info()
{
    set_print_style(COLOR_BLUE, PRINT_STYLE_BOLD);
    printf("======================== Page management initialized ========================\n");

    set_print_style(COLOR_MAGENTA, PRINT_STYLE_NORMAL);
    printf("HEAP_START = %p(aligned to %p)\n"
           "HEAP_SIZE = 0x%lx\n"
           "num of reserved pages = %d\n"
           "num of pages to be allocated for heap = %d\n\n",
           HEAP_START, _heap_start_aligned, HEAP_SIZE, num_reserved_pages, _num_pages);
    printf("TEXT:   %p -> %p\n", TEXT_START, TEXT_END);
    printf("RODATA: %p -> %p\n", RODATA_START, RODATA_END);
    printf("DATA:   %p -> %p\n", DATA_START, DATA_END);
    printf("BSS:    %p -> %p\n", BSS_START, BSS_END);
    printf("HEAP:   %p -> %p\n", _alloc_start, _alloc_end);

    set_print_style(COLOR_BLUE, PRINT_STYLE_BOLD);
    printf("==============================================================================\n");

    reset_print_style();
}
