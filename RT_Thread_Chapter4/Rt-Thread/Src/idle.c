#include <rtthread.h>
#include <rthw.h>

/* 空闲线程的栈 */
#define IDLE_THREAD_STACK_SIZE			512

/* 4字节对齐 */
ALIGN(RT_ALIGN_SIZE)

/* 我觉得定义为静态的栈的原因有——空闲线程的行为是可预测的（包括响应时间等），静态线程栈在整个程序的生命周期中都是存在的
	 并且，这个数组是我们文件（部门）专用的，其他文件（部门）不需要知道它*/
static rt_uint8_t rt_thread_stack[IDLE_THREAD_STACK_SIZE];

/* 空闲线程的线程控制块 */
struct rt_thread idle;

/* extern的一定要完整，要包含后面的[]下标 */
extern rt_list_t rt_thread_priority_table[RT_THREAD_PRIORITY_MAX];

rt_ubase_t rt_idletask_ctr = 0;

void rt_thread_idle_entry(void *parameter)
{
	/* 没有做任何实质性的操作。它可能是为了避免编译器警告关于未使用的参数，等待系统滴答的中断发生 */
	parameter = parameter;
	while(1)
	{
		rt_idletask_ctr ++;
	}
}



void rt_thread_idle_init(void)
{
	/* 初始化线程 */
	rt_thread_init(	&idle,
									"idle",
									rt_thread_idle_entry,
									RT_NULL,
									&rt_thread_stack[0],
									sizeof(rt_thread_stack),
									RT_THREAD_PRIORITY_MAX-1,
									2);
	
	/* 将线程插入到就绪列表 */
    //rt_list_insert_before( &(rt_thread_priority_table[RT_THREAD_PRIORITY_MAX-1]),&(idle.tlist) );
	rt_thread_startup(&idle);
}
