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
#include <string.h>

#include "stm32f10x.h"

#include "bsp_led.h"
#include "bsp_exti.h"

/*
*************************************************************************
*                               ����
*************************************************************************
*/
/* �����߳̿��ƿ�ָ�� */
static rt_thread_t usart_thread = RT_NULL;
static rt_thread_t key_thread = RT_NULL;



/* �ⲿ������Ϣ���п��ƿ� */
rt_mq_t test_mq;
rt_sem_t test_sem;

uint32_t send_data1 = 1;
uint32_t send_data2 = 0;

extern char USART_RX_BUFFER[USART_RBUFFER_SIZE];
/*
*************************************************************************
*                             �̶߳���
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
					rt_kprintf("�����жϵ���KEY%d!\n",r_mq);
				}
				else
				{
					rt_kprintf("���ݽ��ճ���,�������: 0x%lx\n",uwRet);
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
					printf("���յ�������Ϊ��%s!\n",USART_RX_BUFFER);
					//rt_kprintf("���յ�������Ϊ��%s!\n",USART_RX_BUFFER);
					memset(USART_RX_BUFFER,0,USART_RBUFFER_SIZE);
				}
        
					
    }
}


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
		LED1_ON;
		LED2_ON;
	 /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	
		rt_kprintf("����һ���жϹ���ʵ�飡��\n\n");
		
		
		test_mq = rt_mq_create("test_mq",4,4,RT_IPC_FLAG_FIFO);
		if(test_mq == RT_NULL)
		{
			rt_kprintf("��Ϣ���д���ʧ�ܣ�\n");
		}
		else
		{
			rt_kprintf("��Ϣ���д����ɹ���\n");
		}
					
		test_sem = rt_sem_create("test_sem",0,RT_IPC_FLAG_FIFO);
		if(test_sem == RT_NULL)
		{
			rt_kprintf("����ռ���ź�������ʧ�ܣ�\n");
		}
		else
		{
			rt_kprintf("����ռ���ź��������ɹ���\n");
		}
		
		
		/*key�߳�*/
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
		
		/*usart1�߳�*/
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

/* �жϷ����� */
void KEY1_IRQHandler(void)
{
		/* �����ж� */
		rt_interrupt_enter();
    //ȷ���Ƿ������EXTI Line�ж�
    if (EXTI_GetITStatus(KEY1_INT_EXTI_LINE) != RESET) {
				LED1_TOGGLE;
        /* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
        rt_mq_send( test_mq,        /* д�루���ͣ����е�ID(���) */
                    &send_data1, /* д�루���ͣ������� */
                    sizeof(send_data1)); /* ���ݵĳ��� */
        //����жϱ�־λ
        EXTI_ClearITPendingBit(KEY1_INT_EXTI_LINE);
    }
		/* �뿪�ж� */
		rt_interrupt_leave();
}

void KEY2_IRQHandler(void)
{
		/* �����ж� */
		rt_interrupt_enter();
    //ȷ���Ƿ������EXTI Line�ж�
    if (EXTI_GetITStatus(KEY2_INT_EXTI_LINE) != RESET) {
				LED2_TOGGLE;
        /* ������д�루���ͣ��������У��ȴ�ʱ��Ϊ 0  */
        rt_mq_send( test_mq,        /* д�루���ͣ����е�ID(���) */
                    &send_data2, /* д�루���ͣ������� */
                    sizeof(send_data2)); /* ���ݵĳ��� */
        //����жϱ�־λ
        EXTI_ClearITPendingBit(KEY2_INT_EXTI_LINE);
    }
		
		/* �뿪�ж� */
		rt_interrupt_leave();
}

void DEBUG_USART_IRQHandler(void)
{
		/* �����ж� */
		rt_interrupt_enter();
		//ȷ���Ƿ������idle�����ж�
    if (USART_GetITStatus(USART1,USART_IT_IDLE) != RESET) {
				LED1_TOGGLE;
				LED2_TOGGLE;
        
				// �������ݲ��ͷ�һ���ź���
				USART1_DMA_RX_DATA();
				/* �����־λ,�Ҿ�����δ�����������ڸ���cpu���Ѿ�
				 * ���յ������ˣ�����dmaֻ�Ǹ�����һ�ݰ����ˣ���û��ʵ�ʵ�������ݣ�
				 * ���Ծ͵���������ٷ��ĺ���������cpu���Ѿ�������������*/
				USART_ReceiveData(DEBUG_USART1); 
        
    }
		
		/* �뿪�ж� */
		rt_interrupt_leave();
}


/*********************************************END OF FILE**********************/
