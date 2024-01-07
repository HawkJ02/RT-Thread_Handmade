//#ifndef __KEY_H
//#define	__KEY_H


//#include "stm32f10x.h"

////  ���Ŷ���
//#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOE
//#define    KEY1_GPIO_PORT    GPIOE		   
//#define    KEY1_GPIO_PIN		 GPIO_Pin_4

//#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOE
//#define    KEY2_GPIO_PORT    GPIOE		   
//#define    KEY2_GPIO_PIN		  GPIO_Pin_3


// /** �������±��ú�
//	*  ��������Ϊ�ߵ�ƽ������ KEY_ON=1�� KEY_OFF=0
//	*  ����������Ϊ�͵�ƽ���Ѻ����ó�KEY_ON=0 ��KEY_OFF=1 ����
//	*/
//#define KEY_ON	1
//#define KEY_OFF	0

//void Key_GPIO_Config(void);
//uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin);


//#endif /* __KEY_H */

#ifndef __KEY_H
#define __KEY_H

#include "stm32f10x.h"
/******************************************************************************************/
/* ���� ���� */

#define KEY0_GPIO_PORT                  GPIOE
#define KEY0_GPIO_PIN                   GPIO_Pin_4
#define KEY0_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);   /* PE��ʱ��ʹ�� */

#define KEY1_GPIO_PORT                  GPIOE
#define KEY1_GPIO_PIN                   GPIO_Pin_3
#define KEY1_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);   /* PE��ʱ��ʹ�� */

#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   GPIO_Pin_0
#define WKUP_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   /* PA��ʱ��ʹ�� */

/******************************************************************************************/

#define KEY0        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)     /* ��ȡKEY0���� */
#define KEY1        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)     /* ��ȡKEY1���� */
#define WK_UP       GPIO_ReadInputDataBit(WKUP_GPIO_PORT, WKUP_GPIO_PIN)     /* ��ȡWKUP���� */


#define KEY0_PRES    1              /* KEY0���� */
#define KEY1_PRES    2              /* KEY1���� */
#define WKUP_PRES    3              /* KEY_UP����(��WK_UP) */

void Key_GPIO_Config(void);                /* ������ʼ������ */
uint8_t key_scan(uint8_t mode);     /* ����ɨ�躯�� */

#endif
