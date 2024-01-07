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
// ������Ϣ�ṹ��
struct msg
{
    rt_uint8_t *data_ptr;  // ָ�����ݿ��ָ��
    rt_uint32_t data_size; // ���ݿ�ĳ���
};

/* ����ȫ�ֱ��� */
char buffer0[] = "this is KEY0 mail !";
char buffer1[] = "this is KEY1 mail !";
char buffer_timer[] = "this is buffer mail !";
/* �����߳̿��ƿ�ָ�� */
static rt_thread_t send_mb_thread = RT_NULL;
static rt_thread_t recv_mb_thread = RT_NULL;


/* ���廥�������ƿ�ָ�� */
static rt_mailbox_t key_mail = RT_NULL;

/* ���嶨ʱ�����ƿ�ָ�� */
static rt_timer_t swtimer0 = RT_NULL;
/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
static void send_mb_thread_entry(void* parameter)
{		
		rt_err_t uwRet = RT_EOK;
		uint8_t key;
    while (1)
    {
			key = key_scan(0);
			switch(key)
			{
				case KEY0_PRES:
					uwRet = rt_mb_send(key_mail,(rt_uint32_t)&buffer0);
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY0���£��ʼ����ͳɹ�\n");
					}
					else
					{
						rt_kprintf("KEY0���£��ʼ�����ʧ��\n");
					}
					break;
				
				case KEY1_PRES:
					uwRet = rt_mb_send(key_mail,(rt_uint32_t)&buffer1);
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY1���£��ʼ����ͳɹ�\n");
					}
					else
					{
						rt_kprintf("KEY1���£��ʼ�����ʧ��\n");
					}
					break;
			}
			rt_thread_delay(20);
    }
}

static void recv_mb_thread_entry(void* parameter)
{
	rt_err_t uwRet = RT_EOK;
	char *recv_ptr;
	
	while(1)
	{
		struct msg* received_msg_ptr;
		/* ��ߵڶ���������Ҫ�ر�ע�⣬��Ϊ����Ϣ����洢�����ʼ��ĵ�ַ��
			 �����պ����д����� *value = mb->msg_pool[mb->out_offset]; 
			 ��˼���Ǵ����ָ��ָ���λ�ý���洢�ʼ��ĵ�ַ��
			 ���ǰ���������ʽ����ͻ��ɣ�*(&recv_ptr) = recv_ptr = mb->msg_pool[mb->out_offset]; ����������ȷ��
			 �����recv_ptr��printf�У�Ҳ�����ַ����ĵ�һ���ַ��ĵ�ַ���Ϳ��Դ�ӡ����λ�õ�������*/
//		uwRet = rt_mb_recv(key_mail,(rt_uint32_t *)&recv_ptr,RT_WAITING_FOREVER);
//		if(uwRet == RT_EOK)
//		{
//			rt_kprintf("�����������%s !\n\n",recv_ptr);
//			LED1_TOGGLE;
//		}
//		else
//		{
//			rt_kprintf("������ܴ��󣡴�������0x%x\n",uwRet);
//		}	
		
    // ����Ϣ�����н�����Ϣָ��
    if (rt_mb_recv(key_mail, (rt_uint32_t*)&received_msg_ptr,RT_WAITING_FOREVER) == RT_EOK)
    {
        // "Received message: %.*s\n" �Ǹ�ʽ���ַ��������� %.*s �Ǹ�ʽ���Ʒ���
				//�����%.*s ����������ȿɱ���ַ�����ǰ��� .* �ᱻ���洫��Ĳ����滻��
				//������˵��%.*s �е� * ��ʾ�����ɺ���Ĳ���ָ������ s ��ʾ������һ���ַ�����
        rt_kprintf("Received message: %.*s\n", received_msg_ptr->data_size, received_msg_ptr->data_ptr);

        // �ͷ��ڴ�
        rt_free(received_msg_ptr);
    }
		
		
	}
}

static void swtimer0_callback(void* parameter)
{		
		rt_err_t uwRet = RT_EOK;
    while (1)
    {
					uwRet = rt_mb_send(key_mail,(rt_uint32_t)&buffer_timer);
					if(uwRet == RT_EOK)
					{
						rt_kprintf("�����ʱ��0���ʼ����ͳɹ�! \n");
						LED2_TOGGLE;
					}
					else
					{
						rt_kprintf("�����ʱ��0���ʼ�����ʧ��! \n");
					}
					break;
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
	// �ڷ����߳���
  struct msg* msg_ptr;
	// ������һ��������Ҫ����
	char data[] = "Hello, Embedded Systems!";
	int data_length = sizeof(data);
	/* ����汾��Ϣ */
	rt_show_version();
	
	rt_kprintf("����RTT����ʵ�飡\n");
	
	LED1_ON;
	LED2_ON;
	 /* 
	 * ������Ӳ����ʼ����RTTϵͳ��ʼ���Ѿ���main����֮ǰ��ɣ�
	 * ����component.c�ļ��е�rtthread_startup()����������ˡ�
	 * ������main�����У�ֻ��Ҫ�����̺߳������̼߳��ɡ�
	 */
	/* �������� */
	key_mail = rt_mb_create("key_mail",10,RT_IPC_FLAG_FIFO);
	if(key_mail != RT_NULL)
	{
		rt_kprintf("���䴴���ɹ���\n");
	}
	else
	{
		rt_kprintf("���䴴��ʧ�ܣ�\n");
	}
	
//	swtimer0 = rt_timer_create("swtimer0",swtimer0_callback,0,1000,RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
//	if(swtimer0 != RT_NULL)
//	{
//		rt_timer_start(swtimer0);
//		rt_kprintf("�����ʱ��0�����ɹ����������� \n");
//	}
//	else
//	{
//		rt_kprintf("�����ʱ��0����ʧ�ܣ�\n");
//	}
//	
//	
//	/* �������������¼������߳� */
//	send_mb_thread = rt_thread_create("send_mb_thread",
//																	send_mb_thread_entry,
//																	RT_NULL,
//																	512,
//																	1,
//																	20);
//	if(send_mb_thread != RT_NULL)
//	{
//		rt_thread_startup(send_mb_thread);
//	}
//	else
//	{
//		return -1;
//	}
//	
	/* �����¼��ȴ��̣߳�������һ����Ҫ���س�ʼ��֮��Ȼ����������Ҫ�豸 */
	recv_mb_thread = rt_thread_create("recv_mb_thread",
																	recv_mb_thread_entry,
																	RT_NULL,
																	512,
																	1,
																	20);
	if(recv_mb_thread != RT_NULL)
	{
		rt_thread_startup(recv_mb_thread);
	}
	else
	{
		return -1;
	}
//	
//	
	 
    // ������Ϣ�ṹ���ڴ�
    msg_ptr = (struct msg*)rt_malloc(sizeof(struct msg));


    // �����Ϣ�ṹ��
    msg_ptr->data_ptr = (rt_uint8_t*)data;
    msg_ptr->data_size = data_length;
		
		// ������Ϣָ�����Ϣ����
    rt_kprintf("Sending message...\n");
    rt_mb_send(key_mail, (rt_uint32_t)msg_ptr);
}

void Delay(__IO uint32_t nCount)	 //�򵥵���ʱ����
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
