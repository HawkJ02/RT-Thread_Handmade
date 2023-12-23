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

/*
*************************************************************************
*                             线程定义
*************************************************************************
*/
static void led1_thread_entry(void* parameter)
{	
		rt_err_t uwRet = RT_EOK;
		uint8_t key;
    while (1)
    {	
        LED1_ON;
        rt_thread_delay(500);   /* 延时500个tick */
				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
			
        LED1_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
				rt_kprintf("led1_thread_running,LED1 OFF!\r\n");
			
				LED1_ON;
        rt_thread_delay(500);   /* 延时500个tick */
				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
			
        LED1_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
				rt_kprintf("led1_thread_running,LED1 OFF!\r\n");	 	
			
				LED1_ON;
        rt_thread_delay(500);   /* 延时500个tick */
				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
			
        LED1_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
				rt_kprintf("led1_thread_running,LED1 OFF!\r\n");	 	
						
//				rt_kprintf("尝试挂起LED1线程!\r\n");
//			
//				uwRet = rt_thread_suspend(led1_thread);
//				if(uwRet == RT_EOK)
//				{
//					rt_kprintf("LED1线程成功挂起！\n");
//				}
//				else
//				{
//					rt_kprintf("LED1线程挂起失败，失败代码0x%lx！\n",uwRet);
//				}
    }
}

static void led2_thread_entry(void* parameter)
{
    rt_err_t uwRet = RT_EOK;
		uint8_t key;
    while (1)
    {
				
        LED2_ON;
        rt_thread_delay(500);   /* 延时500个tick */
				rt_kprintf("led2_thread_running,LED2 ON!\r\n");
			
        LED2_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
				rt_kprintf("led2_thread_running,LED2 OFF!\r\n");
			
				LED2_ON;
        rt_thread_delay(500);   /* 延时500个tick */
				rt_kprintf("led2_thread_running,LED2 ON!\r\n");
			
        LED2_OFF;     
        rt_thread_delay(500);   /* 延时500个tick */		 		
				rt_kprintf("led2_thread_running,LED2 OFF!\r\n");
//				rt_kprintf("尝试挂起LED1线程!\r\n");
//			
//				rt_thread_resume(led1_thread);
//				uwRet = rt_thread_suspend(led1_thread);
//				if(uwRet == RT_EOK)
//				{
//					rt_kprintf("LED1线程成功挂起！\n");
//				}
//				else
//				{
//					rt_kprintf("LED1线程挂起失败，失败代码0x%lx！\n",uwRet);
//				}
					
    }
}

/* 测试程序，结果是出现线程悖论，key线程无法挂起led1线程 */
//static void key_thread_entry(void* parameter)
//{
//		rt_err_t uwRet = RT_EOK;
//    while (1)
//    {
//				/*KEY1按下挂起线程*/
//        if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
//				{
//					rt_kprintf("挂起LED1线程！\n");
//					uwRet = rt_thread_suspend(led1_thread);
//					if(uwRet == RT_EOK)
//					{
//						rt_kprintf("LED1线程成功挂起！\n");
//					}
//					else
//					{
//						rt_kprintf("LED1线程挂起失败，失败代码0x%lx！\n",uwRet);
//					}
//				}
//				
//				/*KEY2按下恢复线程*/
//				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
//				{
//					rt_kprintf("恢复LED1线程！\n");
//					uwRet = rt_thread_resume(led1_thread);
//					if(uwRet == RT_EOK)
//					{
//						rt_kprintf("LED1线程成功恢复！\n");
//					}
//					else
//					{
//						rt_kprintf("LED1线程恢复失败，失败代码0x%lx！\n",uwRet);
//					}
//				}
//				rt_thread_delay(200);
//    }
//}

/* 测试程序，结果是出现线程悖论，key线程如果无法挂起led1线程，但是该程序中的按键扫描值得学习 */
/* 精髓：我愿称之为鞭尸，将由于阻塞延时挂起的线程恢复然后在他运行之前将他主动挂起！ */
static void key_thread_entry(void* parameter)
{
		rt_err_t uwRet = RT_EOK;
		uint8_t key;
		while(1)
		{
				key = key_scan(0);                  /* 得到键值 */
				
        if (key)
        {
            switch (key)
            {
                case KEY1_PRES:             /* 控制LED1(GREEN)翻转 */
                    rt_kprintf("挂起LED1线程！\n");
										/* 精髓：我愿称之为鞭尸，将由于阻塞延时挂起的线程恢复然后在他运行之前将他主动挂起！ */
										rt_thread_resume(led1_thread);
										/* 精髓：如果不恢复直接挂起的话会挂起失败 */
										
										uwRet = rt_thread_suspend(led1_thread);
										if(uwRet == RT_EOK)
										{
											rt_kprintf("LED1线程成功挂起！\n");
										}
										else
										{
											rt_kprintf("LED1线程挂起失败，失败代码0x%lx！\n",uwRet);
										}
                    break;

                case KEY0_PRES:             /* 同时控制LED0, LED1翻转 */
                    rt_kprintf("恢复LED1线程！\n");
										uwRet = rt_thread_resume(led1_thread);
										if(uwRet == RT_EOK)
										{
											rt_kprintf("LED1线程成功恢复！\n");
										}
										else
										{
											rt_kprintf("LED1线程恢复失败，失败代码0x%lx！\n",uwRet);
										}
                    break;
            } 
        }
        else
        {
            rt_thread_delay(100);
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
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	
	rt_kprintf("这是一个RTT线程管理实验！\n\n");
	rt_kprintf("LED1线程会挂起自己\n");
	
//	uint8_t key;
//	LED1_ON;
//	LED2_ON;
//	while(1)
//	{
//				if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0)
//				{
//					rt_kprintf("key1按下\n");
//				}
//				
//				if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
//				{
//					rt_kprintf("key0按下\n");
//				}
//				
//		    key = key_scan(0);                  /* 得到键值 */
//				
//        if (key)
//        {
//            switch (key)
//            {
//                case KEY1_PRES:             /* 控制LED1(GREEN)翻转 */
//                    LED1_TOGGLE;          /* LED1状态取反 */
//                    break;

//                case KEY0_PRES:             /* 同时控制LED0, LED1翻转 */
//                    LED2_TOGGLE;          /* LED0状态取反 */
//                    LED1_TOGGLE;          /* LED1状态取反 */
//                    break;
//            } 
//        }
//        else
//        {
//            Delay(100);
//        }
//	}

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
	
	/*KEY线程*/
	key_thread = rt_thread_create("key",
																	key_thread_entry,
																	RT_NULL,
																	512,
																	0,
																	20);
	if(key_thread != RT_NULL)
	{
		rt_thread_startup(key_thread);
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
