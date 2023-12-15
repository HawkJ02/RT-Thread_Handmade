#include <rtthread.h>
#include <rthw.h>

/* 硬件定时器列表，数组只有一个成员，代表着只有一条链表 */
static rt_list_t rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL];

void rt_system_timer_init(void)
{
	int i;
	
	for(i = 0;i < sizeof(rt_timer_list) / sizeof(rt_timer_list[0]); i++)
	{
		/* 数组是连续的，元素可以通过+索引来找到 */
		rt_list_init(rt_timer_list + i);
	}
}

/* 内部用于初始化timer结构体的 */
static void _rt_timer_init(rt_timer_t timer,
                           void (*timeout)(void *parameter),
                           void      *parameter,
                           rt_tick_t  time,
                           rt_uint8_t flag)
{
    int i;

    /* 设置标志 */
    timer->parent.flag  = flag;

    /* 先设置为非激活态 */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    timer->timeout_func = timeout;
    timer->parameter    = parameter;

    /* 初始化 定时器实际超时时的系统节拍数 */
    timer->timeout_tick = 0;
    /* 初始化 定时器需要超时的节拍数 */
    timer->init_tick    = time;
		
    /* 初始化定时器的内置节点 */
    for (i = 0; i < RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        rt_list_init(&(timer->row[i]));
    }
}


/**
 * 该函数用于初始化一个定时器，通常该函数用于初始化一个静态的定时器
 *
 * @param timer 静态定时器对象
 * @param name 定时器的名字
 * @param timeout 超时函数
 * @param parameter 超时函数形参
 * @param time 定时器的超时时间
 * @param flag 定时器的标志
 */
void rt_timer_init(rt_timer_t  timer,
                   const char *name,
                   void (*timeout)(void *parameter),
                   void       *parameter,
                   rt_tick_t   time,
                   rt_uint8_t  flag)
{
    /* 定时器对象初始化 */
    rt_object_init((rt_object_t)timer, RT_Object_Class_Timer, name);

    /* 定时器初始化 */
    _rt_timer_init(timer, timeout, parameter, time, flag);
}

rt_inline void _rt_timer_remove(rt_timer_t timer)
{
    int i;

    for (i = 0; i < RT_TIMER_SKIP_LIST_LEVEL; i++)
    {
        rt_list_remove(&timer->row[i]);
    }
}

/**
 * 该函数将停止一个定时器
 *
 * @param timer 将要被停止的定时器
 *
 * @return 操作状态, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_timer_stop(rt_timer_t timer)
{
    register rt_base_t level;

    /* 只有active的定时器才能被停止，否则退出返回错误码 */
    if (!(timer->parent.flag & RT_TIMER_FLAG_ACTIVATED))
        return -RT_ERROR;

    /* 关中断 */
    level = rt_hw_interrupt_disable();

    /* 将定时器从定时器列表删除 */
    _rt_timer_remove(timer);

    /* 开中断 */
    rt_hw_interrupt_enable(level);

    /* 改变定时器的状态为非active */
    timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;

    return RT_EOK;
}


/**
 * 该函数将获取或者设置定时器的一些选项
 * 
 * @param timer 将要被设置或者获取的定时器
 * @param cmd 控制命令
 * @param arg 形参
 *
 * @return RT_EOK
 */
rt_err_t rt_timer_control(rt_timer_t timer, int cmd, void *arg)
{
	switch(cmd)
	{
		case RT_TIMER_CTRL_GET_TIME:
			*(rt_tick_t *)arg = timer->init_tick;
		
		case RT_TIMER_CTRL_SET_TIME:
			timer->init_tick = *(rt_tick_t *)arg;
		
		case RT_TIMER_CTRL_SET_ONESHOT:
			timer->parent.flag &= ~RT_TIMER_FLAG_PERIODIC;
		
		case RT_TIMER_CTRL_SET_PERIODIC:
        timer->parent.flag |= RT_TIMER_FLAG_PERIODIC;
        break;
	}
	return RT_EOK;
}

/**
 * 启动定时器，这个很关键
 *
 * @param timer 将要启动的定时器
 *
 * @return 操作状态, RT_EOK on OK, -RT_ERROR on error
 */
