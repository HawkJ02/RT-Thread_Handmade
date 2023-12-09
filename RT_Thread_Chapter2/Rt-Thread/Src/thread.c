#include "rtthread.h"
#include "rthw.h"

extern struct rt_thread *rt_current_thread;

/*
void�����ʾ�����ķ��������� void������������������κ�ֵ��
(*entry)������ entry �Ǻ���ָ������ơ���Χ entry �� * ��������һ��ָ�롣���� (*entry) ��ʾһ��ָ������ָ�롣
(void *parameter)�����ʾ��������һ���������ò����������� void *����ָ���������͵�ָ�롣
void *���������ָ���������͵����ݡ�
*/
rt_err_t rt_thread_init(struct rt_thread *thread,
												const char *name,
												void (*entry)(void *parameter),
												void *parameter,
												void *stack_start,
												rt_uint32_t stack_size)
{
	/* �̶߳����ʼ�� */
	/* �߳̽ṹ�忪ͷ���ֵĳ�Ա����rt_object_t���� */
	rt_object_init((rt_object_t)thread, RT_Object_Class_Thread, name);
	
	/* ��ʼ���̵߳�����ڵ�	*/
	rt_list_init(&(thread->tlist));
	/* ���̵߳���ڱ��浽�߳̿��ƿ��entry��Ա�� */
	thread->entry = (void *)entry;
	/* ���̵߳�����βα��浽�߳̿��ƿ��parame��Ա�� */
	thread->parameter = parameter;
	/* ���̵߳�ջ��ʼ��ַ��ջ��С���浽�߳̿��ƿ�ĳ�Ա�� */
	thread->stack_addr = stack_start;
	thread->stack_size = stack_size;
	
	/* ��ʼ���߳�ջ�������ظ��̵߳�ջָ�룬��������������Ϊջ�Ǵ������²��Ͽ��ٵģ�����ע��˴�-4��������������������+4  */
	thread->sp = (void *)rt_hw_stack_init( thread->entry, 
																				 thread->parameter,
																				 (void *)((char *)thread->stack_addr + thread->stack_size -4));
	
	return RT_EOK;
	
}


/* ������ʱ */
void rt_thread_delay(rt_tick_t tick)
{
	struct rt_thread *thread;    
    
	/* ��ȡ��ǰ�̵߳��߳̿��ƿ� */    
	thread = rt_current_thread;
    
	/* ������ʱʱ�� */
	thread->remaining_tick = tick;
	
	/* ����ϵͳ���� */
	rt_schedule();
}
