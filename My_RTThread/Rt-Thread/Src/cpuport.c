#include "rtthread.h"

/* 全局变量（导入汇编文件context_rvds.s） */
/* 用于存储上一个线程的sp的指针 */
rt_uint32_t rt_interrupt_from_thread;
/* 用于存储下一个将要运行的线程的sp指针 */
rt_uint32_t rt_interrupt_to_thread;
/* PendSV终端服务函数执行的标志 */
rt_uint32_t rt_thread_switch_interrupt_flag;

struct exception_stack_frame
{
	/* 异常发生时，自动加载到CPU寄存器的内容 */
	rt_uint32_t r0;
	rt_uint32_t r1;
	rt_uint32_t r2;
	rt_uint32_t r3;
	rt_uint32_t r12;
	rt_uint32_t lr;
	rt_uint32_t pc;
	rt_uint32_t psr;
};

struct stack_frame
{
	/* 异常发生时，需要手动加载到CPU寄存器的内容 */
	rt_uint32_t r4;
	rt_uint32_t r5;
	rt_uint32_t r6;
	rt_uint32_t r7;
	rt_uint32_t r8;
	rt_uint32_t r9;
	rt_uint32_t r10;
	rt_uint32_t r11;
	
	struct exception_stack_frame exception_stack_frame;
};

/* hw是hardware硬件的意思，栈初始化，主要就是从上往下开辟了16个字的内存空间给R0-R15寄存器 */
rt_uint8_t *rt_hw_stack_init(void *tentry,
														 void *parameter,
														 rt_uint8_t *stack_addr)
{
	struct stack_frame 	*stack_frame;
	rt_uint8_t 					*stk;
	unsigned long				 i;
	
	/* 获取栈顶指针
			调用rt_hw_stack_init时，传给stack_addr的是（栈顶指针-4），所以需要再加上4个字节*/
	stk = stack_addr + sizeof(rt_uint32_t);
	
	/* 让stk指针向下8字节对齐，虽然通常使用4字节对齐就足够了，但是涉及到浮点运算就需要8字节对齐 */
	stk = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stk,8);
	
	/* stk指针继续向下移动sizeof(struct stack_frame)个偏移量，即16个字的大小，为的是给16个R0-R15寄存器开辟空间 */
	stk -= sizeof(struct stack_frame);
	
	/* 将stk指针强制转化成stack_frame类型后存储到stack_frame中，就是让stack_frame指向stk指向的位置，类似于stack_frame=stk */
	stack_frame = (struct stack_frame *)stk;
	
	/* 以stack_frame为起始地址，将栈空间里面的sizeof(struct stack_frame)个内存初始化为0Xdeadbeef，将开辟的16个字都赋值0xdeadbeef */
	for (i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i ++)
	{
			((rt_uint32_t *)stack_frame)[i] = 0xdeadbeef;
	}
	
	/* 在上一步的基础上，初始化异常发生时自动保存的寄存器,将自动保存的寄存器赋特殊值 */
	stack_frame->exception_stack_frame.r0 = (unsigned long)parameter;		/* r0 : argument */
	stack_frame->exception_stack_frame.r1 	= 0; 												/* r1 */
	stack_frame->exception_stack_frame.r2 	= 0; 												/* r2 */
	stack_frame->exception_stack_frame.r3 	= 0; 												/* r3 */
	stack_frame->exception_stack_frame.r12 	= 0; 												/* r12 */
	stack_frame->exception_stack_frame.lr 	= 0; 												/* lr : 暂时初始化为0 */
	stack_frame->exception_stack_frame.pc 	= (unsigned long)tentry; 		/* pc : 入口 */
	stack_frame->exception_stack_frame.psr 	= 0x01000000L; 							/* psr: 程序状态寄存器*/
	
	/* 返回线程栈指针 */
	return stk;
	
	
	
	
}							