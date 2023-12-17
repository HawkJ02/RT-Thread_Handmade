/**
  ******************************************************************************
  * @file    main.c
  * @author  HawkJ
  * @version V1.0
  * @date    2023-12-17
  * @brief   RTT��Shell
  ******************************************************************************
  * @attention
  ******************************************************************************
  */ 
	
/*
*************************************************************************
*                             ������ͷ�ļ�
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"

#include "stm32f10x.h"
#include "bsp_led.h"

/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ�ָ�� */
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t led2_thread = RT_NULL;

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
static void led1_thread_entry(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
        
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		

				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
    }
}
static void led2_thread_entry(void* parameter)
{	
    while (1)
    {
        LED2_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
        
        LED2_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
				
				rt_kprintf("led2_thread_running,LED2 ON!\r\n");
    }
}

void Delay(__IO u32 nCount); 

/**
  * @brief  ������
  * @param  ��  
  * @retval ��
  */

/*
*************************************************************************
*                             main ����
*************************************************************************
*/
/**
  * @brief  ������
  * @param  ��
  * @retval ��
  */
int main(void)
{	
	 /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	led1_thread = rt_thread_create("led1",
																	led1_thread_entry,
																	RT_NULL,
																	512,
																	3,
																	20);
	if(led1_thread != RT_NULL)
	{
		rt_thread_startup(led1_thread);
	}
	else
	{
		return -1;
	}
	
	led2_thread = rt_thread_create("led2",
																	led2_thread_entry,
																	RT_NULL,
																	512,
																	2,
																	20);
	if(led2_thread != RT_NULL)
	{
		rt_thread_startup(led2_thread);
	}
	else
	{
		return -1;
	}
	
	
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
