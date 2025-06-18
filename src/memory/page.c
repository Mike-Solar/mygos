
// page.c

#include "os.h"

#include "memory.h"
#include "platform.h"
#include "utils.h"


static ptr_t    _alloc_start = 0; // 实际的堆起始地址
static ptr_t    _alloc_end   = 0; // 实际的堆结束地址
static uint32_t _num_pages   = 0; // 可分配的页面数量


// 把页面标记为未被使用
static inline void
_clear(page_ptr page)
{
    page->flags = 0;
}

// 页面是否被使用
static inline bool
_is_free(page_ptr page)
{
    if(page->flags & PAGE_TAKEN) return false;
    else return true;
}

// 设置标记
static inline void
_set_flag(page_ptr page, uint8_t flags)
{
    page->flags |= flags;
}

// 页面是否是连续的最后一页
static inline bool
_is_last(page_ptr page)
{
    if(page->flags & PAGE_LAST) return true;
    else return false;
}

// 将页面对齐（4KB）
static inline ptr_t
_align_page(ptr_t address)
{
    ptr_t order = (1 << PAGE_ORDER) - 1; // 00000000-00000000-00001111-11111111
    return (address + order) & (~order); // 先进位到下一个对齐边界，然后舍去低位
}

/*/
 *    ______________________________HEAP_SIZE_______________________________
 *   /   ___num_reserved_pages___   ______________num_pages______________   \
 *  /   /                        \ /                                     \   \
 *  |---|<--Page-->|<--Page-->|...|<--Page-->|<--Page-->|......|<--Page-->|---|
 *  A   A                         A                                       A   A
 *  |   |                         |                                       |   |
 *  |   |                         |                                       |   _memory_end
 *  |   |                         |                                       |
 *  |   _heap_start_aligned       _alloc_start                            _alloc_end
 *  HEAP_START(BSS_END)
 *
 *  Note: _alloc_end may equal to _memory_end.
/*/

/*/
 * 我们保留了一些 Pages 来保存 Page 结构
 * 保留页的数量取决于 LENGTH_RAM
 * 为简单起见，我们在这里保留的空间只是一个 approximation（近似值）
 * 假设它可以容纳最大 LENGTH_RAM
 * 我们假设 LENGTH_RAM 不应该太小
 * 理想情况下不小于 16M（即 PAGE_SIZE * PAGE_SIZE）
 * 这样可以容纳至少 256 个 Page 结构
 * 使用 HEAP_START 而不是 _heap_start_aligned
 * 作为分配 struct Page 的起始地址，
 * 因为我们不需要对齐 struct Page 的位置。
/*/

void
page_init()
{
    // 找到第一个对齐页
    ptr_t _heap_start_aligned = _align_page(HEAP_START);


    uint32_t num_reserved_pages = LENGTH_RAM / (PAGE_SIZE * PAGE_SIZE);

    _alloc_start = _heap_start_aligned + num_reserved_pages * PAGE_SIZE;
    _num_pages   = (HEAP_SIZE - (_heap_start_aligned - HEAP_START)) / PAGE_SIZE - num_reserved_pages;
    _alloc_end   = _alloc_start + (PAGE_SIZE * _num_pages);


    // 初始化保留页
    page_ptr page = (page_ptr)HEAP_START;
    for(int i = 0; i < _num_pages; page++, i++) _clear(page);

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

// 分配一个内存块，由连续的物理页组成
void*
page_alloc(int npages)
{
    // Note we are searching the page descriptor bitmaps.
    // 请注意，我们正在搜索页面索引。

    page_ptr page_i = (page_ptr)HEAP_START;                          // 从第一个索引开始
    for(int found, i = 0; i <= (_num_pages - npages); page_i++, i++) // 遍历
    {
        // 找到一个空闲页面
        if(_is_free(page_i))
        {
            /*/
             * meet a free page, continue to check if following
             * (npages - 1) pages are also unallocated.
            /*/

            found = 1;

            page_ptr page_j = page_i + 1;
            for(int j = i + 1; j < (i + npages); page_j++, j++)
            {
                if(!_is_free(page_j))
                {
                    found = 0;
                    break;
                }
            }

            /*/
             * get a memory block which is good enough for us,
             * take housekeeping, then return the actual start
             * address of the first page of this memory block
             *
             * 获取一个对我们来说足够好的内存块，
             * 进行标记，然后返回实际的
             * 此内存块的第一页的地址
            /*/

            if(found)
            {
                page_ptr page_k = page_i;
                for(int k = i; k < (i + npages); page_k++, k++) _set_flag(page_k, PAGE_TAKEN);

                _set_flag(--page_k, PAGE_LAST); // 标记最后一页

                return (void*)(_alloc_start + i * PAGE_SIZE);
            }
        }
    }
    return 0;
}


// 释放内存块
void
page_free(void* p)
{
    // Assert (TBD) if p is invalid
    // 如果 p 为空，或者超出范围
    if(!p || (ptr_t)p >= _alloc_end) return;

    // get the first page descriptor of this memory block
    // 获取目标页起始地址
    page_ptr page = (page_ptr)HEAP_START + ((ptr_t)p - _alloc_start) / PAGE_SIZE;

    // loop and clear all the page descriptors of the memory block
    // 循环清除标记
    while(!_is_last(page)) _clear(page++);
    _clear(page); // 清除最后一个
}
