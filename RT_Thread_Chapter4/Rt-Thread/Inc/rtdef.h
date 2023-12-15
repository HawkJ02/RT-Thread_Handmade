#ifndef __RT_DEF_H__
#define __RT_DEF_H__

#include "rtconfig.h"

/*
*************************************************************************
*                               ��������
*************************************************************************
*/

/*	RTT �������������ض���  */
typedef signed 		char				rt_int8_t;
typedef signed 		short				rt_int16_t;
typedef signed 		long				rt_int32_t;
typedef unsigned 	char				rt_uint8_t;
typedef unsigned 	short				rt_uint16_t;
typedef unsigned 	long				rt_uint32_t;
/* ���岼�����ͣ���C������ͨ�������ͱ�ʾ����ֵ�� */
typedef int										rt_bool_t;

/* �������͵����ֵ */
#define RT_UINT8_MAX                    0xff            /**< Maxium number of UINT8 */
#define RT_UINT16_MAX                   0xffff          /**< Maxium number of UINT16 */
#define RT_UINT32_MAX                   0xffffffff      /**< Maxium number of UINT32 */
#define RT_TICK_MAX                     RT_UINT32_MAX   /**< Maxium number of tick */

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

/*
*************************************************************************
*                               �����붨��
*************************************************************************
*/
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


/*
*************************************************************************
*                               ˫������ṹ��
*************************************************************************
*/
/* ����˫������ڵ��������� */
struct rt_list_node
{
	struct rt_list_node *next;
	struct rt_list_node *prev;
};
typedef struct rt_list_node rt_list_t;

/*
*************************************************************************
*                               �ں˶���ṹ��
*************************************************************************
*/
/**
 * ��������������ĺ���ʹ�ܣ���Щ��ͨ����rtconfig.h�ж���
 *  - Thread
 *  - Semaphore
 *  - Mutex
 *  - Event
 *  - MailBox
 *  - MessageQueue
 *  - MemHeap
 *  - MemPool
 *  - Device
 *  - Timer
 *  - Module
 *  - Unknown
 *  - Static
 */
enum rt_object_class_type
{
     RT_Object_Class_Thread = 0,       /* �������߳� */
     RT_Object_Class_Semaphore,        /* �������ź��� */
     RT_Object_Class_Mutex,            /* �����ǻ����� */
     RT_Object_Class_Event,            /* �������¼� */
     RT_Object_Class_MailBox,          /* ���������� */
     RT_Object_Class_MessageQueue,     /* ��������Ϣ���� */
     RT_Object_Class_MemHeap,          /* �������ڴ�� */
     RT_Object_Class_MemPool,          /* �������ڴ�� */
     RT_Object_Class_Device,           /* �������豸 */
     RT_Object_Class_Timer,            /* �����Ƕ�ʱ�� */
     RT_Object_Class_Module,           /* ������ģ�� */
     RT_Object_Class_Unknown,          /* ����δ֪������Ĵ�С���Զ���������Ϊһ���ο���������ö�ٵ�RT_Object_Class_Unknown��������ǰ�涨��Ķ���ĸ��� */
     RT_Object_Class_Static = 0x80     /* �����Ǿ�̬���� */
};

/**
 * �ں˶���������ݽṹ
 */
struct rt_object
{
    char       name[RT_NAME_MAX];                       /* �ں˶�������� */
    rt_uint8_t type;                                    /* �ں˶�������� */
    rt_uint8_t flag;                                    /* �ں˶����״̬ */

    rt_list_t  list;                                    /* �ں˶�����б�ڵ� */
};
typedef struct rt_object *rt_object_t;                  /*�ں˶������������ض��� */

/**
 * �ں˶�����Ϣ�ṹ��
 */
struct rt_object_information
{
	enum rt_object_class_type type;				/* �������� */
	rt_list_t object_list;								/* �����б�ڵ�ͷ */
	rt_size_t object_size;								/* �����С */
	
};


/*
*************************************************************************
*                              ʱ�� & ��ʱ��
*************************************************************************
*/

/**
 * ʱ�� & ��ʱ�� �궨�� ��ʱ����flag
 */
#define RT_TIMER_FLAG_DEACTIVATED       0x0     /* ��ʱ��û�м��� */
#define RT_TIMER_FLAG_ACTIVATED         0x1     /* ��ʱ���Ѿ����� */
#define RT_TIMER_FLAG_ONE_SHOT          0x0     /* ���ζ�ʱ */
#define RT_TIMER_FLAG_PERIODIC          0x2     /* ���ڶ�ʱ */