rt_err_t rt_timer_start(rt_timer_t timer)
{
	unsigned int row_lvl = 0;
	/* timer_list是一个局部变量，用于存储rt_timer_list */
	rt_list_t *timer_list;
	register rt_base_t level;
	/* row_head[RT_TIMER_SKIP_LIST_LEVEL]是一个局部变量，用于存储定时器链表数组 */
	rt_list_t *row_head[RT_TIMER_SKIP_LIST_LEVEL];
	unsigned int tst_nr;
	/* random_nr是一个静态变量，用于记录启动了多少个定时器 */
	static unsigned int random_nr;
	
	/* 常规操作——关中断 */
	level = rt_hw_interrupt_disable();
	
	/* 将定时器从系统定时器列表中移除 */
	_rt_timer_remove(timer);
	
	/* 改变定时器的状态为非active */
	timer->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
	
	/* 开中断 */
  rt_hw_interrupt_enable(level);
	
	/* 计算 timeout tick,定时器实际超时时的系统节拍数，等于当时的rt_tick+init_tick，
		 最大的timeout tick 不能大于 RT_TICK_MAX/2 */
	timer->timeout_tick = rt_tick_get() + timer->init_tick;
	
	/* 关中断 */
  level = rt_hw_interrupt_disable();
		
	/* timer_list是一个局部变量，获取系统定时器列表根节点地址，用于存储rt_timer_list（全局变量） */
	timer_list = rt_timer_list;
	
	/* 获取系统定时器列表第一条链表根节点地址 */
	row_head[0] = &timer_list[0];
	
	 /* 遍历定时器链表数组的所有层，因为RT_TIMER_SKIP_LIST_LEVEL等于1，这个循环只会执行一次 */
	for(row_lvl = 0;row_lvl < RT_TIMER_SKIP_LIST_LEVEL;row_lvl++)
	{
		/* 在本层的双向链表中向后搜索，按照升序插入。
			 for循环中中的条件是：当还没有遍历到最后一个节点，即timer_list[row_lvl].prev，
			 每次都会移到下一个节点。列表不为空，当没有定时器被插入到系统定时器列表时，该循环不执行 */
		for(;row_head[row_lvl] != timer_list[row_lvl].prev;row_head[row_lvl] = row_head[row_lvl]->next)
		{
			struct rt_timer *t;
			
			/* 获取定时器列表节点地址 */
			rt_list_t *p = row_head[row_lvl]->next;
			
			t = rt_list_entry(p,
												struct rt_timer,
												row[row_lvl]);
												
			if((t->timeout_tick - timer->timeout_tick) == 0)
			{
				continue;
			}
			else if((t->timeout_tick - timer->timeout_tick) < RT_TICK_MAX / 2)
			{
				break;
			}
		}
		/* 检查当前层级是否不是最后一层。
			 如果不是，则将下一层级的头节点设置为当前层级头节点的下一个节点。
			 条件不会成真，不会被执行 */
    if (row_lvl != RT_TIMER_SKIP_LIST_LEVEL - 1)
    {
			row_head[row_lvl + 1] = row_head[row_lvl] + 1; 
    }  
		/* random_nr是一个静态变量，用于记录启动了多少个定时器。
			 TIPs:局部静态变量，在不断进入该函数的过程中不会改变。 */
		random_nr++;
		tst_nr = random_nr;
		
		/* 将定时器插入到系统定时器列表 */
		rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL - 1],       /* 双向列表根节点地址 */
                         &(timer->row[RT_TIMER_SKIP_LIST_LEVEL - 1])); /* 要被插入的节点的地址 */
		
		/* 功能：将 timer 插入到 Skip List 的不同层级链表中
			 从定时器链表的第二层开始，如果目前已经打开的定时器没有被掩码RT_TIMER_SKIP_LIST_MASK遮挡，
			 就将 timer 插入到 skip list 中的当前层级链表的头节点之后，
			 如果等于掩码则跳出，进入下一个循环
			 RT_TIMER_SKIP_LIST_LEVEL 等于1，该for循环永远不会执行 */
    for (row_lvl = 2; row_lvl <= RT_TIMER_SKIP_LIST_LEVEL; row_lvl++)
    {
        if (!(tst_nr & RT_TIMER_SKIP_LIST_MASK))
            rt_list_insert_after(row_head[RT_TIMER_SKIP_LIST_LEVEL - row_lvl],
                                 &(timer->row[RT_TIMER_SKIP_LIST_LEVEL - row_lvl]));
        else
            break;

        tst_nr >>= (RT_TIMER_SKIP_LIST_MASK + 1) >> 1;
    }
		
		/* 设置定时器标志位为激活态 */
    timer->parent.flag |= RT_TIMER_FLAG_ACTIVATED;

    /* 开中断 */
    rt_hw_interrupt_enable(level);

	}
	 return -RT_EOK;
}

/**
 * 该函数用于扫描系统定时器列表，当有超时事件发生时
 * 就调用对应的超时函数
 *
 * @note 该函数在操作系统定时器中断中被调用
 */
void rt_timer_check(void)
{
	struct rt_timer *t;
	rt_tick_t current_tick;
	register rt_base_t level;
	
	/* 获取系统时基计数器rt_tick的值 */
	current_tick = rt_tick_get();
	
	/* 关中断 */
  level = rt_hw_interrupt_disable();
	
	/* 系统定时器列表不为空，则扫描定时器列表 */
	while(!rt_list_isempty(&rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL - 1]))
	{
		/* 获取第一个节点定时器的地址 */
    t = rt_list_entry(rt_timer_list[RT_TIMER_SKIP_LIST_LEVEL - 1].next,   /* 节点地址 */
											struct rt_timer,                                    /* 节点所在的父结构的数据类型 */ 
											row[RT_TIMER_SKIP_LIST_LEVEL - 1]);                 /* 节点在父结构的成员名 */
		
		if((current_tick - t->timeout_tick) < RT_TICK_MAX / 2)
		{
			/* 先将定时器从定时器列表移除 */
			_rt_timer_remove(t);
			
			/* 调用超时函数 */
			t->timeout_func(t->parameter);
			
			current_tick = rt_tick_get();
			
			if((t->parent.flag & RT_TIMER_FLAG_PERIODIC) && (t->parent.flag & RT_TIMER_FLAG_ACTIVATED))
			{
				/* 先失活，等下面的start会flag激活 */
				t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
				/* 启动定时器 */
				rt_timer_start(t);
			}
			/* 单次定时器 */
			else
			{
				/* 停止定时器 */
        t->parent.flag &= ~RT_TIMER_FLAG_ACTIVATED;
			}
		}
		else
			break;
	
	}
	
	/* 开中断 */
  rt_hw_interrupt_enable(level);
	
}
