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

												
/* 初始化系统调度器 */
void rt_system_scheduler_init(void);
/* 启动系统调度器 */
void rt_system_scheduler_start(void);
/* 手动任务切换 */										
void rt_schedule(void);
#endif