
// page.c

#include "memory.h"

#include "platform.h"


ptr_t    _alloc_start        = 0; // 实际的堆起始地址
ptr_t    _alloc_end          = 0; // 实际的堆结束地址
uint32_t _num_pages          = 0; // 可分配的页面数量
ptr_t    _heap_start_aligned = 0; // 对齐后的堆起始地址
uint32_t num_reserved_pages  = 0; // 保留的页面数量


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

// 初始化页面管理
void
page_init()
{
    // 找到第一个对齐页
    _heap_start_aligned = _align_page(HEAP_START);

    num_reserved_pages = LENGTH_RAM / (PAGE_SIZE * PAGE_SIZE);

    _alloc_start = _heap_start_aligned + num_reserved_pages * PAGE_SIZE;
    _num_pages   = (HEAP_SIZE - (_heap_start_aligned - HEAP_START)) / PAGE_SIZE - num_reserved_pages;
    _alloc_end   = _alloc_start + (PAGE_SIZE * _num_pages);

    // 初始化保留页
    page_ptr page = (page_ptr)HEAP_START;
    for(int i = 0; i < _num_pages; page++, i++) _clear(page);

    print_memory_info();
}

// 分配一个内存块，由连续的物理页组成
void*
page_alloc(int npages)
{
    page_ptr page_i = (page_ptr)HEAP_START;                          // 从第一个索引开始
    for(int found, i = 0; i <= (_num_pages - npages); page_i++, i++) // 遍历
    {
        if(_is_free(page_i))
        {
            // 找到一个空闲页面，继续检查后续的 (npages - 1) 个页面是否也未分配
            found = 1;

            page_ptr page_j = page_i + 1;
            for(int j = i + 1; j < (i + npages); page_j++, j++)
            {
                // 如果连续的空闲页面中有一个被占用
                if(!_is_free(page_j))
                {
                    found = 0;
                    break;
                }
            }

            // 获取一个足够长的内存块，进行标记，然后返回实际的此内存块的第一页的地址
            if(found != 0)
            {
                page_ptr page_k = page_i;

                for(int k = i; k < (i + npages); page_k++, k++) _set_flag(page_k, PAGE_TAKEN);
                _set_flag(--page_k, PAGE_LAST); // 标记最后一页

                return (void*)(_alloc_start + i * PAGE_SIZE);
            }
        }
    }
    return nullptr; // 如果没有找到足够的连续页面，则返回空指针
}

// 释放内存块
void
page_free(void* p)
{
    // 如果 p 为空，或者超出范围
    if(!p || (ptr_t)p >= _alloc_end) return;

    // 获取目标页起始地址
    page_ptr page = (page_ptr)HEAP_START + ((ptr_t)p - _alloc_start) / PAGE_SIZE;

    // 循环清除标记
    while(!_is_last(page)) _clear(page++);
    _clear(page);
}
