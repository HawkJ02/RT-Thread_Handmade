#include <rtthread.h>
#include <rthw.h>

static rt_tick_t rt_tick = 0;/* ϵͳʱ����������static����˷�װ�ԣ�ֻ��clock.c���Է��� */
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
		/* ͨ������Ľڵ��ַѰ�ҵ���ڵ�ַ��Ȼ����ȥ�ҵ�remaining_tick��λ�� */
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
		/* ͨ������Ľڵ��ַѰ�ҵ���ڵ�ַ��Ȼ����ȥ�ҵ�remaining_tick��λ�� */
		thread = rt_list_entry(	rt_thread_priority_table[i].next,
														struct rt_thread,
														tlist);
		if(thread->remaining_tick > 0)
		{
			thread->remaining_tick--;
			if(thread->remaining_tick == 0)
			{
				//����ͨ���������ȼ��б������̹߳���ģ���û��ɾ���̣߳����Բ���Ҫ����
				//rt_schedule_insert_thread(thread);
        rt_thread_ready_priority_group |= thread->number_mask;
			}
		}
	}
	/* ϵͳ���� */
	rt_schedule();
}
#else
void rt_tick_increase(void)
{
	/* ϵͳʱ����������1����,rt_tick��һ��ȫ�ֱ��� */
  ++ rt_tick;
    
	/* ֧��ʱ��Ƭ��һϵ�в��� */
	struct rt_thread *thread;
	/* ��ȡ��ǰ�߳��߳̿��ƿ� */
	thread = rt_thread_self();
	/* ʱ��Ƭ�ݼ� */
	-- thread->remaining_tick;
	
	if(thread->remaining_tick == 0)
	{
		/* ����ʱ��Ƭ */
		thread->remaining_tick = thread->init_tick;
		/* �ó������� */
		rt_thread_yield();
	}
		
  /* ɨ��ϵͳ��ʱ���б� */
  rt_timer_check();
}
#endif
/**
 * �ú������ڷ��ز���ϵͳ���������ڵĵ�ǰ tick��tick��һ��ȫ�ֱ���
 * @return current tick
 */
rt_tick_t rt_tick_get(void)
{
    /* return the global tick */
    return rt_tick;
}
