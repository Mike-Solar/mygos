
// page.c

#include "os.h"


// Following global vars are defined in mem.S
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

/*/
 * _alloc_start points to the actual(实际的) start address of heap pool
 * _alloc_end points to the actual end address of heap pool
 * _num_pages holds the actual max number of pages we can allocate.
/*/

static ptr_t    _alloc_start = 0;
static ptr_t    _alloc_end   = 0;
static uint32_t _num_pages   = 0;

#define PAGE_SIZE 4096               // 页面大小（字节）
#define PAGE_ORDER 12                // 页面大小的对数
#define PAGE_TAKEN (uint8_t)(1 << 0) // 页面是否使用
#define PAGE_LAST (uint8_t)(1 << 1)  // 是否是一个连续页面的最后一页

/*/
 * Page Descriptor
 * flags:
 * - bit 0: flag if this page is taken(allocated)
 * - bit 1: flag if this page is the last page of the memory block allocated
/*/

struct Page
{
    uint8_t flags;
};

// 把页面标记为未被使用
static inline void
_clear(struct Page* page)
{
    page->flags = 0;
}

// 页面是否被使用
static inline int
_is_free(struct Page* page)
{
    if(page->flags & PAGE_TAKEN) return 0;
    else return 1;
}

// 设置标记
static inline void
_set_flag(struct Page* page, uint8_t flags)
{
    page->flags |= flags;
}

// 页面是否是连续的最后一页
static inline int
_is_last(struct Page* page)
{
    if(page->flags & PAGE_LAST) return 1;
    else return 0;
}

// align the address to the border of page(4K)
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

void
page_init()
{
    // 找到第一个对齐页
    ptr_t _heap_start_aligned = _align_page(HEAP_START);

    /*/
     * We reserved some Pages to hold the Page structures.
     * The number of reserved pages depends on the LENGTH_RAM.
     * For simplicity, the space we reserve here is just an approximation(近似值),
     * assuming that it can accommodate the maximum LENGTH_RAM.
     * We assume LENGTH_RAM should not be too small, ideally no less
     * than 16M (i.e. PAGE_SIZE * PAGE_SIZE).
     *
     * 我们保留了一些 Pages 来保存 Page 结构。
     * 保留页的数量取决于 LENGTH_RAM。
     * 为简单起见，我们在这里保留的空间只是一个 approximation（近似值），
     * 假设它可以容纳最大 LENGTH_RAM。
     * 我们假设 LENGTH_RAM 不应该太小，理想情况下
     * 不小于 16M（即 PAGE_SIZE * PAGE_SIZE）。
    /*/

    uint32_t num_reserved_pages = LENGTH_RAM / (PAGE_SIZE * PAGE_SIZE);

    _alloc_start = _heap_start_aligned + num_reserved_pages * PAGE_SIZE;
    _num_pages   = (HEAP_SIZE - (_heap_start_aligned - HEAP_START)) / PAGE_SIZE - num_reserved_pages;
    _alloc_end   = _alloc_start + (PAGE_SIZE * _num_pages);


    /*/
     * We use HEAP_START, not _heap_start_aligned as begin address for
     * allocating struct Page, because we have no requirement of alignment
     * for position of struct Page.
     *
     * 我们使用 HEAP_START 而不是 _heap_start_aligned
     * 作为分配 struct Page 的起始地址，
     * 因为我们不需要对齐 struct Page 的位置。
    /*/

    // 初始化保留页
    struct Page* page = (struct Page*)HEAP_START;
    for(int i = 0; i < _num_pages; page++, i++) _clear(page);

    printf("HEAP_START = %p(aligned to %p), HEAP_SIZE = 0x%lx,\n"
           "num of reserved pages = %d, num of pages to be allocated for heap = %d\n",
           HEAP_START, _heap_start_aligned, HEAP_SIZE, num_reserved_pages, _num_pages);
    printf("TEXT:   %p -> %p\n", TEXT_START, TEXT_END);
    printf("RODATA: %p -> %p\n", RODATA_START, RODATA_END);
    printf("DATA:   %p -> %p\n", DATA_START, DATA_END);
    printf("BSS:    %p -> %p\n", BSS_START, BSS_END);
    printf("HEAP:   %p -> %p\n", _alloc_start, _alloc_end);
}

/*/
 * Allocate a memory block which is composed of contiguous physical pages
 * - npages: the number of PAGE_SIZE pages to allocate
 *
 * 分配一个由连续物理页组成的内存块
 * - npages：要分配的 PAGE_SIZE 页数
/*/
void*
page_alloc(int npages)
{
    // Note we are searching the page descriptor bitmaps.
    // 请注意，我们正在搜索页面索引。

    struct Page* page_i = (struct Page*)HEAP_START;                  // 从第一个索引开始
    for(int found, i = 0; i <= (_num_pages - npages); page_i++, i++) // 遍历
    {
        // 找到一个
        if(_is_free(page_i))
        {
            /*/
             * meet a free page, continue to check if following
             * (npages - 1) pages are also unallocated.
            /*/

            found = 1;

            struct Page* page_j = page_i + 1;
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
                struct Page* page_k = page_i;
                for(int k = i; k < (i + npages); page_k++, k++) _set_flag(page_k, PAGE_TAKEN);

                _set_flag(--page_k, PAGE_LAST); // 标记最后一页

                return (void*)(_alloc_start + i * PAGE_SIZE);
            }
        }
    }
    return 0;
}

/*/
 * Free the memory block
 * - p: start address of the memory block
/*/
void
page_free(void* p)
{
    // Assert (TBD) if p is invalid
    // 如果 p 为空，或者超出范围
    if(!p || (ptr_t)p >= _alloc_end) return;

    // get the first page descriptor of this memory block
    // 获取目标页起始地址
    struct Page* page = (struct Page*)HEAP_START + ((ptr_t)p - _alloc_start) / PAGE_SIZE;

    // loop and clear all the page descriptors of the memory block
    // 循环清除标记
    while(!_is_last(page)) _clear(page++);
    _clear(page); // 清除最后一个
}

void
page_test()
{
    printf("\npage_test:\n");

    void* p1 = page_alloc(2);
    printf("p1 = %p\n", p1);
    page_free(p1);

    void* p2 = page_alloc(7);
    printf("p2 = %p\n", p2);

    void* p3 = page_alloc(4);
    printf("p3 = %p\n", p3);
    page_free(p3);

    page_free(p2);

    printf("\n");
}
