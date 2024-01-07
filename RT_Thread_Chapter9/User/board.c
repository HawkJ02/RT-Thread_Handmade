/* ������Ӳ�����ͷ�ļ� */
#include "board.h" 

/* RT-Thread���ͷ�ļ� */
#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE (8*1024)/4
/* ���ڲ�SRAM�������һ���־�̬�ڴ�����Ϊrtt�Ķѿռ䣬��������Ϊ8KB
	 ע���ˣ����ڶ����rt_heap����ÿһ����Ԫ��32λ�ģ�����ÿһ�ο��ٶ���32λ��4���ֽڣ���
	 ���������Ҫ���Եؿ���������Ҫʹ������/4�ķ�ʽ��*/
static uint32_t rt_heap[RT_HEAP_SIZE];

RT_WEAK void *rt_heap_begin_get(void)
{
    return rt_heap;
}

RT_WEAK void *rt_heap_end_get(void)
{
    return rt_heap + RT_HEAP_SIZE;
}
#endif

/**
  * @brief  ������Ӳ����ʼ������
  * @param  ��
  * @retval ��
  *
  * @attention
  * RTT�ѿ�������صĳ�ʼ������ͳһ�ŵ�board.c�ļ���ʵ�֣�
  * ��Ȼ���������Щ����ͳһ�ŵ�main.c�ļ�Ҳ�ǿ��Եġ�
  */
void rt_hw_board_init()
{
    /* ��ʼ��SysTick */
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );	
    
		/* Ӳ��BSP��ʼ��ͳͳ�����������LED�����ڣ�LCD�� */
		
		/* USART �˿ڳ�ʼ�� */
		USART_Config();	 
	
		/* LED �˿ڳ�ʼ�� */
		LED_GPIO_Config();	  
	
		/* KEY �˿ڳ�ʼ�� */
		Key_GPIO_Config();
		
/* ���������ʼ������ (use INIT_BOARD_EXPORT()) */
#ifdef RT_USING_COMPONENTS_INIT
    rt_components_board_init();
#endif
    
#if defined(RT_USING_CONSOLE) && defined(RT_USING_DEVICE)
	rt_console_set_device(RT_CONSOLE_DEVICE_NAME);
#endif
    
#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
    rt_system_heap_init(rt_heap_begin_get(), rt_heap_end_get());
#endif
}

void rt_hw_console_output(const char *str)
{
	/*�����ٽ�Σ��ر��ж�*/
	rt_enter_critical();
	
	while(*str != '\0')
	{
		if(*str == '\n')
		{
			USART_SendData(DEBUG_USARTx,'\r'); // ���ͻس���
			/*�ȴ� USART ���ͼĴ���Ϊ�յ�ѭ����ȷ����һ���ַ��Ѿ�������ɣ����ܼ���������һ���ַ���*/
			while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
		}
		
		USART_SendData(DEBUG_USARTx,*str++); // ���͵�ǰ�ַ�
		/*�ȴ� USART ���ͼĴ���Ϊ�յ�ѭ����ȷ����һ���ַ��Ѿ�������ɣ����ܼ���������һ���ַ���*/
		while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
	
	}
	/* �˳��ٽ�� */
  rt_exit_critical();
}


/**
  * @brief  SysTick�жϷ�����
  * @param  ��
  * @retval ��
  *
  * @attention
  * SysTick�жϷ������ڹ̼����ļ�stm32f10x_it.c��Ҳ�����ˣ�������
  * ��board.c���ֶ���һ�Σ���ô�����ʱ�������ظ�����Ĵ��󣬽��
  * �����ǿ��԰�stm32f10x_it.c�е�ע�ͻ���ɾ�����ɡ�
  */
void SysTick_Handler(void)
{
    /* �����ж� */
    rt_interrupt_enter();

    /* ����ʱ�� */
    rt_tick_increase();

    /* �뿪�ж� */
    rt_interrupt_leave();
}
