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
static rt_thread_t key_thread = RT_NULL;

/* 定义内存池控制块指针 */
static rt_mp_t mempool = RT_NULL;

/* 定义申请内存的指针 */
/* 静态内存 */
static rt_uint32_t *p_test1 = RT_NULL;
static rt_uint32_t *p_test2 = RT_NULL;
/* 动态内存 */
#define DYN_SIZE 100
static rt_uint32_t *p_dyn = RT_NULL;
/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
static void led1_thread_entry(void* parameter)
{	
		rt_uint32_t i;
		rt_err_t uwRet = RT_EOK;
		rt_kprintf("正在向内存池申请内存.........\n");
		p_test1 = rt_mp_alloc(mempool,0);
		if(p_test1 != RT_NULL)
		{
			rt_kprintf("静态内存申请成功！内存地址为%lx!\n",p_test1);
		}
		else
		{
			rt_kprintf("静态内存申请失败!\n");
		}
		
		p_test2 = rt_mp_alloc(mempool,0);
		if(p_test2 != RT_NULL)
		{
			rt_kprintf("静态内存申请成功！内存地址为%lx!\n",p_test2);
		}
		else
		{
			rt_kprintf("静态内存申请失败!\n");
		}
		
		rt_kprintf("正在向p_test写入数据.........\n");
		*p_test1 = 0xDEADBEEF;
		*p_test2 = 0xDEADBABE;
		rt_kprintf("数据已成功写入！\n");
		rt_kprintf("*p_test1 = %lx！,地址为%lx\n",*p_test1,p_test1);
		rt_kprintf("*p_test2 = %lx！,地址为%lx\n",*p_test2,p_test2);
		
		p_dyn = (rt_uint32_t *)rt_malloc(sizeof(rt_uint32_t) * DYN_SIZE);
		
		for(i =0;i<DYN_SIZE;i++)
		{
			p_dyn[i] = 0xDEADBEAF;
		}
		
		rt_free(p_dyn);
		rt_mp_free(p_test1);
		
		
    while (1)
    {	
				
    }
}

static void led2_thread_entry(void* parameter)
{
    rt_err_t uwRet = RT_EOK;
    while (1)
    {
				
        
					
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
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	
	rt_kprintf("这是一个RTT内存管理实验！\n\n");
	rt_kprintf("首先是静态内存分配！\n");
	
	mempool = rt_mp_create("mempool",20,3);
	if(mempool != RT_NULL)
	{
		rt_kprintf("静态内存池创建成功！\n");
	}
	else
	{
		return -1;
	}
	
	
	
	/*LED1线程*/
	led1_thread = rt_thread_create("led1",
																	led1_thread_entry,
																	RT_NULL,
																	512,
																	1,
																	20);
	if(led1_thread != RT_NULL)
	{
		rt_thread_startup(led1_thread);
	}
	else
	{
		return -1;
	}
	
	/*LED2线程*/
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

/*********************************************END OF FILE**********************/
