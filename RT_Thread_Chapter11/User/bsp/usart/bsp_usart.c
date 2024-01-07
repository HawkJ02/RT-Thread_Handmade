/**
  ******************************************************************************
  * @file    bsp_usart.c
  * @author  fire
  * @version V1.0
  * @date    2013-xx-xx
  * @brief   重定向c库printf函数到usart端口
  ******************************************************************************
  * @attention
  *
  * 实验平台:秉火STM32 F103-霸道 开发板  
  * 论坛    :http://www.firebbs.cn
  * 淘宝    :https://fire-stm32.taobao.com
  *
  ******************************************************************************
  */ 
	
#include "bsp_usart.h"
#include "rtthread.h"

extern rt_sem_t test_sem;

 /**
  * @brief  配置嵌套向量中断控制器NVIC
  * @param  无
  * @retval 无
  */
static void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  /* 配置NVIC为优先级组1 */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);
  
  /* 配置中断源：按键1 */
  NVIC_InitStructure.NVIC_IRQChannel = DEBUG_USART_IRQ;
  /* 配置抢占优先级 */
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  /* 配置子优先级 */
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  /* 使能中断通道 */
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);

}

char USART_RX_BUFFER[USART_RBUFFER_SIZE];

static void USART1_DMA_Configuration()
{
	DMA_InitTypeDef DMA_InitStructure;
	
	// 打开DMA时钟
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1,ENABLE);
	
	// 配置DMA
	// DMA的搬运起始位置
	DMA_InitStructure.DMA_PeripheralBaseAddr 	= (uint32_t)USART_DR_ADDRESS;
	// DMA的搬运目的地
	DMA_InitStructure.DMA_MemoryBaseAddr			=	(uint32_t)USART_RX_BUFFER;
	// 搬运方向：由外设到内存
	DMA_InitStructure.DMA_DIR 								=  DMA_DIR_PeripheralSRC;
	// 搬运货物的数量
	DMA_InitStructure.DMA_BufferSize					=	USART_RBUFFER_SIZE;
	// 外设地址是否递增
	DMA_InitStructure.DMA_PeripheralInc				= DMA_PeripheralInc_Disable;
	// 内存地址是否递增
	DMA_InitStructure.DMA_MemoryInc						= DMA_MemoryInc_Enable;
	// 外设的数据单位
	DMA_InitStructure.DMA_PeripheralDataSize	= DMA_PeripheralDataSize_Byte;
	// 内存的数据单位
	DMA_InitStructure.DMA_MemoryDataSize			= DMA_MemoryDataSize_Byte;
	// DMA模式：循环
	DMA_InitStructure.DMA_Mode								= DMA_Mode_Circular;
	// DMA的优先级:极高
	DMA_InitStructure.DMA_Priority						= DMA_Priority_VeryHigh;
	// DMA是否允许内存到内存
	DMA_InitStructure.DMA_M2M									= DMA_M2M_Disable;
	
	// 初始化DMA
	DMA_Init(USART1_RX_DMA_CHANNEL,&DMA_InitStructure);
	
	// DMA1_FLAG_TC5 是指DMA1控制器的通道5传输完成标志位,
	// TC 通常表示 "Transfer Complete"
	// 清除DMA1通道5标志位
	DMA_ClearFlag(DMA1_FLAG_TC5);
	
	// 打开DMA通道的传输错误中断,
	// TE 代表了 "Transfer Error"
	DMA_ITConfig(USART1_RX_DMA_CHANNEL,DMA_IT_TE,ENABLE);
	// 使能DMA
	DMA_Cmd(USART1_RX_DMA_CHANNEL,ENABLE);
}

 /**
  * @brief  USART GPIO 配置,工作参数配置
  * @param  无
  * @retval 无
  */
