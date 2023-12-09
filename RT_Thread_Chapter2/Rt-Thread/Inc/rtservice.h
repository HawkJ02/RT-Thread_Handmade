#ifndef __RT_SERVICE_H__
#define __RT_SERVICE_H__

#include "rtdef.h"
#include "rtconfig.h"
#include "rtthread.h"

/* 已知一个结构体中成员的地址，反推出该结构体的首地址，就是用（member的实际地址）减去（假设该结构体的首地址是0，
	 再指向member的虚拟地址，也就是member相对于首地址的偏移量） */
#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
			
#define rt_list_entry(node, type, member) \
    rt_container_of(node, type, member)
		
		

/* 初始化一个指向自身的双向链表节点 */
rt_inline void rt_list_init(rt_list_t *l)
{
	l->next = l->prev = l;
}

/* 在双向链表的表头后面插入一个节点，将n插入到l后面 */
rt_inline void rt_list_insert_after(rt_list_t *l,rt_list_t *n)
{
	/* 首先让原始状态节点2的前指针指向新节点n */
	l->next->prev = n;
	/* 接着让新节点n的后指针指向原始状态节点2 */
	n->next = l->next;
	/* 然后让节点1和新节点n相互指向 */
	l->next = n;
	n->prev = l;
}

/* 在双向链表的表头前面插入一个节点 */
rt_inline void rt_list_insert_before(rt_list_t *l,rt_list_t *n)
{
	l->prev->next = n;
	n->prev = l->prev;
	
	l->prev = n;
	n->next = l;
}

/* 在双向链表中删除一个节点 */
rt_inline void rt_list_remove(rt_list_t *n)
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
	
	n->next = n->prev = n;
}




#endif
