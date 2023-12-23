//#ifndef __KEY_H
//#define	__KEY_H


//#include "stm32f10x.h"

////  引脚定义
//#define    KEY1_GPIO_CLK     RCC_APB2Periph_GPIOE
//#define    KEY1_GPIO_PORT    GPIOE		   
//#define    KEY1_GPIO_PIN		 GPIO_Pin_4

//#define    KEY2_GPIO_CLK     RCC_APB2Periph_GPIOE
//#define    KEY2_GPIO_PORT    GPIOE		   
//#define    KEY2_GPIO_PIN		  GPIO_Pin_3


// /** 按键按下标置宏
//	*  按键按下为高电平，设置 KEY_ON=1， KEY_OFF=0
//	*  若按键按下为低电平，把宏设置成KEY_ON=0 ，KEY_OFF=1 即可
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
/* 引脚 定义 */

#define KEY0_GPIO_PORT                  GPIOE
#define KEY0_GPIO_PIN                   GPIO_Pin_4
#define KEY0_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);   /* PE口时钟使能 */

#define KEY1_GPIO_PORT                  GPIOE
#define KEY1_GPIO_PIN                   GPIO_Pin_3
#define KEY1_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOE,ENABLE);   /* PE口时钟使能 */

#define WKUP_GPIO_PORT                  GPIOA
#define WKUP_GPIO_PIN                   GPIO_Pin_0
#define WKUP_GPIO_CLK_ENABLE()          RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);   /* PA口时钟使能 */

/******************************************************************************************/

#define KEY0        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_4)     /* 读取KEY0引脚 */
#define KEY1        GPIO_ReadInputDataBit(GPIOE,GPIO_Pin_3)     /* 读取KEY1引脚 */
#define WK_UP       GPIO_ReadInputDataBit(WKUP_GPIO_PORT, WKUP_GPIO_PIN)     /* 读取WKUP引脚 */


#define KEY0_PRES    1              /* KEY0按下 */
#define KEY1_PRES    2              /* KEY1按下 */
#define WKUP_PRES    3              /* KEY_UP按下(即WK_UP) */

void Key_GPIO_Config(void);                /* 按键初始化函数 */
uint8_t key_scan(uint8_t mode);     /* 按键扫描函数 */

#endif
