/******************************************************************************
 * 文件名  ：bsp_usart2.c     
 * 硬件连接：---------------------
 *          | 									  |
 *          | PA2  - USART2(Tx)   |
 *          | PA3  - USART2(Rx)   |
 *          |                     |
 *           ---------------------
 * @brief   串口2配置，波特率9600，用于接收GPS模块传来的数据
 * 库版本  ：ST3.5.0

**********************************************************************************/
#include "bsp_usart2.h"
#include <stdarg.h>
#include "stm32f10x.h"

volatile uint8_t uart_buff[512];

/* DMA传输结束标志 */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;


/*
 * 函数名：GPS_Interrupt_Config
 * 描述  ：DMA中断配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
 //////////////////////////////
static void GPS_Interrupt_Config(void)
{
	NVIC_InitTypeDef NVIC_InitStructure;

  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);

	// DMA2 Channel Interrupt ENABLE
	NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel6_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
}

/*
 * 描述  ：DMA中断里运行的函数
 */
 //////////////////////////////
void GPS_ProcessDMAIRQ(void)
{
  
  if(DMA_GetITStatus(DMA1_IT_HT6) )         /* DMA 半传输完成 */
  {
    GPS_HalfTransferEnd = 1;                //设置半传输完成标志位
    DMA_ClearFlag(DMA1_FLAG_HT6);
  }
  else if(DMA_GetITStatus(DMA1_IT_TC6))     /* DMA 传输完成 */
  {
    GPS_TransferEnd = 1;                    //设置传输完成标志位
    DMA_ClearFlag(DMA1_FLAG_TC6);

   }
}
 

/*
 * 描述  ：DMA配置函数
 */
 //////////////////////////////
static void GPS_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		/*开启DMA时钟*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		/*设置DMA源：串口数据寄存器地址*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = (0x40004400+0x04);	   

		/*内存地址(要传输的变量的指针)*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart_buff;

		/*方向：从内存到外设*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	

		/*传输大小DMA_BufferSize=SENDBUFF_SIZE*/	
		DMA_InitStructure.DMA_BufferSize = UART_BUFF_SIZE;

		/*外设地址不增*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*内存地址自增*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	

		/*外设数据单位*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*内存数据单位 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMA模式：不断循环*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*优先级：中*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  

		/*禁止内存到内存的传输	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*配置DMA的通道*/		   
		DMA_Init(DMA1_Channel6, &DMA_InitStructure); 	   
    
    GPS_Interrupt_Config();
		
    DMA_ITConfig(DMA1_Channel6,DMA_IT_HT|DMA_IT_TC,ENABLE);  //配置DMA发送完成后产生中断

		/*使能DMA*/
		DMA_Cmd (DMA1_Channel6,ENABLE);		
    
    /* 配置串口 向 DMA发出TX请求 */
		USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    
}

/*
 * 描述  ：GPS配置函数
 */
 //////////////////////////////
void GPS_Config(void)
{
  //GPS_USART_INIT();
  GPS_DMA_Config();  
  
}
 ///////////
 
 
/*
 * 函数名：USART2_Config
 * 描述  ：USART2 GPIO 配置,工作模式配置
 * 输入  ：无
 * 输出  : 无
 * 调用  ：外部调用
 */
 //////////////////////////////
void USART2_Config(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;
	

	/* config USART2 clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);

	/* USART2 GPIO config */
  /* Configure USART2 Tx (PA.02) as alternate function push-pull */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	    
  /* Configure USART2 Rx (PA.03) as input floating */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_3;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure);
	  
	/* USART2 mode config */
	USART_InitStructure.USART_BaudRate = 9600;                //GPS模块默认使用波特率：9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure); 

	USART_Cmd(USART2, ENABLE);
	
	#if 0    //开启USART2中断
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//开启接受中断

	//Usart2 NVIC 配置
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//抢占优先级3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//子优先级3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQ通道使能
	NVIC_Init(&NVIC_InitStructure);	//根据指定的参数初始化VIC寄存器、
	#endif
	GPS_Config();
}


/*
 * 函数名：fputc
 * 描述  ：重定向c库函数printf到USART2
 * 输入  ：无
 * 输出  ：无
 * 调用  ：由printf调用
 */
