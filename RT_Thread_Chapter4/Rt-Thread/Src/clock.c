#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;/* 系统时基计数器，static提高了封装性，只有clock.c可以访问 */
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];
extern rt_uint32_t rt_thread_ready_priority_group;

#if 0	
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

	for(i=0;i<RT_THREAD_PRIORITY_MAX;i++)
	{
		/* 通过链表的节点地址寻找到入口地址，然后再去找到remaining_tick的位置 */
		thread = rt_list_entry(	rt_thread_priority_table[i].next,
														struct rt_thread,
														tlist);
		if(thread->remaining_tick > 0)
		{
			thread->remaining_tick--;
			if(thread->remaining_tick == 0)
			{
				//我们通过就绪优先级列表来将线程挂起的，并没有删除线程，所以不需要插入
				//rt_schedule_insert_thread(thread);
        rt_thread_ready_priority_group |= thread->number_mask;
			}
		}
	}
	/* 系统调度 */
	rt_schedule();
}
#else
void rt_tick_increase(void)
{
	/* 系统时基计数器加1操作,rt_tick是一个全局变量 */
  ++ rt_tick;
    
	/* 支持时间片的一系列操作 */
	struct rt_thread *thread;
	/* 获取当前线程线程控制块 */
	thread = rt_thread_self();
	/* 时间片递减 */
	-- thread->remaining_tick;
	
	if(thread->remaining_tick == 0)
	{
		/* 重置时间片 */
		thread->remaining_tick = thread->init_tick;
		/* 让出处理器 */
		rt_thread_yield();
	}
		
  /* 扫描系统定时器列表 */
  rt_timer_check();
}
#endif
/**
 * 该函数用于返回操作系统启动到现在的当前 tick，tick是一个全局变量
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* return the global tick */
    return rt_tick;
}
