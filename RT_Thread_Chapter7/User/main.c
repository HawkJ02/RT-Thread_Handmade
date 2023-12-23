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

/* 定义线程控制块指针 */
static rt_thread_t send_thread = RT_NULL;
static rt_thread_t recv_thread = RT_NULL;

static rt_thread_t B_occupy_thread = RT_NULL;
static rt_thread_t B_wait_thread = RT_NULL;

static rt_thread_t C_occupy_thread = RT_NULL;
static rt_thread_t C_release_thread = RT_NULL;

/* 定义消息控制块指针 */
static rt_mq_t test_mq = RT_NULL;

/* 定义信号量控制块指针 */
static rt_sem_t Binary_sem;	/* 二值信号量 */
static rt_sem_t Count_sem;		/* 计数信号量 */

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
static void receive_thread_entry(void* parameter)
{	
		rt_err_t uwRet = RT_EOK;
		uint32_t r_queue;
    while (1)
    {
			//rt_kprintf("已进入接收线程！\n");
			/* 队列读取（接收），等待时间为一直等待 */
			uwRet = rt_mq_recv(	test_mq,
														&r_queue,
														sizeof(r_queue),
														RT_WAITING_FOREVER);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("本次接收到的数据是：%d\n",r_queue);
			}
			else
			{
				rt_kprintf("数据接收错误，错误代码 : 0x%lx\n",uwRet);
			}
			rt_thread_delay(200);
    }
}

static void send_thread_entry(void* parameter)
{		
		rt_err_t 	uwRet = RT_EOK;
		uint32_t	send_data0 = 0;
		uint32_t	send_data1 = 1;
		uint8_t		key;
		
    while (1)
    {
			//rt_kprintf("已进入发送线程！\n");
			key = key_scan(0);
			
			switch(key)
			{
				case KEY0_PRES:
					uwRet = rt_mq_send(test_mq,&send_data0,sizeof(send_data0));
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY0按下，数据0已发送\n");
					}
					else
					{
						rt_kprintf("数据不能发送至消息队列，错误代码 : 0x%lx\n",uwRet);
					}
					break;
					
				case KEY1_PRES:
					uwRet = rt_mq_send(test_mq,&send_data1,sizeof(send_data1));
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY1按下，数据1已发送\n");
					}
					else
					{
						rt_kprintf("数据不能发送至消息队列，错误代码 : 0x%lx\n",uwRet);
					}
					break;
			}
			rt_thread_delay(100);
    }
}

static void B_occupy_thread_entry(void* parameter)
{	
		rt_err_t uwRet;
    while (1)
    {
			//rt_kprintf("已进入信号量占用线程！\n");
			rt_sem_take(Binary_sem,RT_WAITING_FOREVER);
			++ ucValue[0];
			rt_thread_delay(500);
			++ ucValue[1];
			
			uwRet = rt_sem_release(Binary_sem);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("二值信号量已释放！\n");
			}
			rt_thread_yield();
    }
}

static void B_wait_thread_entry(void* parameter)
{		
    while (1)
    {
			//rt_kprintf("已进入信号量等待线程！\n");
			rt_sem_take(Binary_sem,RT_WAITING_FOREVER);
			
			if(ucValue[0] == ucValue[1])
			{
				rt_kprintf("二值信号量线程已同步！\n");
			}
			else
			{
				rt_kprintf("二值信号量同步失败！\n");
			}
			rt_sem_release(Binary_sem);
			rt_thread_delay(1000);
    }
}

static void C_occupy_thread_entry(void* parameter)
{	
		rt_err_t uwRet;
		uint8_t key;
	
    while (1)
    {
			//rt_kprintf("已进入计数信号量占用线程！\n");

			key = key_scan(0);
			
			if(key == KEY1_PRES)
			{
				uwRet = rt_sem_take(Count_sem,0);
				if(uwRet == RT_EOK)
				{
					rt_kprintf("KEY1按下！已获取一个计数信号量！\n");
				}
				else
				{
					rt_kprintf("KEY1按下！计数信号量没有空余！\n");
				}
			}
			
			rt_thread_delay(20);
    }
}

