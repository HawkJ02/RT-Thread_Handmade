/**
  ******************************************************************************
  * @file    main.c
  * @author  HawkJ
  * @version V1.0
  * @date    2023-12-16
  * @brief   移植RT_Thread
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
#include "bsp_key.h"


/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义全局变量 */
uint8_t ucValue[2] = {0x00, 0x00};

/* 定义事件掩码的位 */
#define KEY0_EVENT 	(0x01 << 0)
#define KEY1_EVENT	(0x01 << 1)



/* 定义线程控制块指针 */
static rt_thread_t stu_a_thread = RT_NULL;
static rt_thread_t tch_b_thread = RT_NULL;
static rt_thread_t send_evt_thread = RT_NULL;
static rt_thread_t recv_evt_thread = RT_NULL;


/* 定义互斥量控制块指针 */
static rt_mutex_t drill_mux = RT_NULL;	/* 互斥量 */

/* 定义事件控制块指针 */
static rt_event_t apple_evt = RT_NULL;

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/

static void stu_a_thread_entry(void* parameter)
{		
		rt_err_t uwRet = RT_EOK;
    while (1)
    {
			uwRet = rt_mutex_take(drill_mux,RT_WAITING_FOREVER);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("学生A已取用电钻！\n");
			}
			else
			{
				rt_kprintf("电钻正在使用中！\n");
			}
		
			++ ucValue[0];
			rt_thread_delay(100);
			++ ucValue[1];
			uwRet = rt_mutex_release(drill_mux);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("电钻已归还！\n");
			}
			else
			{
				rt_kprintf("电钻归还失败！\n");
			}
			
			rt_thread_yield();
    }
}


static void tch_b_thread_entry(void* parameter)
{		
		
    while (1)
    {
			rt_mutex_take(drill_mux,RT_WAITING_FOREVER);
			
			if(ucValue[0] == ucValue[1])
			{
				rt_kprintf("互斥信号量已同步！\n");
			}
			else
			{
				rt_kprintf("互斥信号量同步失败！\n");
			}
			rt_mutex_release(drill_mux);
			rt_thread_delay(1000);
    }
}


static void send_evt_thread_entry(void* parameter)
{		
		uint8_t key;
    while (1)
    {
			key = key_scan(0);
			
			switch(key)
			{
				case KEY0_PRES:
					rt_event_send(apple_evt,KEY0_EVENT);
					rt_kprintf("KEY0按下！\n");
					break;
				
				case KEY1_PRES:
					rt_event_send(apple_evt,KEY1_EVENT);
					rt_kprintf("KEY1按下！\n");
					break;
			}
			rt_thread_delay(20);
    }
}

static void recv_evt_thread_entry(void* parameter)
{
	/* 定义一个局部变量，用于保存事件的掩码值 */
	rt_uint32_t recved;
	while(1)
	{
		rt_event_recv(apple_evt,																	/* 事件对象 */
									KEY0_EVENT | KEY1_EVENT,										/* 接收线程所感兴趣的事件 */
									RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,		/* 事件接受的模式：与模式——感兴趣的事件都必须发生；
																																 清除模式，事件发生后清除，多次使用。
																																 如果是启动自检（使用一次）的例子中就不需要清除 */
									RT_WAITING_FOREVER,													/* 一直等待 */
									&recved);																		/* 存储事件掩码值的局部变量，用于检验 */
		
		/* 再次确认此时的事件的掩码值是否是正确的 */
		if((recved == KEY0_EVENT)  || (recved == KEY1_EVENT))
		{
			rt_kprintf("或模式！KEY0或KEY1按下，获取事件成功!\n");
			LED1_TOGGLE;
		}
		
		if(recved == (KEY0_EVENT | KEY1_EVENT))
		{
			rt_kprintf("与模式！KEY0和KEY1都已经按下过，获取事件成功!\n");
			LED1_TOGGLE;
		}
		
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
	/* 输出版本信息 */
	rt_show_version();
	rt_kprintf("这是RTT互斥量和事件实验！\n");
	rt_kprintf("同步成功则输出“同步成功！”\n");
	LED1_ON;
	LED2_ON;
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	/* 创建互斥信号量 */
	drill_mux = rt_mutex_create("drill_mux",RT_IPC_FLAG_FIFO);
	if(drill_mux != RT_NULL)
	{
		rt_kprintf("互斥信号量创建成功！\n");
	}
	else
	{
		rt_kprintf("互斥信号量创建失败！\n");
	}
	
	/* 创建事件，因为时间到来的时候是将挂起的线程一并通知，所以按常理来说是按照优先级顺序 */
	apple_evt = rt_event_create("apple_evt",RT_IPC_FLAG_PRIO);
	if(apple_evt != RT_NULL)
	{
		rt_kprintf("事件创建成功！\n");
	}
	else
	{
		rt_kprintf("事件创建失败！\n");
	}
	
//	
//	/* 创建互斥信号量低优先级占用线程A线程 */
//	stu_a_thread = rt_thread_create("stu_a_thread",
//																	stu_a_thread_entry,
//																	RT_NULL,
//																	512,
//																	6,
//																	20);
//	if(stu_a_thread != RT_NULL)
//	{
//		rt_thread_startup(stu_a_thread);
//	}
//	else
//	{
//		return -1;
//	}
//	
//	/* 创建互斥信号量高优先级等待线程B线程 */
//	tch_b_thread = rt_thread_create("tch_b_thread",
//																	tch_b_thread_entry,
//																	RT_NULL,
//																	512,
//																	1,
//																	20);
//	if(tch_b_thread != RT_NULL)
//	{
//		rt_thread_startup(tch_b_thread);
//	}
//	else
//	{
//		return -1;
//	}
	
	/* 创建按键触发事件发送线程 */
	send_evt_thread = rt_thread_create("send_evt_thread",
																	send_evt_thread_entry,
																	RT_NULL,
																	512,
																	1,
																	20);
	if(send_evt_thread != RT_NULL)
	{
		rt_thread_startup(send_evt_thread);
	}
	else
	{
		return -1;
	}
	
	/* 创建事件等待线程，可以是一个需要重重初始化之后然后启动的重要设备 */
	recv_evt_thread = rt_thread_create("recv_evt_thread",
																	recv_evt_thread_entry,
																	RT_NULL,
																	512,
																	1,
																	20);
	if(recv_evt_thread != RT_NULL)
	{
		rt_thread_startup(recv_evt_thread);
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
