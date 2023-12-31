/* 开发板硬件相关头文件 */
#include "board.h" 

/* RT-Thread相关头文件 */
#include <rthw.h>
#include <rtthread.h>

#if defined(RT_USING_USER_MAIN) && defined(RT_USING_HEAP)
#define RT_HEAP_SIZE (8*1024)/4
/* 从内部SRAM里面分配一部分静态内存来作为rtt的堆空间，这里配置为8KB
	 注意了，由于定义的rt_heap数组每一个单元是32位的，所以每一次开辟都是32位（4个字节），
	 所以如果想要明显地看出来就需要使用以上/4的方式。*/
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
  * @brief  开发板硬件初始化函数
  * @param  无
  * @retval 无
  *
  * @attention
  * RTT把开发板相关的初始化函数统一放到board.c文件中实现，
  * 当然，你想把这些函数统一放到main.c文件也是可以的。
  */
void rt_hw_board_init()
{
    /* 初始化SysTick */
    SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND );	
    
		/* 硬件BSP初始化统统放在这里，比如LED，串口，LCD等 */
		
		/* USART 端口初始化 */
		USART_Config();	 
	
		/* LED 端口初始化 */
		LED_GPIO_Config();	  
	
		/* KEY 端口初始化 */
		Key_GPIO_Config();
		
/* 调用组件初始化函数 (use INIT_BOARD_EXPORT()) */
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
	/*进入临界段，关闭中断*/
	rt_enter_critical();
	
	while(*str != '\0')
	{
		if(*str == '\n')
		{
			USART_SendData(DEBUG_USARTx,'\r'); // 发送回车符
			/*等待 USART 发送寄存器为空的循环，确保上一个字符已经发送完成，才能继续发送下一个字符。*/
			while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
		}
		
		USART_SendData(DEBUG_USARTx,*str++); // 发送当前字符
		/*等待 USART 发送寄存器为空的循环，确保上一个字符已经发送完成，才能继续发送下一个字符。*/
		while(USART_GetFlagStatus(DEBUG_USARTx,USART_FLAG_TXE) == RESET);
	
	}
	/* 退出临界段 */
  rt_exit_critical();
}


/**
  * @brief  SysTick中断服务函数
  * @param  无
  * @retval 无
  *
  * @attention
  * SysTick中断服务函数在固件库文件stm32f10x_it.c中也定义了，而现在
  * 在board.c中又定义一次，那么编译的时候会出现重复定义的错误，解决
  * 方法是可以把stm32f10x_it.c中的注释或者删除即可。
  */
void SysTick_Handler(void)
{
    /* 进入中断 */
    rt_interrupt_enter();

    /* 更新时基 */
    rt_tick_increase();

    /* 离开中断 */
    rt_interrupt_leave();
}
