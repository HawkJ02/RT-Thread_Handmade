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

/* �����߳̿��ƿ�ָ�� */
static rt_thread_t send_thread = RT_NULL;
static rt_thread_t recv_thread = RT_NULL;

static rt_thread_t B_occupy_thread = RT_NULL;
static rt_thread_t B_wait_thread = RT_NULL;

static rt_thread_t C_occupy_thread = RT_NULL;
static rt_thread_t C_release_thread = RT_NULL;

/* ������Ϣ���ƿ�ָ�� */
static rt_mq_t test_mq = RT_NULL;

/* �����ź������ƿ�ָ�� */
static rt_sem_t Binary_sem;	/* ��ֵ�ź��� */
static rt_sem_t Count_sem;		/* �����ź��� */

/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
static void receive_thread_entry(void* parameter)
{	
		rt_err_t uwRet = RT_EOK;
		uint32_t r_queue;
    while (1)
    {
			//rt_kprintf("�ѽ�������̣߳�\n");
			/* ���ж�ȡ�����գ����ȴ�ʱ��Ϊһֱ�ȴ� */
			uwRet = rt_mq_recv(	test_mq,
														&r_queue,
														sizeof(r_queue),
														RT_WAITING_FOREVER);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("���ν��յ��������ǣ�%d\n",r_queue);
			}
			else
			{
				rt_kprintf("���ݽ��մ��󣬴������ : 0x%lx\n",uwRet);
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
			//rt_kprintf("�ѽ��뷢���̣߳�\n");
			key = key_scan(0);
			
			switch(key)
			{
				case KEY0_PRES:
					uwRet = rt_mq_send(test_mq,&send_data0,sizeof(send_data0));
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY0���£�����0�ѷ���\n");
					}
					else
					{
						rt_kprintf("���ݲ��ܷ�������Ϣ���У�������� : 0x%lx\n",uwRet);
					}
					break;
					
				case KEY1_PRES:
					uwRet = rt_mq_send(test_mq,&send_data1,sizeof(send_data1));
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY1���£�����1�ѷ���\n");
					}
					else
					{
						rt_kprintf("���ݲ��ܷ�������Ϣ���У�������� : 0x%lx\n",uwRet);
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
			//rt_kprintf("�ѽ����ź���ռ���̣߳�\n");
			rt_sem_take(Binary_sem,RT_WAITING_FOREVER);
			++ ucValue[0];
			rt_thread_delay(500);
			++ ucValue[1];
			
			uwRet = rt_sem_release(Binary_sem);
			if(uwRet == RT_EOK)
			{
				rt_kprintf("��ֵ�ź������ͷţ�\n");
			}
			rt_thread_yield();
    }
}

static void B_wait_thread_entry(void* parameter)
{		
    while (1)
    {
			//rt_kprintf("�ѽ����ź����ȴ��̣߳�\n");
			rt_sem_take(Binary_sem,RT_WAITING_FOREVER);
			
			if(ucValue[0] == ucValue[1])
			{
				rt_kprintf("��ֵ�ź����߳���ͬ����\n");
			}
			else
			{
				rt_kprintf("��ֵ�ź���ͬ��ʧ�ܣ�\n");
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
			//rt_kprintf("�ѽ�������ź���ռ���̣߳�\n");

			key = key_scan(0);
			
			if(key == KEY1_PRES)
			{
				uwRet = rt_sem_take(Count_sem,0);
				if(uwRet == RT_EOK)
				{
					rt_kprintf("KEY1���£��ѻ�ȡһ�������ź�����\n");
				}
				else
				{
					rt_kprintf("KEY1���£������ź���û�п��࣡\n");
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
			//rt_kprintf("�ѽ�������ź����ͷ��̣߳�\n");

			key = key_scan(0);
			
			if(key == KEY0_PRES)
			{
				uwRet = rt_sem_release(Count_sem);
				if(uwRet == RT_EOK)
				{
					rt_kprintf("KEY0���£����ͷ�һ�������ź�����\n");
				}
				else
				{
					rt_kprintf("KEY0���£������ź�����δ��ռ�ã�\n");
				}
			}
			rt_thread_delay(20);
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
	rt_kprintf("����RTT��Ϣ�������ź���ʵ�飡\n");
	rt_kprintf("����KEY1����KEY2���Ͷ�����Ϣ������ʵ��ͬ����ֵ�ź����ͼ����ź�����\n");
	rt_kprintf("�����߳̽��յ���Ϣ���к���ڴ��ڴ�ӡ��ͬ���ɹ������������ӡ��\n");
	 /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	/* ������Ϣ���� */
	test_mq = rt_mq_create("test_mq",40,20,RT_IPC_FLAG_FIFO);
	if(test_mq != RT_NULL)
	{
		rt_kprintf("��Ϣ���д����ɹ���\n");
	}
	else
	{
		rt_kprintf("��Ϣ���д���ʧ�ܣ�\n");
	}
	
	/* ������ֵ�ź��� */
	Binary_sem = rt_sem_create("Binart_sem",1,RT_IPC_FLAG_FIFO);
	if(Binary_sem != RT_NULL)
	{
		rt_kprintf("��ֵ�ź��������ɹ���\n");
	}
	else
	{
		rt_kprintf("��ֵ�ź�������ʧ�ܣ�\n");
	}
	
	/* ���������ź��� */
	Count_sem = rt_sem_create("Count_sem",5,RT_IPC_FLAG_FIFO);
	if(Count_sem != RT_NULL)
	{
		rt_kprintf("�����ź��������ɹ���\n");
	}
	else
	{
		rt_kprintf("�����ź�������ʧ�ܣ�\n");
	}
	
	/* ������Ϣ�����߳� */
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
	
	/* ������Ϣ�����߳� */
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
	
	/* ������ֵ�ź����ȴ��߳� */
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
	
	/* ������ֵ�ź���ռ���߳� */
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
	
	/* ���������ź����ͷ��߳� */
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
	
	/* ���������ź���ռ���߳� */
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

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