//int fputc(int ch, FILE *f)
//{
///* 将Printf内容发往串口 */
//  USART_SendData(USART2, (unsigned char) ch);
//  while (!(USART2->SR & USART_FLAG_TXE));
// 
//  return (ch);
//}

/*
 * 函数名：itoa
 * 描述  ：将整形数据转换成字符串
 * 输入  ：-radix =10 表示10进制，其他结果为0
 *         -value 要转换的整形数
 *         -buf 转换后的字符串
 *         -radix = 10
 * 输出  ：无
 * 返回  ：无
 * 调用  ：被USART2_printf()调用
 */
static char *itoa(int value, char *string, int radix)
{
    int     i, d;
    int     flag = 0;
    char    *ptr = string;

    /* This implementation only works for decimal numbers. */
    if (radix != 10)
    {
        *ptr = 0;
        return string;
    }

    if (!value)
    {
        *ptr++ = 0x30;
        *ptr = 0;
        return string;
    }

    /* if this is a negative value insert the minus sign. */
    if (value < 0)
    {
        *ptr++ = '-';

        /* Make the value positive. */
        value *= -1;
    }

    for (i = 10000; i > 0; i /= 10)
    {
        d = value / i;

        if (d || flag)
        {
            *ptr++ = (char)(d + 0x30);
            value -= (d * i);
            flag = 1;
        }
    }

    /* Null terminate the string. */
    *ptr = 0;

    return string;

} /* NCL_Itoa */


void DMA1_Channel6_IRQHandler(void)
 {
	 GPS_ProcessDMAIRQ();
 }


 
#if 0   //串口中断直接接收数据存入缓冲数组的方法无法正常工作！！
//中断缓存串口数据
static uint16_t uart_p = 0;
volatile u8 flag1=0,flag2=0;
void USART2_IRQHandler(void)
{
	char c=0;
	if(USART_GetITStatus(USART2, USART_IT_RXNE) != RESET)
	{
		c=USART_ReceiveData(USART2);
    if(uart_p<UART_BUFF_SIZE)
    {
            uart_buff[uart_p] = c;
            uart_p++;
		if(uart_p==UART_HALF_BUFF_SIZE)
		{
			flag1=1;
		}
	}
		else  //(uart_p==UART_BUFF_SIZE)
		{
			
			flag2=1;
			uart_p=0;
			uart_buff[uart_p] = c;
		}
		USART_ClearITPendingBit(USART1, USART_IT_RXNE);
	}
		
}

//获取接收到的数据和长度
char *get_rebuff(uint8_t *len)
{
    *len = uart_p;
    return (char *)&uart_buff;
}

void clean_rebuff(void)
{
    uart_p = 0;
}

#endif



/*
 * 函数名：USART2_printf
 * 描述  ：格式化输出，类似于C库中的printf，但这里没有用到C库
 * 输入  ：-USARTx 串口通道，这里只用到了串口2，即USART2
 *		     -Data   要发送到串口的内容的指针
 *			   -...    其他参数
 * 输出  ：无
 * 返回  ：无 
 * 调用  ：外部调用
 *         典型应用USART2_printf( USART2, "\r\n this is a demo \r\n" );
 *            		 USART2_printf( USART2, "\r\n %d \r\n", i );
 *            		 USART2_printf( USART2, "\r\n %s \r\n", j );
 */
void USART2_printf(USART_TypeDef* USARTx, char *Data,...)
{
	const char *s;
  int d;   
  char buf[16];

  va_list ap;
  va_start(ap, Data);

	while ( *Data != 0)     // 判断是否到达字符串结束符
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //回车符
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //换行符
					USART_SendData(USARTx, 0x0a);	
					Data ++;
					break;
				
				default:
					Data ++;
				    break;
			}			 
		}
		else if ( *Data == '%')
		{									  //
			switch ( *++Data )
			{				
				case 's':										  //字符串
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//十进制
          d = va_arg(ap, int);
          itoa(d, buf, 10);
          for (s = buf; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;
				 default:
						Data++;
				    break;
			}		 
		} /* end of else if */
		else USART_SendData(USARTx, *Data++);
		while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
	}
}

