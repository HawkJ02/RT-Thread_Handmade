///**
//  ******************************************************************************
//  * @file    bsp_key.c
//  * @author  fire
//  * @version V1.0
//  * @date    2013-xx-xx
//  * @brief   ����Ӧ��bsp��ɨ��ģʽ��
//  ******************************************************************************
//  * @attention
//  *
//  * ʵ��ƽ̨:���� F103-�Ե� STM32 ������ 
//  * ��̳    :http://www.firebbs.cn
//  * �Ա�    :http://firestm32.taobao.com
//  *
//  ******************************************************************************
//  */ 
//  
//#include "bsp_key.h"  

///**
//  * @brief  ���ð����õ���I/O��
//  * @param  ��
//  * @retval ��
//  */
//void Key_GPIO_Config(void)
//{
//	GPIO_InitTypeDef GPIO_InitStructure;
//	
//	/*���������˿ڵ�ʱ��*/
//	RCC_APB2PeriphClockCmd(KEY1_GPIO_CLK|KEY2_GPIO_CLK,ENABLE);
//	
//	//ѡ�񰴼�������
//	GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
//	// ���ð���������Ϊ��������
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//	//ʹ�ýṹ���ʼ������
//	GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);
//	
//	//ѡ�񰴼�������
//	GPIO_InitStructure.GPIO_Pin = KEY2_GPIO_PIN; 
//	//���ð���������Ϊ��������
//	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
//	//ʹ�ýṹ���ʼ������
//	GPIO_Init(KEY2_GPIO_PORT, &GPIO_InitStructure);	
//}

// /*
// * ��������Key_Scan
// * ����  ������Ƿ��а�������
// * ����  ��GPIOx��x ������ A��B��C��D���� E
// *		     GPIO_Pin������ȡ�Ķ˿�λ 	
// * ���  ��KEY_OFF(û���°���)��KEY_ON�����°�����
// */
//uint8_t Key_Scan(GPIO_TypeDef* GPIOx,uint16_t GPIO_Pin)
//{			
//	/*����Ƿ��а������� */
//	if(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON )  
//	{	 
//		/*�ȴ������ͷ� */
//		while(GPIO_ReadInputDataBit(GPIOx,GPIO_Pin) == KEY_ON);   
//		return 	KEY_ON;	 
//	}
//	else
//		return KEY_OFF;
//}
///*********************************************END OF FILE**********************/





#include "bsp_key.h"


/**
 * @brief       ������ʼ������
 * @param       ��
 * @retval      ��
 */
void Key_GPIO_Config(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
	
    KEY0_GPIO_CLK_ENABLE();                                     /* KEY0ʱ��ʹ�� */
    KEY1_GPIO_CLK_ENABLE();                                     /* KEY1ʱ��ʹ�� */
    WKUP_GPIO_CLK_ENABLE();                                     /* WKUPʱ��ʹ�� */
		
    //ѡ�񰴼�������
		GPIO_InitStructure.GPIO_Pin = KEY0_GPIO_PIN; 
		// ���ð���������Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		//ʹ�ýṹ���ʼ������
		GPIO_Init(KEY0_GPIO_PORT, &GPIO_InitStructure);

		//ѡ�񰴼�������
		GPIO_InitStructure.GPIO_Pin = KEY1_GPIO_PIN; 
		//���ð���������Ϊ��������
		GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU; 
		//ʹ�ýṹ���ʼ������
		GPIO_Init(KEY1_GPIO_PORT, &GPIO_InitStructure);	


}

/**
 * @brief       ����ɨ�躯��
 * @note        �ú�������Ӧ���ȼ�(ͬʱ���¶������): WK_UP > KEY1 > KEY0!!
 * @param       mode:0 / 1, ���庬������:
 *   @arg       0,  ��֧��������(���������²���ʱ, ֻ�е�һ�ε��û᷵�ؼ�ֵ,
 *                  �����ɿ��Ժ�, �ٴΰ��²Ż᷵��������ֵ)
 *   @arg       1,  ֧��������(���������²���ʱ, ÿ�ε��øú������᷵�ؼ�ֵ)
 * @retval      ��ֵ, ��������:
 *              KEY0_PRES, 1, KEY0����
 *              KEY1_PRES, 2, KEY1����
 *              WKUP_PRES, 3, WKUP����
 */
uint8_t key_scan(uint8_t mode)
{
    static uint8_t key_up = 1;  /* �������ɿ���־ */
    uint8_t keyval = 0;
    if (mode) key_up = 1;       /* ֧������ */

    if (key_up && (KEY0 == 0 || KEY1 == 0 ))  /* �����ɿ���־Ϊ1, ��������һ������������ */
    {		
        key_up = 0;

        if (KEY0 == 0)  keyval = KEY0_PRES;

        if (KEY1 == 0)  keyval = KEY1_PRES;

    }
    else if (KEY0 == 1 && KEY1 == 1 ) /* û���κΰ�������, ��ǰ����ɿ� */
    {
        key_up = 1;
    }

    return keyval;              /* ���ؼ�ֵ */
}
