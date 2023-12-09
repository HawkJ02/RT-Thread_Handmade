#ifndef __RT_SERVICE_H__
#define __RT_SERVICE_H__

#include "rtdef.h"
#include "rtconfig.h"
#include "rtthread.h"

/* ��֪һ���ṹ���г�Ա�ĵ�ַ�����Ƴ��ýṹ����׵�ַ�������ã�member��ʵ�ʵ�ַ����ȥ������ýṹ����׵�ַ��0��
	 ��ָ��member�������ַ��Ҳ����member������׵�ַ��ƫ������ */
#define rt_container_of(ptr, type, member) \
    ((type *)((char *)(ptr) - (unsigned long)(&((type *)0)->member)))
			
#define rt_list_entry(node, type, member) \
    rt_container_of(node, type, member)
		
		

/* ��ʼ��һ��ָ�������˫������ڵ� */
rt_inline void rt_list_init(rt_list_t *l)
{
	l->next = l->prev = l;
}

/* ��˫������ı�ͷ�������һ���ڵ㣬��n���뵽l���� */
rt_inline void rt_list_insert_after(rt_list_t *l,rt_list_t *n)
{
	/* ������ԭʼ״̬�ڵ�2��ǰָ��ָ���½ڵ�n */
	l->next->prev = n;
	/* �������½ڵ�n�ĺ�ָ��ָ��ԭʼ״̬�ڵ�2 */
	n->next = l->next;
	/* Ȼ���ýڵ�1���½ڵ�n�໥ָ�� */
	l->next = n;
	n->prev = l;
}

/* ��˫������ı�ͷǰ�����һ���ڵ� */
rt_inline void rt_list_insert_before(rt_list_t *l,rt_list_t *n)
{
	l->prev->next = n;
	n->prev = l->prev;
	
	l->prev = n;
	n->next = l;
}

/* ��˫��������ɾ��һ���ڵ� */
rt_inline void rt_list_remove(rt_list_t *n)
{
	n->next->prev = n->prev;
	n->prev->next = n->next;
	
	n->next = n->prev = n;
}




#endif
