#include "./SYSTEM/sys/sys.h"
#include "./SYSTEM/usart/usart.h"
#include "./SYSTEM/delay/delay.h"
#include "./BSP/LED/led.h"
#include "./BSP/LCD/lcd.h"


/*
*************************************************************************
*                             包含的头文件
*************************************************************************
*/
#include "rtconfig.h"
#include "rtdef.h"
#include "rtthread.h"
#include "rthw.h"

/* RT-THREAD PRIVATE*/
ALIGN(RT_ALIGN_SIZE)


/*
*************************************************************************
*                              全局变量
*************************************************************************
*/
/* RT-THREAD PRIVATE VALUE*/
uint32_t flag1;
uint32_t flag2;
uint32_t flag3;
uint32_t flag4;

extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

/*
*************************************************************************
*                      线程控制块 & STACK & 线程声明
*************************************************************************
*/
/* RT-THREAD PRIVATE FUNCTION*/
/* 定义线程栈*/
rt_uint8_t rt_flag1_thread_stack[512];
rt_uint8_t rt_flag2_thread_stack[512];
rt_uint8_t rt_flag3_thread_stack[512];
rt_uint8_t rt_flag4_thread_stack[512];


/* 定义线程控制块 */
struct rt_thread rt_flag1_thread;
struct rt_thread rt_flag2_thread;
struct rt_thread rt_flag3_thread;
struct rt_thread rt_flag4_thread;

/*
*************************************************************************
*                               函数声明
*************************************************************************
*/
/* 延时函数 */
void rt_delay(uint32_t count)
{
	for(;count != 0;count--);
}


/* 对于无时间片的多线程结构，如果多个线程处于一个优先级分两种情况
	 1、如果所有的线程都有阻塞延时，则不会产生不良影响，所有线程都正常运行，因为当前线程进入休眠后会从就绪链表中移除
	 2、如果有线程长期占用该线程，则会产生不良影响，高优先级的线程不受影响，但是同等或者低优先级的线程会被堵塞*/
/* 线程1 */
void flag1_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag1 = 1;
		/* 延时20ms */
		rt_delay(100);
		//rt_thread_delay(40);				/* 延时40ms */
		flag1 = 0;
		rt_delay(100);
		//rt_thread_delay(40);				/* 延时40ms */

	}
}

/* 线程2 */
void flag2_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag2 = 1;
		rt_delay(100);
		//rt_thread_delay(20);				/* 延时20ms */
		flag2 = 0;
		rt_delay(100);
		//rt_thread_delay(20);				/* 延时20ms */

	}
}

/* 线程3 */
void flag3_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag3 = 1;
		rt_thread_delay(30);				/* 延时30ms */
		flag3 = 0;
		rt_thread_delay(30);				/* 延时30ms */

	}
}

/* 线程4 */
void flag4_thread_entry( void *p_arg)
{
	for(;;)
	{
		flag4 = 1;
		rt_thread_delay(60);				/* 延时60ms */
		flag4 = 0;
		rt_thread_delay(60);				/* 延时60ms */

	}
}

/* 重定义系统滴答中断处理函数 */
void SysTick_Handler(void)
{
    /* 进入中断 */
    rt_interrupt_enter();

    rt_tick_increase();

    /* 离开中断 */
    rt_interrupt_leave();
}


int main(void)
{
	/* 硬件初始化 */
	/* 将硬件相关的初始化放在这里，如果是软件仿真则没有相关初始化代码 */
	
	/* 关中断，关就可以了，后面会自动开的 */
	rt_hw_interrupt_disable();
	
	/* 	TIPS：需要设置系统的核心时钟SystemCoreClock频率为25MHz，设置魔术棒Xtal的频率也为25Mhz！
			SysTick中断频率设置,这意味着 SysTick 定时器的计数器会从 250,000 开始计数，计数到 0 时产生一个中断。
			由于系统核心时钟频率为 25 MHz，这样配置后，SysTick 定时器将大约每 1/100 秒（10 毫秒）产生一次中断，符合每秒 100 次的时钟滴答要求。
			问题记录：因为最开始移植这个项目工程文件system_stm32f1xx.c中设置的系统核心时钟频率是16MHz，所以使得非阻塞延时时间为12ms，本来延时20ms，结果延时12ms。
			究其原因，在仿真的过程中，我在魔术棒target中设置的仿真器核心时钟就是25Mhz，
			如果软件设置为16Mhz，那么会提前到达重载值，然后提前完成中断，导致本来延时20ms，结果延时12ms。
			除了以上一种方法，还可以将魔术棒中的xTal设置为16Mhz，然后SystemCoreClock的频率也为16Mhz，就不会出错了。*/
	SysTick_Config( SystemCoreClock / RT_TICK_PER_SECOND);
	
	/* 系统定时器列表初始化 */
	rt_system_timer_init();
	
	/* 调度器初始化 */
	rt_system_scheduler_init();

  /* 初始化空闲线程 */    
  rt_thread_idle_init();	
	
	/* 初始化线程1 */
	rt_thread_init(	&rt_flag1_thread,
									"rt_flag1_thread",
									flag1_thread_entry,
									RT_NULL,
									&rt_flag1_thread_stack[0],
									sizeof(rt_flag1_thread_stack),
									4,
									1);
	/* 将线程插入到就绪列表 */
	//rt_list_insert_before(&(rt_thread_priority_table[0]),&(rt_flag1_thread.tlist));
	rt_thread_startup(&rt_flag1_thread);
									
	/* 初始化线程2 */
	rt_thread_init(	&rt_flag2_thread,
									"rt_flag2_thread",
									flag2_thread_entry,
									RT_NULL,
									&rt_flag2_thread_stack[0],
									sizeof(rt_flag2_thread_stack),
									4,
									3);
	rt_thread_startup(&rt_flag2_thread);
									
  /* 初始化线程3 */
	rt_thread_init(	&rt_flag3_thread,
									"rt_flag3_thread",
									flag3_thread_entry,
									RT_NULL,
									&rt_flag3_thread_stack[0],
									sizeof(rt_flag3_thread_stack),
									1,
									2);
	rt_thread_startup(&rt_flag3_thread);

	/* 初始化线程4 */
	rt_thread_init(	&rt_flag4_thread,
									"rt_flag4_thread",
									flag4_thread_entry,
									RT_NULL,
									&rt_flag4_thread_stack[0],
									sizeof(rt_flag4_thread_stack),
									2,
									2);
	rt_thread_startup(&rt_flag4_thread);			

									
	/* 启动系统调度器 */
	rt_system_scheduler_start(); 
	
	
}