void USART_Config(void)
{

	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	//初始化串口DMA
	USART1_DMA_Configuration();

	// 打开串口GPIO的时钟
	DEBUG_USART_GPIO_APBxClkCmd(DEBUG_USART_GPIO_CLK, ENABLE);
	
	// 打开串口外设的时钟
	DEBUG_USART_APBxClkCmd(DEBUG_USART_CLK, ENABLE);

	// 将USART Tx的GPIO配置为推挽复用模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_TX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_Init(DEBUG_USART_TX_GPIO_PORT, &GPIO_InitStructure);

  // 将USART Rx的GPIO配置为浮空输入模式
	GPIO_InitStructure.GPIO_Pin = DEBUG_USART_RX_GPIO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(DEBUG_USART_RX_GPIO_PORT, &GPIO_InitStructure);
	
	// 配置串口的工作参数
	// 配置波特率
	USART_InitStructure.USART_BaudRate = DEBUG_USART_BAUDRATE;
	// 配置 针数据字长
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	// 配置停止位
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	// 配置校验位
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	// 配置硬件流控制
	USART_InitStructure.USART_HardwareFlowControl = 
	USART_HardwareFlowControl_None;
	// 配置工作模式，收发一起
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
	// 完成串口的初始化配置
	USART_Init(DEBUG_USART1, &USART_InitStructure);	
	
	// 初始化NVIC中断向量表
	NVIC_Configuration();
	// 设置IDLE空闲中断
	USART_ITConfig(DEBUG_USART1,USART_IT_IDLE,ENABLE);
	// 当使用USART_DMAReq_Rx时，USART外设将接收数据并将其存储到接收缓冲区，
	// 然后DMA传输将从接收缓冲区将数据传输到内存中。
	USART_DMACmd(DEBUG_USART1,USART_DMAReq_Rx,ENABLE);
	
	// 使能串口
	USART_Cmd(DEBUG_USART1, ENABLE);	    
}


void USART1_DMA_RX_DATA()
{
	// 先关闭DMA
	DMA_Cmd(USART1_RX_DMA_CHANNEL,DISABLE);
	// 清除DMA1通道5接收成功的标志位
	DMA_ClearFlag(DMA1_FLAG_TC5);
	// 重新赋值计数值，搬运数据大小
	// "CN" 表示 "Counter"，即计数器，
	// "DTR" 可能是 "Data Transfer Register" 的缩写。
	USART1_RX_DMA_CHANNEL->CNDTR = USART_RBUFFER_SIZE;
	// 打开DMA
	DMA_Cmd(USART1_RX_DMA_CHANNEL,ENABLE);
	// 释放信号量，表示已经接收到新的数据
	rt_sem_release(test_sem);
}










/*****************  发送一个字节 **********************/
void Usart_SendByte( USART_TypeDef * pUSARTx, uint8_t ch)
{
	/* 发送一个字节数据到USART */
	USART_SendData(pUSARTx,ch);
		
	/* 等待发送数据寄存器为空 */
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

/****************** 发送8位的数组 ************************/
void Usart_SendArray( USART_TypeDef * pUSARTx, uint8_t *array, uint16_t num)
{
  uint8_t i;
	
	for(i=0; i<num; i++)
  {
	    /* 发送一个字节数据到USART */
	    Usart_SendByte(pUSARTx,array[i]);	
  
  }
	/* 等待发送完成 */
	while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET);
}

/*****************  发送字符串 **********************/
void Usart_SendString( USART_TypeDef * pUSARTx, char *str)
{
	unsigned int k=0;
  do 
  {
      Usart_SendByte( pUSARTx, *(str + k) );
      k++;
  } while(*(str + k)!='\0');
  
  /* 等待发送完成 */
  while(USART_GetFlagStatus(pUSARTx,USART_FLAG_TC)==RESET)
  {}
}

/*****************  发送一个16位数 **********************/
void Usart_SendHalfWord( USART_TypeDef * pUSARTx, uint16_t ch)
{
	uint8_t temp_h, temp_l;
	
	/* 取出高八位 */
	temp_h = (ch&0XFF00)>>8;
	/* 取出低八位 */
	temp_l = ch&0XFF;
	
	/* 发送高八位 */
	USART_SendData(pUSARTx,temp_h);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);
	
	/* 发送低八位 */
	USART_SendData(pUSARTx,temp_l);	
	while (USART_GetFlagStatus(pUSARTx, USART_FLAG_TXE) == RESET);	
}

///重定向c库函数printf到串口，重定向后可使用printf函数
int fputc(int ch, FILE *f)
{
		/* 发送一个字节数据到串口 */
		USART_SendData(DEBUG_USART1, (uint8_t) ch);
		
		/* 等待发送完毕 */
		while (USART_GetFlagStatus(DEBUG_USART1, USART_FLAG_TXE) == RESET);		
	
		return (ch);
}

///重定向c库函数scanf到串口，重写向后可使用scanf、getchar等函数
int fgetc(FILE *f)
{
		/* 等待串口输入数据 */
		while (USART_GetFlagStatus(DEBUG_USART1, USART_FLAG_RXNE) == RESET);

		return (int)USART_ReceiveData(DEBUG_USART1);
}

