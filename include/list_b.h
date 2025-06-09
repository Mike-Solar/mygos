/*
 * @Description: 移植Linux2.6内核list.h
 * @Version: V1.0
 * @Autor: https://blog.csdn.net/qq_16933601
 * @Date: 2020-09-12 22:54:51
 * @LastEditors: Carlos
 * @LastEditTime: 2020-09-16 00:35:17
 * 这是Linux内核源码的一部分，以GPL 2.0发布
 */
#ifndef _MYLIST_B_H
#define _MYLIST_B_H
 //原来链表删除后指向的位置，这里我们修改成 0
// # define POISON_POINTER_DELTA 0
// #define LIST_POISON1  ((void __attribute__((section(".boot.text"))) *) 0x00100100 + POISON_POINTER_DELTA)
// #define LIST_POISON2  ((void __attribute__((section(".boot.text"))) *) 0x00200200 + POISON_POINTER_DELTA)
#define NULL ((void __attribute__((section(".boot.text"))) *)0)
#define LIST_POISON1  NULL
#define LIST_POISON2  NULL

//计算member在type中的位置
#define offsetof(type, member)  (size_t)(&((type*)0)->member)
//根据member的地址获取type的起始地址
#define container_of(ptr, type, member) ({          \
        const typeof(((type *)0)->member)*__mptr = (ptr);    \
    (type *)((char *)__mptr - offsetof(type, member)); })

#define LIST_HEAD_INIT(name) { &(name), &(name) }

#define LIST_HEAD(name) \
	struct list_head name = LIST_HEAD_INIT(name)

static inline void __attribute__((section(".boot.text"))) INIT_LIST_HEAD_b(struct list_head *list)
{
	list->next = list;
	list->prev = list;
}

static inline void __attribute__((section(".boot.text"))) init_list_head_b(struct list_head *list)
{
    list->prev = list;
    list->next = list;
}

#ifndef CONFIG_DEBUG_LIST
static inline void __attribute__((section(".boot.text"))) __list_add_b(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next)
{
	next->prev = new;
	new->next = next;
	new->prev = prev;
	prev->next = new;
}
#else
extern void __attribute__((section(".boot.text"))) __list_add_b(struct list_head *new,
			      struct list_head *prev,
			      struct list_head *next);
#endif

//从头部添加
/**
 * list_add - add a new entry
 * @new: new entry to be added
 * @head: list head to add it after
 *
 * Insert a new entry after the specified head.
 * This is good for implementing stacks.
 */
#ifndef CONFIG_DEBUG_LIST
static inline void __attribute__((section(".boot.text"))) list_add_b(struct list_head *new, struct list_head *head)
{
	__list_add_b(new, head, head->next);
}
#else
extern void __attribute__((section(".boot.text"))) list_add_b(struct list_head *new, struct list_head *head);
#endif
//从尾部添加
static inline void __attribute__((section(".boot.text"))) list_add_tail_b(struct list_head *new, struct list_head *head)
{
    __list_add_b(new, head->prev, head);
}

static inline  void __attribute__((section(".boot.text"))) __list_del_b(struct list_head *prev, struct list_head *next)
{
    prev->next = next;
    next->prev = prev;
}

static void __attribute__((section(".boot.text"))) list_del_b(struct list_head *entry)
{
    __list_del_b(entry->prev, entry->next);
    entry->next = LIST_POISON1;
    entry->prev = LIST_POISON2;
}


static void __attribute__((section(".boot.text"))) __list_splice_b(struct list_head *list,
				 struct list_head *head)
{
	struct list_head *first = list->next;
	struct list_head *last = list->prev;
	struct list_head *at = head->next;

	first->prev = head;
	head->next = first;

	last->next = at;
	at->prev = last;
}
/**
 * list_empty - tests whether a list is empty
 * @head: the list to test.
 */
static inline int __attribute__((section(".boot.text"))) list_empty_b(const struct list_head *head)
{
	return head->next == head;
}
/**
 * list_splice - join two lists
 * @list: the new list to add.
 * @head: the place to add it in the first list.
 */
static inline void __attribute__((section(".boot.text"))) list_splice_b(struct list_head *list, struct list_head *head)
{
	if (!list_empty_b(list))
		__list_splice_b(list, head);
}
/**
 * list_replace - replace old entry by new one
 * @old : the element to be replaced
 * @new : the new element to insert
 *
 * If @old was empty, it will be overwritten.
 */
static inline void __attribute__((section(".boot.text"))) list_replace_b(struct list_head *old,
				struct list_head *new)
{
	new->next = old->next;
	new->next->prev = new;
	new->prev = old->prev;
	new->prev->next = new;
}

static inline void __attribute__((section(".boot.text"))) list_replace_init_b(struct list_head *old,
					struct list_head *new)
{
	list_replace_b(old, new);
	INIT_LIST_HEAD_b(old);
}
/**
 * list_move - delete from one list and add as another's head
 * @list: the entry to move
 * @head: the head that will precede our entry
 */
static inline void __attribute__((section(".boot.text"))) list_move_b(struct list_head *list, struct list_head *head)
{
	__list_del_b(list->prev, list->next);
	list_add_b(list, head);
}

/**
 * list_move_tail - delete from one list and add as another's tail
 * @list: the entry to move
 * @head: the head that will follow our entry
 */
static inline void __attribute__((section(".boot.text"))) list_move_tail_b(struct list_head *list,
				  struct list_head *head)
{
	__list_del_b(list->prev, list->next);
	list_add_tail_b(list, head);
}
#endif