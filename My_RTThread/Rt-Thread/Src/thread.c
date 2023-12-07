#include "rtthread.h"
#include "rthw.h"


/*
void：这表示函数的返回类型是 void，即这个函数不返回任何值。
(*entry)：这里 entry 是函数指针的名称。包围 entry 的 * 表明这是一个指针。整体 (*entry) 表示一个指向函数的指针。
(void *parameter)：这表示函数接受一个参数，该参数的类型是 void *，即指向任意类型的指针。
void *：代表可以指向任意类型的数据。
*/
rt_err_t rt_thread_init(struct rt_thread *thread,
												void (*entry)(void *parameter),
												void *parameter,
												void *stack_start,
												rt_uint32_t stack_size)
{
	/* 初始化线程的链表节点	*/
	rt_list_init(&(thread->tlist));
	/* 将线程的入口保存到线程控制块的entry成员中 */
	thread->entry = (void *)entry;
	/* 将线程的入口形参保存到线程控制块的parame成员中 */
	thread->parameter = parameter;
	/* 将线程的栈起始地址和栈大小保存到线程控制块的成员中 */
	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/* 初始化线程栈，并返回该线程的栈指针，第三个参数是因为栈是从上往下不断开辟的，并且注意此处-4，后面参数传进函数后会+4  */
	thread->sp = (void *)rt_hw_stack_init( thread->entry, 
																				 thread->parameter,
																				 (void *)((char *)thread->stack_addr + thread->stack_size -4));
	
	return RT_EOK;
	
}