static void C_release_thread_entry(void* parameter)
{	
		rt_err_t uwRet;
		uint8_t key;
	
    while (1)
    {
			//rt_kprintf("已进入计数信号量释放线程！\n");

			key = key_scan(0);
			
			if(key == KEY0_PRES)
			{
				uwRet = rt_sem_release(Count_sem);
				if(uwRet == RT_EOK)
				{
					rt_kprintf("KEY0按下！已释放一个计数信号量！\n");
				}
				else
				{
					rt_kprintf("KEY0按下！计数信号量都未被占用！\n");
				}
			}
			rt_thread_delay(20);
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
	rt_kprintf("这是RTT消息队列与信号量实验！\n");
	rt_kprintf("按下KEY1或者KEY2发送队列消息，并且实现同步二值信号量和计数信号量！\n");
	rt_kprintf("接收线程接收到消息队列后会在串口打印，同步成功与计数结果会打印！\n");
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	/* 创建消息队列 */
	test_mq = rt_mq_create("test_mq",40,20,RT_IPC_FLAG_FIFO);
	if(test_mq != RT_NULL)
	{
		rt_kprintf("消息队列创建成功！\n");
	}
	else
	{
		rt_kprintf("消息队列创建失败！\n");
	}
	
	/* 创建二值信号量 */
	Binary_sem = rt_sem_create("Binart_sem",1,RT_IPC_FLAG_FIFO);
	if(Binary_sem != RT_NULL)
	{
		rt_kprintf("二值信号量创建成功！\n");
	}
	else
	{
		rt_kprintf("二值信号量创建失败！\n");
	}
	
	/* 创建计数信号量 */
	Count_sem = rt_sem_create("Count_sem",5,RT_IPC_FLAG_FIFO);
	if(Count_sem != RT_NULL)
	{
		rt_kprintf("计数信号量创建成功！\n");
	}
	else
	{
		rt_kprintf("计数信号量创建失败！\n");
	}
	
	/* 创建消息发送线程 */
	send_thread = rt_thread_create("send_thread",
																	send_thread_entry,
																	RT_NULL,
																	256,
																	3,
																	20);
	if(send_thread != RT_NULL)
	{
		rt_thread_startup(send_thread);
	}
	else
	{
		return -1;
	}
	
	/* 创建消息接收线程 */
	recv_thread = rt_thread_create("recv_thread",
																	receive_thread_entry,
																	RT_NULL,
																	256,
																	2,
																	20);
	if(recv_thread != RT_NULL)
	{
		rt_thread_startup(recv_thread);
	}
	else
	{
		return -1;
	}
	
	/* 创建二值信号量等待线程 */
	B_wait_thread = rt_thread_create("B_wait_thread",
																	B_wait_thread_entry,
																	RT_NULL,
																	256,
																	4,
																	20);
	if(B_wait_thread != RT_NULL)
	{
		rt_thread_startup(B_wait_thread);
	}
	else
	{
		return -1;
	}
	
	/* 创建二值信号量占用线程 */
	B_occupy_thread = rt_thread_create("B_occupy_thread",
																	B_occupy_thread_entry,
																	RT_NULL,
																	256,
																	5,
																	20);
	if(B_occupy_thread != RT_NULL)
	{
		rt_thread_startup(B_occupy_thread);
	}
	else
	{
		return -1;
	}
	
	/* 创建计数信号量释放线程 */
	C_release_thread = rt_thread_create("C_release_thread",
																	C_release_thread_entry,
																	RT_NULL,
																	256,
																	6,
																	20);
	if(C_release_thread != RT_NULL)
	{
		rt_thread_startup(C_release_thread);
	}
	else
	{
		return -1;
	}
	
	/* 创建计数信号量占用线程 */
	C_occupy_thread = rt_thread_create("C_occupy_thread",
																	C_occupy_thread_entry,
																	RT_NULL,
																	256,
																	0,
																	20);
	if(C_occupy_thread != RT_NULL)
	{
		rt_thread_startup(C_occupy_thread);
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
