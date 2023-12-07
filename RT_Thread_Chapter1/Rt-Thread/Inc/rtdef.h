#ifndef __RT_DEF_H__
#define __RT_DEF_H__


//数据类型
/*	RTT 基础数据类型重定义  */
typedef signed 		char				rt_int8_t;
typedef signed 		short				rt_int16_t;
typedef signed 		long				rt_int32_t;
typedef unsigned 	char				rt_uint8_t;
typedef unsigned 	short				rt_uint16_t;
typedef unsigned 	long				rt_uint32_t;
/* 定义布尔类型（在C语言中通常用整型表示布尔值） */
typedef int										rt_bool_t;

/* 32位 CPU 特定类型定义 */
/* 用于地址或大小表示的基础类型（在32位处理器上为长整型） */
typedef long									rt_base_t;
/* 用于地址或大小表示的无符号基础类型 */
typedef unsigned long 				rt_ubase_t;
/* 错误类型，用于函数返回值 */
typedef rt_base_t 						rt_err_t;
/* 用于时间或时钟相关变量的类型 */
typedef	rt_uint32_t						rt_time_t;
/* 用于系统时钟滴答的类型 */
typedef	rt_uint32_t						rt_tick_t;
/* 标志类型，用于状态标记或条件标志 */
typedef	rt_base_t							rt_flag_t;
/* 表示大小的类型 */
typedef	rt_ubase_t						rt_size_t;
/* 设备相关的类型 */
typedef	rt_ubase_t						rt_dev_t;
/* 文件偏移量或相似概念的类型 */
typedef	rt_base_t							rt_off_t;

/* 布尔类型重定义 */
#define RT_TRUE								1
#define RT_FALSE							0

/* 定义错误码 */
#define RT_EOK								0		/* 无错误 */
#define RT_ERROR							1		/* 报错 */
#define RT_ETIMEOUT						2		/* 超时 */
#define RT_EFULL							3		/* 资源满 */
#define RT_EEMPTY							4		/* 资源空 */
#define RT_ENOMEM							5		/* 没有内存空间了 */
#define RT_ENOSYS							6		/* 非系统 */
#define RT_EBUSY							7		/* 忙 */
#define RT_EIO								8		/* IO错误 */
#define RT_EINTR							9		/* 中断系统调用 */
#define RT_EINVAL							10	/* 无效参数 */

/* 针对不同编译器的宏定义 */
/* 检查是否定义了 __CC_ARM，这通常表示使用的是 ARM Compiler */
#ifdef __CC_ARM
#define rt_inline							static __inline
#define	ALIGN(n)							__attribute__((aligned(n)))

/* 检查是否定义了 __IAR_SYSTEMS_ICC__，这表示使用的是 IAR Systems Compiler */
#elif defined (__IAR_SYSTEMS_ICC__)
#define rt_inline							static inline
#define	ALIGN(n)							PRAGMA(data_alignment=n)

/* 检查是否定义了 _GNUC_，这通常表示使用的是 GNU Compiler Collection (GCC) */
#elif defined	(_GNUC_)
#define rt_inline							static __inline
#define	ALIGN(n)							__attribute__((aligned(n)))
#else
#error not supported tool chain
#endif

/* 向上对齐：将size向上调整到最接近于align的倍数。是因为~（align-1）一定能够得到align的倍数，比如align是4 */
/* 当align是4时，align-1二进制为0011，~（align-1）二进制为1100（十进制12），只要这个二进制数的第三位是1、低位全为0，则这个数必然是4的倍数 */
/* ((size)+(align)-1)将size向上拉了一个段位，比如size是7的时候，他是在4-7这个区间的，加上3得到10之后就在8-11这个区间了，然后再& ~((align)-1) 就可以得到8，最终实现向上对齐*/
#define RT_ALIGN(size,align)	(((size)+(align)-1) & ~((align)-1))
/* 向下对齐：将size向下调整到最接近于align的倍数。同理，因为没有加上align-1，所以没有到上面一个区间，所以是向下对齐*/
#define RT_ALIGN_DOWN(size,align)					((size) & ~((align)-1))

#define RT_NULL 							(0)

/* 定义双向链表节点数据类型 */
struct rt_list_node
{
	struct rt_list_node *next;
	struct rt_list_node *prev;
};
typedef struct rt_list_node rt_list_t;

/* 线程控制块类型声明 */
struct rt_thread
{
	void *sp;									/* 线程栈指针 */
	void *entry;							/* 线程入口地址 */
	void *parameter;					/* 线程形参 */
	void *stack_addr;					/* 线程栈起始地址 */
	rt_uint32_t stack_size;		/* 线程栈大小，单位为字节Byte */
	
	rt_list_t tlist;					/* 线程链表节点 */
};
/* 定义线程控制块变量使用 struct rt_thread xxx */
/* 定义线程控制块指针使用 rt_thread_t      xxx */
typedef struct rt_thread *rt_thread_t;


#endif