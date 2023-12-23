///**
//  ******************************************************************************
//  * @file    bsp_key.c
//  * @author  fire
//  * @version V1.0
//  * @date    2013-xx-xx
//  * @brief   按键应用bsp（扫描模式）
//  ******************************************************************************
//  * @attention
//  *
//  * 实验平台:秉火 F103-霸道 STM32 开发板 
//  * 论坛    :http://www.firebbs.cn
//  * 淘宝    :http://firestm32.taobao.com
//  *
//  ******************************************************************************
//  */ 
//  
//#include "bsp_key.h"  

///**
//  * @brief  配置按键用到的I/O口
//  * @param  无
//  * @retval 无
//  */
//void Key_GPIO_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	/*开启按键端口的时钟*/
//	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
//	
//	//选择按键的引脚
//	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
//	// 设置按键的引脚为浮空输入
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//	//使用结构体初始化按键
//	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
//	
//	//选择按键的引脚
//	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
//	//设置按键的引脚为浮空输入
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//	//使用结构体初始化按键
//	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
//}

// /*
// * 函数名：Key_Scan
// * 描述  ：检测是否有按键按下
// * 输入  ：GPIOx：x 可以是 A，B，C，D或者 E
// *		     GPIO_Pin：待读取的端口位 	
// * 输出  ：KEY_OFF(没按下按键)、KEY_ON（按下按键）
// */
//uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
//{			
//	/*检测是否有按键按下 */
//	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
//	{	 
//		/*等待按键释放 */
//		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
//		return 	KEY_ON;	 
//	}
//	else
//		return KEY_OFF;
//}
///*********************************************END OF FILE**********************/





#include "bsp_key.h"


/**
 * @brief       按键初始化函数
 * @param       无
 * @retval      无
 */
void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    KEY0_GPIO_CLK_ENABLE();                                     /* KEY0时钟使能 */
    KEY1_GPIO_CLK_ENABLE();                                     /* KEY1时钟使能 */
    WKUP_GPIO_CLK_ENABLE();                                     /* WKUP时钟使能 */
		
    //选择按键的引脚
		GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN; 
		// 设置按键的引脚为浮空输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		//使用结构体初始化按键
		GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);

		//选择按键的引脚
		GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
		//设置按键的引脚为浮空输入
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		//使用结构体初始化按键
		GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);	


}

/**
 * @brief       按键扫描函数
 * @note        该函数有响应优先级(同时按下多个按键): WK_UP > KEY1 > KEY0!!
 * @param       mode:0 / 1, 具体含义如下:
 *   @arg       0,  不支持连续按(当按键按下不放时, 只有第一次调用会返回键值,
 *                  必须松开以后, 再次按下才会返回其他键值)
 *   @arg       1,  支持连续按(当按键按下不放时, 每次调用该函数都会返回键值)
 * @retval      键值, 定义如下:
 *              KEY0_PRES, 1, KEY0按下
 *              KEY1_PRES, 2, KEY1按下
 *              WKUP_PRES, 3, WKUP按下
 */
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;  /* 按键按松开标志 */
    uint8_t keyval = 0;
    if (mode) key_up = 1;       /* 支持连按 */

    if (key_up && (KEY0 == 0 || KEY1 == 0 ))  /* 按键松开标志为1, 且有任意一个按键按下了 */
    {		
        key_up = 0;

        if (KEY0 == 0)  keyval = KEY0_PRES;

        if (KEY1 == 0)  keyval = KEY1_PRES;

    }
    else if (KEY0 == 1 && KEY1 == 1 ) /* 没有任何按键按下, 标记按键松开 */
    {
        key_up = 1;
    }

    return keyval;              /* 返回键值 */
}
