#include <rtthread.h>
#include <rthw.h>

/**
 * �ú�����ָ���������ַ�����һ���ط�����������һ���ط�
 * ���������һ����Ҫ�ص�����ȷ�����ᳬ��Ŀ���ַ����ķ����С��
 * �Ӷ����⻺������������Դ�ַ�������С�� n��ʣ���Ŀ���ַ����ᱻ0��䡣�������ڷ�ֹδ��ʼ�����ڴ��ȡ��
 * @param dst �ַ���������Ŀ�ĵ�
 * @param src �ַ�������������ָ���ַ�������ָ��
 * @param n Ҫ��������󳤶�
 *
 * @return the result
 */
 char *rt_strncpy(char *dst, const char *src, rt_ubase_t n)
 {
	if(n != 0)
	{
		char *d = dst;
		const char *s = src;
		
		do
		{
			if((*d++ = *s++) == 0)
			{
				while(--n != 0)
					*d++ =0;
				break;
			}
		}
		while(--n != 0);
	}
	
	return (dst);
 }
