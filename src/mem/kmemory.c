//
// Created by katherinesolar on 25-5-23.
//

#include "kmemory.h"

#include "device_tree_parser.h"
uint64_t heap_size=0;
uint64_t heap_start=0;
struct heap_block *heap_head;
struct heap_block *heap_used;
struct heap_block *heap_free;
struct heap_block *heap_used_last;
extern uint64_t __heap_end;
extern uint64_t __heap_start;
//初始化堆内存
void init_heap() {
	heap_start=__heap_start;
	heap_size=1024*1024*1024;
	heap_head=(struct heap_block *)memory.base;
	heap_head->size=heap_size;
	heap_head->next=NULL;
	heap_head->prev=NULL;
	heap_head->is_free=TRUE;
	heap_head->data=heap_head+sizeof(struct heap_block);
	heap_free=(struct heap_block *)memory.base;
	heap_used=NULL;
	heap_used_last=NULL;
}
//内部函数，无优化版
void *__kmalloc(uint64_t size) {
	if (size > heap_size) {
		return NULL;
	}
	if (size == 0) {
		return NULL;
	}
	struct heap_block *block=heap_free;
	while (block!=NULL) {
		if (block->size >= size) {
			break;
		}
	}
	if (block==NULL) {
		return NULL;
	}
	// 发现空间正好的一块
	if (block->size == size) {
		// 从空闲链表取出该块
		block->prev->next=block->next;
		block->next->prev=block->prev;
		// 如果是第一块
		if (heap_used == NULL) {
			block->prev=NULL;
			block->next=NULL;
			heap_used=block;
		}
		if (heap_used_last==NULL) {
			heap_used_last=block;
		}
		else {
			// 如果不是第一块，把这一块放进已用链表
			heap_used_last->next=block;
			block->prev=heap_used_last;
			block->next=NULL;
			heap_used_last=block;
		}
		block->is_free=FALSE;
		return block->data;
	}
	//找到比它大的一块
	struct heap_block *next=block->next;                      //下一块
	struct heap_block *current=(void *)block+size;            //分配完空间后剩下的一块
	//剩下一块的大小
	current->size=block->size-size-sizeof(struct heap_block);
	//构造剩下一块的头部，加入空闲链表
	current->next=next;
	next->prev=current;
	current->prev=block->prev;
	block->prev->next=current;
	current->is_free=TRUE;
	current->data=(void *)current+sizeof(struct heap_block);
	//加入已用链表
	block->is_free=FALSE;
	block->size=size;
	block->next=NULL;
	block->prev=heap_used_last;
	heap_used_last->next=block;
	heap_used_last=block;
	return block->data;
}

void __kfree(void *ptr) {
	ptr-=sizeof(struct heap_block);
	struct heap_block *block=(struct heap_block *)ptr;
	if (block->is_free) {
		return;
	}
	block->is_free=TRUE;
	block->prev->next=block->next;
	if (block->next!=NULL) {
		block->next->prev=block->prev;
	}
	struct heap_block *cur=heap_free;
	while (cur->next!=NULL) {
		if (cur<block && cur->next>block) {
			break;
		}
	}
	if (cur->next==NULL) {
		cur->next=block;
		block->prev=cur;
	}else {
		cur->next->prev=block;
		block->next=cur->next;
		cur->next=block;
		block->prev=cur;
	}
}

void *kmalloc(uint64_t size) {
	return __kmalloc(size);
}
void kfree(void *ptr) {
	__kfree(ptr);
}