#define RT_TIMER_FLAG_HARD_TIMER        0x0     /* Ӳ����ʱ������ʱ���ص������� tick isr�е��� */

#define RT_TIMER_FLAG_SOFT_TIMER        0x4     /* �����ʱ������ʱ���ص������ڶ�ʱ���߳��е��� */


#define RT_TIMER_CTRL_SET_TIME          0x0     /* ���ö�ʱ����ʱʱ�� */
#define RT_TIMER_CTRL_GET_TIME          0x1     /* ��ȡ��ʱ����ʱʱ�� */
#define RT_TIMER_CTRL_SET_ONESHOT       0x2     /* �޸Ķ�ʱ��Ϊһ�ζ�ʱ */
#define RT_TIMER_CTRL_SET_PERIODIC      0x3     /* �޸Ķ�ʱ��Ϊ���ڶ�ʱ */

#ifndef RT_TIMER_SKIP_LIST_LEVEL
#define RT_TIMER_SKIP_LIST_LEVEL          1
#endif

/* 1 or 3 */
#ifndef RT_TIMER_SKIP_LIST_MASK
#define RT_TIMER_SKIP_LIST_MASK         0x3
#endif


/* ��ʱ���ṹ�� */
struct rt_timer
{
    struct rt_object parent;                       	  /* �� rt_object �̳� */

    rt_list_t row[RT_TIMER_SKIP_LIST_LEVEL];					/* ���ڸ����Ķ�ʱ�� timer��ͨ�� row[i] ��ȡ�ڵ� i �㼶�ϵ�����ڵ� */
    void (*timeout_func)(void *parameter);          	/* ��ʱ���� */
    void            *parameter;                     	/* ��ʱ�����β� */
	
    rt_tick_t        init_tick;                     	/* ��ʱ��ʵ����Ҫ��ʱ��ʱ�� */
    rt_tick_t        timeout_tick;                   	/* ��ʱ��ʵ�ʳ�ʱ��ϵͳ������ */
};
typedef struct rt_timer *rt_timer_t;


/*
*************************************************************************
*                               �߳̽ṹ��
*************************************************************************
*/

struct rt_thread
{
    /* rt ���� */
    char        name[RT_NAME_MAX];    /* ��������� */
    rt_uint8_t  type;                 /* �������� */
    rt_uint8_t  flags;                /* �����״̬ */
    rt_list_t   list;                 /* ������б�ڵ� */
    
	rt_list_t   tlist;               	 	/* �߳�����ڵ� */
    
	void        *sp;	              		/* �߳�ջָ�� */
	void        *entry;	              	/* �߳���ڵ�ַ */
	void        *parameter;	          	/* �߳��β� */	
	void        *stack_addr;          	/* �߳���ʼ��ַ */
	rt_uint32_t stack_size;           	/* �߳�ջ��С����λΪ�ֽ� */
	
	rt_ubase_t  init_tick;            	/* ��ʼʱ��Ƭ */
  rt_ubase_t  remaining_tick;       	/* ʣ��ʱ��Ƭ */ 
	
	rt_uint8_t 	current_priority;				/* ��ǰ���ȼ� */
	rt_uint8_t 	init_priority;					/* ��ʼ���ȼ� */
	rt_uint32_t	number_mask;						/* ��ǰ���ȼ����� */
	
	rt_err_t		error;									/* ������ */
	rt_uint8_t	stat;										/* �̵߳�״̬ */
	
	struct rt_timer thread_timer;				/* ���õ��̶߳�ʱ�� */
};
typedef struct rt_thread *rt_thread_t;

/*
 * �߳�״̬����
 */
#define RT_THREAD_INIT                  0x00                /* ��ʼ̬ */
#define RT_THREAD_READY                 0x01                /* ����̬ */
#define RT_THREAD_SUSPEND               0x02                /* ����̬ */
#define RT_THREAD_RUNNING               0x03                /* ����̬ */
#define RT_THREAD_BLOCK                 RT_THREAD_SUSPEND   /* ����̬ */
#define RT_THREAD_CLOSE                 0x04                /* �ر�̬ */
#define RT_THREAD_STAT_MASK             0x0f

#define RT_THREAD_STAT_SIGNAL           0x10
#define RT_THREAD_STAT_SIGNAL_READY     (RT_THREAD_STAT_SIGNAL | RT_THREAD_READY)
#define RT_THREAD_STAT_SIGNAL_SUSPEND   0x20
#define RT_THREAD_STAT_SIGNAL_MASK      0xf0


#endif
