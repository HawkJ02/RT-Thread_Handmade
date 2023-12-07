#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"

/* RT-THREAD INCLUDE*/
#include "rtconfig.h"
#include "rtdef.h"
#include "rtthread.h"


/* RT-THREAD PRIVATE*/
ALIGN(RT_ALIGN_SIZE)

/* RT-THREAD PRIVATE VALUE*/
uint32_t flag1;
uint32_t flag2;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/* RT-THREAD PRIVATE FUNCTION*/
/* �����߳�ջ*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];

/* �����߳̿��ƿ� */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;

/* ��ʱ���� */
void rt_delay(uint32_t count)
{
	for(;count != 0;count--);
}

/* �߳�1 */
void flag1_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		rt_delay(100);
		flag1 = 0;
		rt_delay(100);
		
		rt_schedule();
	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		rt_delay(100);
		flag2 = 0;
		rt_delay(100);
		
		rt_schedule();
	}
}



int main(void)
{
		/* ��������ʼ�� */
		rt_system_scheduler_init();
	
		/* �߳�1��2��ʼ�� */
		rt_thread_init( &rt_flag1_thread,
										flag1_thread_entry,
										RT_NULL,
										&rt_flag1_thread_stack[0],
										sizeof(rt_flag1_thread_stack));
		/* ���̲߳�������б� */
		rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
		
		rt_thread_init( &rt_flag2_thread,
											flag2_thread_entry,
											RT_NULL,
											&rt_flag2_thread_stack[0],
											sizeof(rt_flag2_thread_stack));
		/* ���̲߳�������б� */
		rt_list_insert_before(&(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist));
	
		/* ����ϵͳ������ */
		rt_system_scheduler_start();
	

}
