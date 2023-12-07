#ifndef __RT_THREAD_H__
#define __RT_THREAD_H__

#include "rtdef.h"
#include "rtconfig.h"
#include "rtservice.h"
#include "rthw.h"

rt_err_t rt_thread_init(struct rt_thread *thread,
												void (*entry)(void *parameter),
												void *parameter,
												void *stack_start,
												rt_uint32_t stack_size);

												
/* ��ʼ��ϵͳ������ */
void rt_system_scheduler_init(void);
/* ����ϵͳ������ */
void rt_system_scheduler_start(void);
/* �ֶ������л� */										
void rt_schedule(void);
#endif