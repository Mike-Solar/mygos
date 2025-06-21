
// memory.h

#pragma once

#include "types.h"


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


// 这些全局变量在 mem.S 中定义，表示程序的各个内存段的起始和结束地址
extern ptr_t TEXT_START;   // 程序代码段起始地址
extern ptr_t TEXT_END;     // 程序代码段结束地址
extern ptr_t DATA_START;   // 数据段起始地址
extern ptr_t DATA_END;     // 数据段结束地址
extern ptr_t RODATA_START; // 只读数据段起始地址
extern ptr_t RODATA_END;   // 只读数据段结束地址
extern ptr_t BSS_START;    // BSS 段起始地址
extern ptr_t BSS_END;      // BSS 段结束地址
extern ptr_t HEAP_START;   // 堆起始地址
extern ptr_t HEAP_SIZE;    // 堆大小


// 页面
struct Page
{
    uint8_t flags;
};

typedef struct Page* page_ptr; // 页面指针类型


#define PAGE_SIZE 4096               // 页面大小（字节）
#define PAGE_ORDER 12                // 页面大小的对数
#define PAGE_TAKEN (uint8_t)(1 << 0) // 页面是否使用
#define PAGE_LAST (uint8_t)(1 << 1)  // 是否是一个连续页面的最后一页


void  page_init();            // 初始化页面管理
void* page_alloc(int npages); // 分配一个内存块，由连续的物理页组成
void  page_free(void* p);     // 释放一个内存块，回收连续的物理页

void print_memory_info();     // 打印内存信息，包括堆起始地址、大小、保留页数量等
