#ifndef	__RTTHREAD_CFG_H__
#define __RTTHREAD_CFG_H__

/* 字节对齐 */
#define RT_ALIGN_SIZE										4

/* 最大线程优先级 */
#define RT_THREAD_PRIORITY_MAX					32

/* 对象名字的最大长度 */
#define RT_NAME_MAX	            8      /* 内核对象的名字长度，字符串形式 */

/* 设定了每秒钟发生的时钟滴答数，100——10ms，1000——1ms*/
#define RT_TICK_PER_SECOND	    1000
#endif
