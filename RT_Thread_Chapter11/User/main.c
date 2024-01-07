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
#include <string.h>

#include "stm32f10x.h"

#include "bsp_led.h"
#include "bsp_exti.h"

/*
*************************************************************************
*                               变量
*************************************************************************
*/
/* 定义线程控制块指针 */
static rt_thread_t usart_thread = RT_NULL;
static rt_thread_t key_thread = RT_NULL;



/* 外部定义消息队列控制块 */
rt_mq_t test_mq;
rt_sem_t test_sem;

uint32_t send_data1 = 1;
uint32_t send_data2 = 0;

extern char USART_RX_BUFFER[USART_RBUFFER_SIZE];
/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
static void key_thread_entry(void* parameter)
{	
		rt_err_t uwRet = RT_EOK;
		rt_uint32_t r_mq;
    while (1)
    {	
				uwRet = rt_mq_recv(test_mq,&r_mq,sizeof(r_mq),RT_WAITING_FOREVER);
				if(uwRet == RT_EOK)
				{
					rt_kprintf("触发中断的是KEY%d!\n",r_mq);
				}
				else
				{
					rt_kprintf("数据接收出错,错误代码: 0x%lx\n",uwRet);
				}
    }
}

static void usart_thread_entry(void* parameter)
{
    rt_err_t uwRet = RT_EOK;
    while (1)
    {
				uwRet = rt_sem_take(test_sem,RT_WAITING_FOREVER);
				if(uwRet == RT_EOK)
				{
					printf("接收到的数据为：%s!\n",USART_RX_BUFFER);
					//rt_kprintf("接收到的数据为：%s!\n",USART_RX_BUFFER);
					memset(USART_RX_BUFFER,0,USART_RBUFFER_SIZE);
				}
        
					
    }
}


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
		LED1_ON;
		LED2_ON;
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	
		rt_kprintf("这是一个中断管理实验！！\n\n");
		
		
		test_mq = rt_mq_create("test_mq",4,4,RT_IPC_FLAG_FIFO);
		if(test_mq == RT_NULL)
		{
			rt_kprintf("消息队列创建失败！\n");
		}
		else
		{
			rt_kprintf("消息队列创建成功！\n");
		}
					
		test_sem = rt_sem_create("test_sem",0,RT_IPC_FLAG_FIFO);
		if(test_sem == RT_NULL)
		{
			rt_kprintf("串口占用信号量创建失败！\n");
		}
		else
		{
			rt_kprintf("串口占用信号量创建成功！\n");
		}
		
		
		/*key线程*/
		key_thread = rt_thread_create("key",
																		key_thread_entry,
																		RT_NULL,
																		512,
																		1,
																		20);
		if(key_thread != RT_NULL)
		{
			rt_thread_startup(key_thread);
		}
		else
		{
			return -1;
		}
		
		/*usart1线程*/
		usart_thread = rt_thread_create("usart",
																		usart_thread_entry,
																		RT_NULL,
																		512,
																		2,
																		20);
		if(usart_thread != RT_NULL)
		{
			rt_thread_startup(usart_thread);
		}
		else
		{
			return -1;
		}
}

/* 中断服务函数 */
void KEY1_IRQHandler(void)
{
		/* 进入中断 */
		rt_interrupt_enter();
    //确保是否产生了EXTI Line中断
    if (EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) {
				LED1_TOGGLE;
        /* 将数据写入（发送）到队列中，等待时间为 0  */
        rt_mq_send( test_mq,        /* 写入（发送）队列的ID(句柄) */
                    &send_data1, /* 写入（发送）的数据 */
                    sizeof(send_data1)); /* 数据的长度 */
        //清除中断标志位
        EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
    }
		/* 离开中断 */
		rt_interrupt_leave();
}

void KEY2_IRQHandler(void)
{
		/* 进入中断 */
		rt_interrupt_enter();
    //确保是否产生了EXTI Line中断
    if (EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) {
				LED2_TOGGLE;
        /* 将数据写入（发送）到队列中，等待时间为 0  */
        rt_mq_send( test_mq,        /* 写入（发送）队列的ID(句柄) */
                    &send_data2, /* 写入（发送）的数据 */
                    sizeof(send_data2)); /* 数据的长度 */
        //清除中断标志位
        EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);
    }
		
		/* 离开中断 */
		rt_interrupt_leave();
}

void DEBUG_USART_IRQHandler(void)
{
		/* 进入中断 */
		rt_interrupt_enter();
		//确保是否产生了idle串口中断
    if (USART_GetITStatus(USART1,USART_IT_IDLE) != RESET) {
				LED1_TOGGLE;
				LED2_TOGGLE;
        
				// 接收数据并释放一个信号量
				USART1_DMA_RX_DATA();
				/* 清除标志位,我觉得这段代码的意义在于告诉cpu我已经
				 * 接收到数据了，但是dma只是复制了一份搬走了，并没有实际的清除数据，
				 * 所以就调用下这个官方的函数，告诉cpu我已经接收完数据了*/
				USART_ReceiveData(DEBUG_USART1); 
        
    }
		
		/* 离开中断 */
		rt_interrupt_leave();
}


/*********************************************END OF FILE**********************/
