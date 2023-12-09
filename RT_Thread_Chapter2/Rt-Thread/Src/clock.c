#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;/* 系统时基计数器，static提高了封装性，只有clock.c可以访问 */

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

void rt_tick_increase(void)
{
	rt_ubase_t i;
	
	struct rt_thread *thread;
	
	rt_tick++;
	
	for(i=0;i<RT_THREAD_PRIORITY_MAX;i++)
	{
		/* 通过链表的节点地址寻找到入口地址，然后再去找到remaining_tick的位置 */
		thread = rt_list_entry(	rt_thread_priority_table[i].next,
														struct rt_thread,
														tlist);
		if(thread->remaining_tick > 0)
		{
			thread->remaining_tick--;
		}
	}
	
	/* 系统调度 */
	rt_schedule();
}
