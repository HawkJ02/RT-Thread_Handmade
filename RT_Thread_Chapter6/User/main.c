/**
  ******************************************************************************
  * @file    main.c
  * @author  HawkJ
  * @version V1.0
  * @date    2023-12-17
  * @brief   RTT的Shell
  ******************************************************************************
  * @attention
  ******************************************************************************
  */ 
	
/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/ 
#include "board.h"
#include "rtthread.h"

#include "stm32f10x.h"
#include "bsp_led.h"

/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块指针 */
static rt_thread_t led1_thread = RT_NULL;
static rt_thread_t led2_thread = RT_NULL;

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
static void led1_thread_entry(void* parameter)
{	
    while (1)
    {
        LED1_ON;
        rt_thread_delay(500);   /* 延时500个tick */
        
        LED1_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		

				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
    }
}
static void led2_thread_entry(void* parameter)
{	
    while (1)
    {
        LED2_ON;
        rt_thread_delay(500);   /* 延时500个tick */
        
        LED2_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
				
				rt_kprintf("led2_thread_running,LED2 ON!\r\n");
    }
}

void Delay(__IO u32 nCount); 

/**
  * @brief  主函数
  * @param  无  
  * @retval 无
  */

/*
*************************************************************************
*                             main 函数
*************************************************************************
*/
/**
  * @brief  主函数
  * @param  无
  * @retval 无
  */
int main(void)
{	
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
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

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
