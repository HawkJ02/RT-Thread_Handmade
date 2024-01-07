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

/* �����ڴ�ؿ��ƿ�ָ�� */
static rt_mp_t mempool = RT_NULL;

/* ���������ڴ��ָ�� */
/* ��̬�ڴ� */
static rt_uint32_t *p_test1 = RT_NULL;
static rt_uint32_t *p_test2 = RT_NULL;
/* ��̬�ڴ� */
#define DYN_SIZE 100
static rt_uint32_t *p_dyn = RT_NULL;
/*
*************************************************************************
*                             �̶߳���
*************************************************************************
*/
static void led1_thread_entry(void* parameter)
{	
		rt_uint32_t i;
		rt_err_t uwRet = RT_EOK;
		rt_kprintf("�������ڴ�������ڴ�.........\n");
		p_test1 = rt_mp_alloc(mempool,0);
		if(p_test1 != RT_NULL)
		{
			rt_kprintf("��̬�ڴ�����ɹ����ڴ��ַΪ%lx!\n",p_test1);
		}
		else
		{
			rt_kprintf("��̬�ڴ�����ʧ��!\n");
		}
		
		p_test2 = rt_mp_alloc(mempool,0);
		if(p_test2 != RT_NULL)
		{
			rt_kprintf("��̬�ڴ�����ɹ����ڴ��ַΪ%lx!\n",p_test2);
		}
		else
		{
			rt_kprintf("��̬�ڴ�����ʧ��!\n");
		}
		
		rt_kprintf("������p_testд������.........\n");
		*p_test1 = 0xDEADBEEF;
		*p_test2 = 0xDEADBABE;
		rt_kprintf("�����ѳɹ�д�룡\n");
		rt_kprintf("*p_test1 = %lx��,��ַΪ%lx\n",*p_test1,p_test1);
		rt_kprintf("*p_test2 = %lx��,��ַΪ%lx\n",*p_test2,p_test2);
		
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
	
	rt_kprintf("����һ��RTT�ڴ����ʵ�飡\n\n");
	rt_kprintf("�����Ǿ�̬�ڴ���䣡\n");
	
	mempool = rt_mp_create("mempool",20,3);
	if(mempool != RT_NULL)
	{
		rt_kprintf("��̬�ڴ�ش����ɹ���\n");
	}
	else
	{
		return -1;
	}
	
	
	
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
	
	
	
	
}

/*********************************************END OF FILE**********************/
