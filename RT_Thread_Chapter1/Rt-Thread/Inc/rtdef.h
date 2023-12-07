#ifndef __RT_DEF_H__
#define __RT_DEF_H__


//��������
/*	RTT �������������ض���  */
typedef signed 		char				rt_int8_t;
typedef signed 		short				rt_int16_t;
typedef signed 		long				rt_int32_t;
typedef unsigned 	char				rt_uint8_t;
typedef unsigned 	short				rt_uint16_t;
typedef unsigned 	long				rt_uint32_t;
/* ���岼�����ͣ���C������ͨ�������ͱ�ʾ����ֵ�� */
typedef int										rt_bool_t;

/* 32λ CPU �ض����Ͷ��� */
/* ���ڵ�ַ���С��ʾ�Ļ������ͣ���32λ��������Ϊ�����ͣ� */
typedef long									rt_base_t;
/* ���ڵ�ַ���С��ʾ���޷��Ż������� */
typedef unsigned long 				rt_ubase_t;
/* �������ͣ����ں�������ֵ */
typedef rt_base_t 						rt_err_t;
/* ����ʱ���ʱ����ر��������� */
typedef	rt_uint32_t						rt_time_t;
/* ����ϵͳʱ�ӵδ������ */
typedef	rt_uint32_t						rt_tick_t;
/* ��־���ͣ�����״̬��ǻ�������־ */
typedef	rt_base_t							rt_flag_t;
/* ��ʾ��С������ */
typedef	rt_ubase_t						rt_size_t;
/* �豸��ص����� */
typedef	rt_ubase_t						rt_dev_t;
/* �ļ�ƫ���������Ƹ�������� */
typedef	rt_base_t							rt_off_t;

/* ���������ض��� */
#define RT_TRUE								1
#define RT_FALSE							0

/* ��������� */
#define RT_EOK								0		/* �޴��� */
#define RT_ERROR							1		/* ���� */
#define RT_ETIMEOUT						2		/* ��ʱ */
#define RT_EFULL							3		/* ��Դ�� */
#define RT_EEMPTY							4		/* ��Դ�� */
#define RT_ENOMEM							5		/* û���ڴ�ռ��� */
#define RT_ENOSYS							6		/* ��ϵͳ */
#define RT_EBUSY							7		/* æ */
#define RT_EIO								8		/* IO���� */
#define RT_EINTR							9		/* �ж�ϵͳ���� */
#define RT_EINVAL							10	/* ��Ч���� */

/* ��Բ�ͬ�������ĺ궨�� */
/* ����Ƿ����� __CC_ARM����ͨ����ʾʹ�õ��� ARM Compiler */
#ifdef __CC_ARM
#define rt_inline							static __inline
#define	ALIGN(n)							__attribute__((aligned(n)))

/* ����Ƿ����� __IAR_SYSTEMS_ICC__�����ʾʹ�õ��� IAR Systems Compiler */
#elif defined (__IAR_SYSTEMS_ICC__)
#define rt_inline							static inline
#define	ALIGN(n)							PRAGMA(data_alignment=n)

/* ����Ƿ����� _GNUC_����ͨ����ʾʹ�õ��� GNU Compiler Collection (GCC) */
#elif defined	(_GNUC_)
#define rt_inline							static __inline
#define	ALIGN(n)							__attribute__((aligned(n)))
#else
#error not supported tool chain
#endif

/* ���϶��룺��size���ϵ�������ӽ���align�ı���������Ϊ~��align-1��һ���ܹ��õ�align�ı���������align��4 */
/* ��align��4ʱ��align-1������Ϊ0011��~��align-1��������Ϊ1100��ʮ����12����ֻҪ������������ĵ���λ��1����λȫΪ0�����������Ȼ��4�ı��� */
/* ((size)+(align)-1)��size��������һ����λ������size��7��ʱ��������4-7�������ģ�����3�õ�10֮�����8-11��������ˣ�Ȼ����& ~((align)-1) �Ϳ��Եõ�8������ʵ�����϶���*/
#define RT_ALIGN(size,align)	(((size)+(align)-1) & ~((align)-1))
/* ���¶��룺��size���µ�������ӽ���align�ı�����ͬ����Ϊû�м���align-1������û�е�����һ�����䣬���������¶���*/
#define RT_ALIGN_DOWN(size,align)					((size) & ~((align)-1))

#define RT_NULL 							(0)

/* ����˫������ڵ��������� */
struct rt_list_node
{
	struct rt_list_node *next;
	struct rt_list_node *prev;
};
typedef struct rt_list_node rt_list_t;

/* �߳̿��ƿ��������� */
struct rt_thread
{
	void *sp;									/* �߳�ջָ�� */
	void *entry;							/* �߳���ڵ�ַ */
	void *parameter;					/* �߳��β� */
	void *stack_addr;					/* �߳�ջ��ʼ��ַ */
	rt_uint32_t stack_size;		/* �߳�ջ��С����λΪ�ֽ�Byte */
	
	rt_list_t tlist;					/* �߳�����ڵ� */
};
/* �����߳̿��ƿ����ʹ�� struct rt_thread xxx */
/* �����߳̿��ƿ�ָ��ʹ�� rt_thread_t      xxx */
typedef struct rt_thread *rt_thread_t;


#endif