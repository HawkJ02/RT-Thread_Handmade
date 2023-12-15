#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/
#include "rtconfig.h"
#include "rtdef.h"
#include "rtthread.h"
#include "rthw.h"

/* RT-THREAD PRIVATE*/
ALIGN(RT_ALIGN_SIZE)


/*
*************************************************************************
*                              ȫ�ֱ���
*************************************************************************
*/
/* RT-THREAD PRIVATE VALUE*/
uint32_t flag1;
uint32_t flag2;
uint32_t flag3;
uint32_t flag4;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/*
*************************************************************************
*                      �߳̿��ƿ� & STACK & �߳�����
*************************************************************************
*/
/* RT-THREAD PRIVATE FUNCTION*/
/* �����߳�ջ*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];
rt_uint8_t rt_flag4_thread_stack[512];


/* �����߳̿��ƿ� */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;
struct rt_thread rt_flag3_thread;
struct rt_thread rt_flag4_thread;

/*
*************************************************************************
*                               ��������
*************************************************************************
*/
/* ��ʱ���� */
void rt_delay(uint32_t count)
{
	for(;count != 0;count--);
}


/* ������ʱ��Ƭ�Ķ��߳̽ṹ���������̴߳���һ�����ȼ����������
	 1��������е��̶߳���������ʱ���򲻻��������Ӱ�죬�����̶߳��������У���Ϊ��ǰ�߳̽������ߺ��Ӿ����������Ƴ�
	 2��������̳߳���ռ�ø��̣߳�����������Ӱ�죬�����ȼ����̲߳���Ӱ�죬����ͬ�Ȼ��ߵ����ȼ����̻߳ᱻ����*/
/* �߳�1 */
void flag1_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		/* ��ʱ20ms */
		rt_delay(100);
		//rt_thread_delay(40);				/* ��ʱ40ms */
		flag1 = 0;
		rt_delay(100);
		//rt_thread_delay(40);				/* ��ʱ40ms */

	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		rt_delay(100);
		//rt_thread_delay(20);				/* ��ʱ20ms */
		flag2 = 0;
		rt_delay(100);
		//rt_thread_delay(20);				/* ��ʱ20ms */

	}
}

/* �߳�3 */
void flag3_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag3 = 1;
		rt_thread_delay(30);				/* ��ʱ30ms */
		flag3 = 0;
		rt_thread_delay(30);				/* ��ʱ30ms */

	}
}

/* �߳�4 */
void flag4_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag4 = 1;
		rt_thread_delay(60);				/* ��ʱ60ms */
		flag4 = 0;
		rt_thread_delay(60);				/* ��ʱ60ms */

	}
}

/* �ض���ϵͳ�δ��жϴ����� */
void SysTick_Handler(void)
{
    /* �����ж� */
    rt_interrupt_enter();

    rt_tick_increase();

    /* �뿪�ж� */
    rt_interrupt_leave();
}


int main(void)
{
	/* Ӳ����ʼ�� */
	/* ��Ӳ����صĳ�ʼ�����������������������û����س�ʼ������ */
	
	/* ���жϣ��ؾͿ����ˣ�������Զ����� */
	rt_hw_interrupt_disable();
	
	/* 	TIPS����Ҫ����ϵͳ�ĺ���ʱ��SystemCoreClockƵ��Ϊ25MHz������ħ����Xtal��Ƶ��ҲΪ25Mhz��
			SysTick�ж�Ƶ������,����ζ�� SysTick ��ʱ���ļ�������� 250,000 ��ʼ������������ 0 ʱ����һ���жϡ�
			����ϵͳ����ʱ��Ƶ��Ϊ 25 MHz���������ú�SysTick ��ʱ������Լÿ 1/100 �루10 ���룩����һ���жϣ�����ÿ�� 100 �ε�ʱ�ӵδ�Ҫ��
			�����¼����Ϊ�ʼ��ֲ�����Ŀ�����ļ�system_stm32f1xx.c�����õ�ϵͳ����ʱ��Ƶ����16MHz������ʹ�÷�������ʱʱ��Ϊ12ms��������ʱ20ms�������ʱ12ms��
			����ԭ���ڷ���Ĺ����У�����ħ����target�����õķ���������ʱ�Ӿ���25Mhz��
			����������Ϊ16Mhz����ô����ǰ��������ֵ��Ȼ����ǰ����жϣ����±�����ʱ20ms�������ʱ12ms��
			��������һ�ַ����������Խ�ħ�����е�xTal����Ϊ16Mhz��Ȼ��SystemCoreClock��Ƶ��ҲΪ16Mhz���Ͳ�������ˡ�*/
	SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND);
	
	/* ϵͳ��ʱ���б��ʼ�� */
	rt_system_timer_init();
	
	/* ��������ʼ�� */
	rt_system_scheduler_init();

  /* ��ʼ�������߳� */    
  rt_thread_idle_init();	
	
	/* ��ʼ���߳�1 */
	rt_thread_init(	&rt_flag1_thread,
									"rt_flag1_thread",
									flag1_thread_entry,
									RT_NULL,
									&rt_flag1_thread_stack[0],
									sizeof(rt_flag1_thread_stack),
									4,
									1);
	/* ���̲߳��뵽�����б� */
	//rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	rt_thread_startup(&rt_flag1_thread);
									
	/* ��ʼ���߳�2 */
	rt_thread_init(	&rt_flag2_thread,
									"rt_flag2_thread",
									flag2_thread_entry,
									RT_NULL,
									&rt_flag2_thread_stack[0],
									sizeof(rt_flag2_thread_stack),
									4,
									3);
	rt_thread_startup(&rt_flag2_thread);
									
  /* ��ʼ���߳�3 */
	rt_thread_init(	&rt_flag3_thread,
									"rt_flag3_thread",
									flag3_thread_entry,
									RT_NULL,
									&rt_flag3_thread_stack[0],
									sizeof(rt_flag3_thread_stack),
									1,
									2);
	rt_thread_startup(&rt_flag3_thread);

	/* ��ʼ���߳�4 */
	rt_thread_init(	&rt_flag4_thread,
									"rt_flag4_thread",
									flag4_thread_entry,
									RT_NULL,
									&rt_flag4_thread_stack[0],
									sizeof(rt_flag4_thread_stack),
									2,
									2);
	rt_thread_startup(&rt_flag4_thread);			

									
	/* ����ϵͳ������ */
	rt_system_scheduler_start(); 
	
	
}
