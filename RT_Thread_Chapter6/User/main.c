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
static rt_thread_t key_thread = RT_NULL;

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
static void led1_thread_entry(void* parameter)
{	
		rt_err_t uwRet = RT_EOK;
		uint8_t key;
    while (1)
    {	
        LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
			
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
				rt_kprintf("led1_thread_running,LED1 OFF!\r\n");
			
				LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
			
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
				rt_kprintf("led1_thread_running,LED1 OFF!\r\n");	 	
			
				LED1_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
				rt_kprintf("led1_thread_running,LED1 ON!\r\n");
			
        LED1_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
				rt_kprintf("led1_thread_running,LED1 OFF!\r\n");	 	
						
//				rt_kprintf("���Թ���LED1�߳�!\r\n");
//			
//				uwRet = rt_thread_suspend(led1_thread);
//				if(uwRet == RT_EOK)
//				{
//					rt_kprintf("LED1�̳߳ɹ�����\n");
//				}
//				else
//				{
//					rt_kprintf("LED1�̹߳���ʧ�ܣ�ʧ�ܴ���0x%lx��\n",uwRet);
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
        rt_thread_delay(500);   /* ��ʱ500��tick */
				rt_kprintf("led2_thread_running,LED2 ON!\r\n");
			
        LED2_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
				rt_kprintf("led2_thread_running,LED2 OFF!\r\n");
			
				LED2_ON;
        rt_thread_delay(500);   /* ��ʱ500��tick */
				rt_kprintf("led2_thread_running,LED2 ON!\r\n");
			
        LED2_OFF;     
        rt_thread_delay(500);   /* ��ʱ500��tick */		 		
				rt_kprintf("led2_thread_running,LED2 OFF!\r\n");
//				rt_kprintf("���Թ���LED1�߳�!\r\n");
//			
//				rt_thread_resume(led1_thread);
//				uwRet = rt_thread_suspend(led1_thread);
//				if(uwRet == RT_EOK)
//				{
//					rt_kprintf("LED1�̳߳ɹ�����\n");
//				}
//				else
//				{
//					rt_kprintf("LED1�̹߳���ʧ�ܣ�ʧ�ܴ���0x%lx��\n",uwRet);
//				}
					
    }
}

/* ���Գ��򣬽���ǳ����߳���ۣ�key�߳��޷�����led1�߳� */
//static void key_thread_entry(void* parameter)
//{
//		rt_err_t uwRet = RT_EOK;
//    while (1)
//    {
//				/*KEY1���¹����߳�*/
//        if(Key_Scan(KEY1_GPIO_PORT,KEY1_GPIO_PIN) == KEY_ON)
//				{
//					rt_kprintf("����LED1�̣߳�\n");
//					uwRet = rt_thread_suspend(led1_thread);
//					if(uwRet == RT_EOK)
//					{
//						rt_kprintf("LED1�̳߳ɹ�����\n");
//					}
//					else
//					{
//						rt_kprintf("LED1�̹߳���ʧ�ܣ�ʧ�ܴ���0x%lx��\n",uwRet);
//					}
//				}
//				
//				/*KEY2���»ָ��߳�*/
//				if(Key_Scan(KEY2_GPIO_PORT,KEY2_GPIO_PIN) == KEY_ON)
//				{
//					rt_kprintf("�ָ�LED1�̣߳�\n");
//					uwRet = rt_thread_resume(led1_thread);
//					if(uwRet == RT_EOK)
//					{
//						rt_kprintf("LED1�̳߳ɹ��ָ���\n");
//					}
//					else
//					{
//						rt_kprintf("LED1�ָ̻߳�ʧ�ܣ�ʧ�ܴ���0x%lx��\n",uwRet);
//					}
//				}
//				rt_thread_delay(200);
//    }
//}

/* ���Գ��򣬽���ǳ����߳���ۣ�key�߳�����޷�����led1�̣߳����Ǹó����еİ���ɨ��ֵ��ѧϰ */
/* ���裺��Ը��֮Ϊ��ʬ��������������ʱ������ָ̻߳�Ȼ����������֮ǰ������������ */
static void key_thread_entry(void* parameter)
{
		rt_err_t uwRet = RT_EOK;
		uint8_t key;
		while(1)
		{
				key = key_scan(0);                  /* �õ���ֵ */
				
        if (key)
        {
            switch (key)
            {
                case KEY1_PRES:             /* ����LED1(GREEN)��ת */
                    rt_kprintf("����LED1�̣߳�\n");
										/* ���裺��Ը��֮Ϊ��ʬ��������������ʱ������ָ̻߳�Ȼ����������֮ǰ������������ */
										rt_thread_resume(led1_thread);
										/* ���裺������ָ�ֱ�ӹ���Ļ������ʧ�� */
										
										uwRet = rt_thread_suspend(led1_thread);
										if(uwRet == RT_EOK)
										{
											rt_kprintf("LED1�̳߳ɹ�����\n");
										}
										else
										{
											rt_kprintf("LED1�̹߳���ʧ�ܣ�ʧ�ܴ���0x%lx��\n",uwRet);
										}
                    break;

                case KEY0_PRES:             /* ͬʱ����LED0, LED1��ת */
                    rt_kprintf("�ָ�LED1�̣߳�\n");
										uwRet = rt_thread_resume(led1_thread);
										if(uwRet == RT_EOK)
										{
											rt_kprintf("LED1�̳߳ɹ��ָ���\n");
										}
										else
										{
											rt_kprintf("LED1�ָ̻߳�ʧ�ܣ�ʧ�ܴ���0x%lx��\n",uwRet);
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
	
	rt_kprintf("����һ��RTT�̹߳���ʵ�飡\n\n");
	rt_kprintf("LED1�̻߳�����Լ�\n");
	
//	uint8_t key;
//	LED1_ON;
//	LED2_ON;
//	while(1)
//	{
//				if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3) == 0)
//				{
//					rt_kprintf("key1����\n");
//				}
//				
//				if(GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4) == 0)
//				{
//					rt_kprintf("key0����\n");
//				}
//				
//		    key = key_scan(0);                  /* �õ���ֵ */
//				
//        if (key)
//        {
//            switch (key)
//            {
//                case KEY1_PRES:             /* ����LED1(GREEN)��ת */
//                    LED1_TOGGLE;          /* LED1״̬ȡ�� */
//                    break;

//                case KEY0_PRES:             /* ͬʱ����LED0, LED1��ת */
//                    LED2_TOGGLE;          /* LED0״̬ȡ�� */
//                    LED1_TOGGLE;          /* LED1״̬ȡ�� */
//                    break;
//            } 
//        }
//        else
//        {
//            Delay(100);
//        }
//	}

	/*LED1�߳�*/
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
	
	/*LED2�߳�*/
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
	
	/*KEY�߳�*/
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

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
