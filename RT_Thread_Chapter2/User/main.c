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

/* �����߳̿��ƿ� */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;


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


/* �߳�1 */
void flag1_thread_entry( void *p_arg)
{
	for(;;)
	{
#if 0
		flag1 = 1;
		rt_delay(100);
		flag1 = 0;
		rt_delay(100);
		
		rt_schedule();
#else
		flag1 = 1;
		/* ��ʱ20ms */
		rt_thread_delay(20);				/* ��ʱ20ms */
		flag1 = 0;
		rt_thread_delay(20);				/* ��ʱ20ms */
#endif
	}
}

/* �߳�2 */
void flag2_thread_entry( void *p_arg)
{
	for(;;)
	{
#if 0
		flag2 = 1;
		rt_delay(100);
		flag2 = 0;
		rt_delay(100);
		
		rt_schedule();
#else
		flag2 = 1;
		rt_thread_delay(20);				/* ��ʱ20ms */
		flag2 = 0;
		rt_thread_delay(20);				/* ��ʱ20ms */
#endif
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
	
	/* ��������ʼ�� */
	rt_system_scheduler_init();

  /* ��ʼ�������߳� */    
  rt_thread_idle_init();	
	
	/* ��ʼ���߳� */
	rt_thread_init(	&rt_flag1_thread,
									"rt_flag1_thread",
									flag1_thread_entry,
									RT_NULL,
									&rt_flag1_thread_stack[0],
									sizeof(rt_flag1_thread_stack));
	/* ���̲߳��뵽�����б� */
	rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	
	/* ��ʼ���߳� */
	rt_thread_init(	&rt_flag2_thread,
									"rt_flag2_thread",
									flag2_thread_entry,
									RT_NULL,
									&rt_flag2_thread_stack[0],
									sizeof(rt_flag2_thread_stack));
	/* ���̲߳��뵽�����б� */
	rt_list_insert_before(&(rt_thread_priority_table[1]),&(rt_flag2_thread.tlist));
	
	/* ����ϵͳ������ */
	rt_system_scheduler_start(); 
	
	
}
