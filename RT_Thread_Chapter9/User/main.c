/**
  ******************************************************************************
  * @file    main.c
  * @author  HawkJ
  * @version V1.0
  * @date    2023-12-16
  * @brief   移植RT_Thread
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
#include "bsp_key.h"


/*
*************************************************************************
*                               变量
*************************************************************************
*/
// 定义消息结构体
struct msg
{
    rt_uint8_t *data_ptr;  // 指向数据块的指针
    rt_uint32_t data_size; // 数据块的长度
};

/* 定义全局变量 */
char buffer0[] = "this is KEY0 mail !";
char buffer1[] = "this is KEY1 mail !";
char buffer_timer[] = "this is buffer mail !";
/* 定义线程控制块指针 */
static rt_thread_t send_mb_thread = RT_NULL;
static rt_thread_t recv_mb_thread = RT_NULL;


/* 定义互斥量控制块指针 */
static rt_mailbox_t key_mail = RT_NULL;

/* 定义定时器控制块指针 */
static rt_timer_t swtimer0 = RT_NULL;
/*
*************************************************************************
*                             线程定义
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
						rt_kprintf("KEY0按下！邮件发送成功\n");
					}
					else
					{
						rt_kprintf("KEY0按下！邮件发送失败\n");
					}
					break;
				
				case KEY1_PRES:
					uwRet = rt_mb_send(key_mail,(rt_uint32_t)&buffer1);
					if(uwRet == RT_EOK)
					{
						rt_kprintf("KEY1按下！邮件发送成功\n");
					}
					else
					{
						rt_kprintf("KEY1按下！邮件发送失败\n");
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
		/* 这边第二个参数需要特别注意，因为在消息邮箱存储的是邮件的地址，
			 而接收函数中存在着 *value = mb->msg_pool[mb->out_offset]; 
			 意思就是传入的指针指向的位置将会存储邮件的地址，
			 我们按照以下形式传入就会变成：*(&recv_ptr) = recv_ptr = mb->msg_pool[mb->out_offset]; 这样才是正确的
			 最后传入recv_ptr到printf中，也就是字符串的第一个字符的地址，就可以打印所在位置的数据了*/
//		uwRet = rt_mb_recv(key_mail,(rt_uint32_t *)&recv_ptr,RT_WAITING_FOREVER);
//		if(uwRet == RT_EOK)
//		{
//			rt_kprintf("邮箱的内容是%s !\n\n",recv_ptr);
//			LED1_TOGGLE;
//		}
//		else
//		{
//			rt_kprintf("邮箱接受错误！错误码是0x%x\n",uwRet);
//		}	
		
    // 从消息邮箱中接收消息指针
    if (rt_mb_recv(key_mail, (rt_uint32_t*)&received_msg_ptr,RT_WAITING_FOREVER) == RT_EOK)
    {
        // "Received message: %.*s\n" 是格式化字符串，其中 %.*s 是格式控制符。
				//在这里，%.*s 用来输出长度可变的字符串，前面的 .* 会被后面传入的参数替换。
				//具体来说，%.*s 中的 * 表示长度由后面的参数指定，而 s 表示参数是一个字符串。
        rt_kprintf("Received message: %.*s\n", received_msg_ptr->data_size, received_msg_ptr->data_ptr);

        // 释放内存
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
						rt_kprintf("软件定时器0！邮件发送成功! \n");
						LED2_TOGGLE;
					}
					else
					{
						rt_kprintf("软件定时器0！邮件发送失败! \n");
					}
					break;
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
	// 在发送线程中
  struct msg* msg_ptr;
	// 假设有一段数据需要发送
	char data[] = "Hello, Embedded Systems!";
	int data_length = sizeof(data);
	/* 输出版本信息 */
	rt_show_version();
	
	rt_kprintf("这是RTT邮箱实验！\n");
	
	LED1_ON;
	LED2_ON;
	 /* 
	 * 开发板硬件初始化，RTT系统初始化已经在main函数之前完成，
	 * 即在component.c文件中的rtthread_startup()函数中完成了。
	 * 所以在main函数中，只需要创建线程和启动线程即可。
	 */
	/* 创建邮箱 */
	key_mail = rt_mb_create("key_mail",10,RT_IPC_FLAG_FIFO);
	if(key_mail != RT_NULL)
	{
		rt_kprintf("邮箱创建成功！\n");
	}
	else
	{
		rt_kprintf("邮箱创建失败！\n");
	}
	
//	swtimer0 = rt_timer_create("swtimer0",swtimer0_callback,0,1000,RT_TIMER_FLAG_PERIODIC | RT_TIMER_FLAG_SOFT_TIMER);
//	if(swtimer0 != RT_NULL)
//	{
//		rt_timer_start(swtimer0);
//		rt_kprintf("软件定时器0创建成功！已启动！ \n");
//	}
//	else
//	{
//		rt_kprintf("软件定时器0创建失败！\n");
//	}
//	
//	
//	/* 创建按键触发事件发送线程 */
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
	/* 创建事件等待线程，可以是一个需要重重初始化之后然后启动的重要设备 */
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
	 
    // 分配消息结构体内存
    msg_ptr = (struct msg*)rt_malloc(sizeof(struct msg));


    // 填充消息结构体
    msg_ptr->data_ptr = (rt_uint8_t*)data;
    msg_ptr->data_size = data_length;
		
		// 发送消息指针给消息邮箱
    rt_kprintf("Sending message...\n");
    rt_mb_send(key_mail, (rt_uint32_t)msg_ptr);
}

void Delay(__IO uint32_t nCount)	 //简单的延时函数
{
	for(; nCount != 0; nCount--);
}
/*********************************************END OF FILE**********************/
