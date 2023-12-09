#include <rtthread.h>
#include <rthw.h>

/**
 * 该函数将指定个数的字符串从一个地方拷贝到另外一个地方
 * 这个函数的一个重要特点是它确保不会超过目标字符串的分配大小，
 * 从而避免缓冲区溢出。如果源字符串长度小于 n，剩余的目标字符串会被0填充。这有助于防止未初始化的内存读取。
 * @param dst 字符串拷贝的目的地
 * @param src 字符串从哪里拷贝，指向字符常量的指针
 * @param n 要拷贝的最大长度
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
