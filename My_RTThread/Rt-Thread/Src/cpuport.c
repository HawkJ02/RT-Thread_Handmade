#include "rtthread.h"

/* ȫ�ֱ������������ļ�context_rvds.s�� */
/* ���ڴ洢��һ���̵߳�sp��ָ�� */
rt_uint32_t rt_interrupt_from_thread;
/* ���ڴ洢��һ����Ҫ���е��̵߳�spָ�� */
rt_uint32_t rt_interrupt_to_thread;
/* PendSV�ն˷�����ִ�еı�־ */
rt_uint32_t rt_thread_switch_interrupt_flag;

struct exception_stack_frame
{
	/* �쳣����ʱ���Զ����ص�CPU�Ĵ��������� */
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
	/* �쳣����ʱ����Ҫ�ֶ����ص�CPU�Ĵ��������� */
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

/* hw��hardwareӲ������˼��ջ��ʼ������Ҫ���Ǵ������¿�����16���ֵ��ڴ�ռ��R0-R15�Ĵ��� */
rt_uint8_t *rt_hw_stack_init(void *tentry,
														 void *parameter,
														 rt_uint8_t *stack_addr)
{
	struct stack_frame 	*stack_frame;
	rt_uint8_t 					*stk;
	unsigned long				 i;
	
	/* ��ȡջ��ָ��
			����rt_hw_stack_initʱ������stack_addr���ǣ�ջ��ָ��-4����������Ҫ�ټ���4���ֽ�*/
	stk = stack_addr + sizeof(rt_uint32_t);
	
	/* ��stkָ������8�ֽڶ��룬��Ȼͨ��ʹ��4�ֽڶ�����㹻�ˣ������漰�������������Ҫ8�ֽڶ��� */
	stk = (rt_uint8_t *)RT_ALIGN_DOWN((rt_uint32_t)stk,8);
	
	/* stkָ����������ƶ�sizeof(struct stack_frame)��ƫ��������16���ֵĴ�С��Ϊ���Ǹ�16��R0-R15�Ĵ������ٿռ� */
	stk -= sizeof(struct stack_frame);
	
	/* ��stkָ��ǿ��ת����stack_frame���ͺ�洢��stack_frame�У�������stack_frameָ��stkָ���λ�ã�������stack_frame=stk */
	stack_frame = (struct stack_frame *)stk;
	
	/* ��stack_frameΪ��ʼ��ַ����ջ�ռ������sizeof(struct stack_frame)���ڴ��ʼ��Ϊ0Xdeadbeef�������ٵ�16���ֶ���ֵ0xdeadbeef */
	for (i = 0; i < sizeof(struct stack_frame) / sizeof(rt_uint32_t); i ++)
	{
			((rt_uint32_t *)stack_frame)[i] = 0xdeadbeef;
	}
	
	/* ����һ���Ļ����ϣ���ʼ���쳣����ʱ�Զ�����ļĴ���,���Զ�����ļĴ���������ֵ */
	stack_frame->exception_stack_frame.r0 = (unsigned long)parameter;		/* r0 : argument */
	stack_frame->exception_stack_frame.r1 	= 0; 												/* r1 */
	stack_frame->exception_stack_frame.r2 	= 0; 												/* r2 */
	stack_frame->exception_stack_frame.r3 	= 0; 												/* r3 */
	stack_frame->exception_stack_frame.r12 	= 0; 												/* r12 */
	stack_frame->exception_stack_frame.lr 	= 0; 												/* lr : ��ʱ��ʼ��Ϊ0 */
	stack_frame->exception_stack_frame.pc 	= (unsigned long)tentry; 		/* pc : ��� */
	stack_frame->exception_stack_frame.psr 	= 0x01000000L; 							/* psr: ����״̬�Ĵ���*/
	
	/* �����߳�ջָ�� */
	return stk;
	
	
	
	
}							