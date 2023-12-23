/**
  ******************************************************************************
  * @file    main.c
  * @author  HawkJ
  * @version V1.0
  * @date    2023-12-16
  * @brief   ��ֲRT_Thread
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
#include "bsp_key.h"


/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* ����ȫ�ֱ��� */
uint8_t ucValue[2] = {0x00, 0x00};

/* �����¼������λ */
#define KEY0_EVENT 	(0x01 << 0)
#define KEY1_EVENT	(0x01 << 1)



/* �����߳̿��ƿ�ָ�� */
static rt_thread_t stu_a_thread = RT_NULL;
static rt_thread_t tch_b_thread = RT_NULL;
static rt_thread_t send_evt_thread = RT_NULL;
static rt_thread_t recv_evt_thread = RT_NULL;


/* ���廥�������ƿ�ָ�� */
static rt_mutex_t drill_mux = RT_NULL;	/* ������ */

/* �����¼����ƿ�ָ�� */
static rt_event_t apple_evt = RT_NULL;

/*
*************************************************************************
*                             �̶߳���
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
				rt_kprintf("ѧ��A��ȡ�õ��꣡\n");
			}
			else
			{
				rt_kprintf("��������ʹ���У�\n");
			}
		
			++ ucValue[0];
			rt_thread_delay(100);
			++ ucValue[1];
			uwRet = rt_mutex_release(drill_mux);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("�����ѹ黹��\n");
			}
			else
			{
				rt_kprintf("����黹ʧ�ܣ�\n");
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
				rt_kprintf("�����ź�����ͬ����\n");
			}
			else
			{
				rt_kprintf("�����ź���ͬ��ʧ�ܣ�\n");
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
					rt_kprintf("KEY0���£�\n");
					break;
				
				case KEY1_PRES:
					rt_event_send(apple_evt,KEY1_EVENT);
					rt_kprintf("KEY1���£�\n");
					break;
			}
			rt_thread_delay(20);
    }
}

static void recv_evt_thread_entry(void* parameter)
{
	/* ����һ���ֲ����������ڱ����¼�������ֵ */
	rt_uint32_t recved;
	while(1)
	{
		rt_event_recv(apple_evt,																	/* �¼����� */
									KEY0_EVENT | KEY1_EVENT,										/* �����߳�������Ȥ���¼� */
									RT_EVENT_FLAG_AND | RT_EVENT_FLAG_CLEAR,		/* �¼����ܵ�ģʽ����ģʽ��������Ȥ���¼������뷢����
																																 ���ģʽ���¼���������������ʹ�á�
																																 ����������Լ죨ʹ��һ�Σ��������оͲ���Ҫ��� */
									RT_WAITING_FOREVER,													/* һֱ�ȴ� */
									&recved);																		/* �洢�¼�����ֵ�ľֲ����������ڼ��� */
		
		/* �ٴ�ȷ�ϴ�ʱ���¼�������ֵ�Ƿ�����ȷ�� */
		if((recved == KEY0_EVENT)  || (recved == KEY1_EVENT))
		{
			rt_kprintf("��ģʽ��KEY0��KEY1���£���ȡ�¼��ɹ�!\n");
			LED1_TOGGLE;
		}
		
		if(recved == (KEY0_EVENT | KEY1_EVENT))
		{
			rt_kprintf("��ģʽ��KEY0��KEY1���Ѿ����¹�����ȡ�¼��ɹ�!\n");
			LED1_TOGGLE;
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
	/* ����汾��Ϣ */
	rt_show_version();
	rt_kprintf("����RTT���������¼�ʵ�飡\n");
	rt_kprintf("ͬ���ɹ��������ͬ���ɹ�����\n");
	LED1_ON;
	LED2_ON;
	 /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	/* ���������ź��� */
	drill_mux = rt_mutex_create("drill_mux",RT_IPC_FLAG_FIFO);
	if(drill_mux != RT_NULL)
	{
		rt_kprintf("�����ź��������ɹ���\n");
	}
	else
	{
		rt_kprintf("�����ź�������ʧ�ܣ�\n");
	}
	
	/* �����¼�����Ϊʱ�䵽����ʱ���ǽ�������߳�һ��֪ͨ�����԰�������˵�ǰ������ȼ�˳�� */
	apple_evt = rt_event_create("apple_evt",RT_IPC_FLAG_PRIO);
	if(apple_evt != RT_NULL)
	{
		rt_kprintf("�¼������ɹ���\n");
	}
	else
	{
		rt_kprintf("�¼�����ʧ�ܣ�\n");
	}
	
//	
//	/* ���������ź��������ȼ�ռ���߳�A�߳� */
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
//	/* ���������ź��������ȼ��ȴ��߳�B�߳� */
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
	
	/* �������������¼������߳� */
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
	
	/* �����¼��ȴ��̣߳�������һ����Ҫ���س�ʼ��֮��Ȼ����������Ҫ�豸 */
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

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
