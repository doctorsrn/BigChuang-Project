/******************************************************************************
 * �ļ���  ��bsp_usart2.c     
 * Ӳ�����ӣ�---------------------
 *          | 									  |
 *          | PA2  - USART2(Tx)   |
 *          | PA3  - USART2(Rx)   |
 *          |                     |
 *           ---------------------
 * @brief   ����2���ã�������9600�����ڽ���GPSģ�鴫��������
 * ��汾  ��ST3.5.0

**********************************************************************************/
#include "bsp_usart2.h"
#include <stdarg.h>
#include "stm32f10x.h"

volatile uint8_t uart_buff[512];

/* DMA���������־ */
__IO uint8_t GPS_TransferEnd = 0, GPS_HalfTransferEnd = 0;


/*
 * ��������GPS_Interrupt_Config
 * ����  ��DMA�ж�����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
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
 * ����  ��DMA�ж������еĺ���
 */
 //////////////////////////////
void GPS_ProcessDMAIRQ(void)
{
  
  if(DMA_GetITStatus(DMA1_IT_HT6) )         /* DMA �봫����� */
  {
    GPS_HalfTransferEnd = 1;                //���ð봫����ɱ�־λ
    DMA_ClearFlag(DMA1_FLAG_HT6);
  }
  else if(DMA_GetITStatus(DMA1_IT_TC6))     /* DMA ������� */
  {
    GPS_TransferEnd = 1;                    //���ô�����ɱ�־λ
    DMA_ClearFlag(DMA1_FLAG_TC6);

   }
}
 

/*
 * ����  ��DMA���ú���
 */
 //////////////////////////////
static void GPS_DMA_Config(void)
{
		DMA_InitTypeDef DMA_InitStructure;
	
		/*����DMAʱ��*/
		RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

		/*����DMAԴ���������ݼĴ�����ַ*/
		DMA_InitStructure.DMA_PeripheralBaseAddr = (0x40004400+0x04);	   

		/*�ڴ��ַ(Ҫ����ı�����ָ��)*/
		DMA_InitStructure.DMA_MemoryBaseAddr = (u32)uart_buff;

		/*���򣺴��ڴ浽����*/		
		DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;	

		/*�����СDMA_BufferSize=SENDBUFF_SIZE*/	
		DMA_InitStructure.DMA_BufferSize = UART_BUFF_SIZE;

		/*�����ַ����*/	    
		DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable; 

		/*�ڴ��ַ����*/
		DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;	

		/*�������ݵ�λ*/	
		DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;

		/*�ڴ����ݵ�λ 8bit*/
		DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;	 

		/*DMAģʽ������ѭ��*/
		DMA_InitStructure.DMA_Mode = DMA_Mode_Circular;	 

		/*���ȼ�����*/	
		DMA_InitStructure.DMA_Priority = DMA_Priority_Medium;  

		/*��ֹ�ڴ浽�ڴ�Ĵ���	*/
		DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;

		/*����DMA��ͨ��*/		   
		DMA_Init(DMA1_Channel6, &DMA_InitStructure); 	   
    
    GPS_Interrupt_Config();
		
    DMA_ITConfig(DMA1_Channel6,DMA_IT_HT|DMA_IT_TC,ENABLE);  //����DMA������ɺ�����ж�

		/*ʹ��DMA*/
		DMA_Cmd (DMA1_Channel6,ENABLE);		
    
    /* ���ô��� �� DMA����TX���� */
		USART_DMACmd(USART2, USART_DMAReq_Rx, ENABLE);

    
}

/*
 * ����  ��GPS���ú���
 */
 //////////////////////////////
void GPS_Config(void)
{
  //GPS_USART_INIT();
  GPS_DMA_Config();  
  
}
 ///////////
 
 
/*
 * ��������USART2_Config
 * ����  ��USART2 GPIO ����,����ģʽ����
 * ����  ����
 * ���  : ��
 * ����  ���ⲿ����
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
	USART_InitStructure.USART_BaudRate = 9600;                //GPSģ��Ĭ��ʹ�ò����ʣ�9600
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;
	USART_InitStructure.USART_StopBits = USART_StopBits_1;
	USART_InitStructure.USART_Parity = USART_Parity_No ;
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;

	USART_Init(USART2, &USART_InitStructure); 

	USART_Cmd(USART2, ENABLE);
	
	#if 0    //����USART2�ж�
	USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);//���������ж�

	//Usart2 NVIC ����
	NVIC_InitTypeDef NVIC_InitStructure;
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;
	NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=2;//��ռ���ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelSubPriority =2;		//�����ȼ�3
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;			//IRQͨ��ʹ��
	NVIC_Init(&NVIC_InitStructure);	//����ָ���Ĳ�����ʼ��VIC�Ĵ�����
	#endif
	GPS_Config();
}


/*
 * ��������fputc
 * ����  ���ض���c�⺯��printf��USART2
 * ����  ����
 * ���  ����
 * ����  ����printf����
 */
//int fputc(int ch, FILE *f)
//{
///* ��Printf���ݷ������� */
//  USART_SendData(USART2, (unsigned char) ch);
//  while (!(USART2->SR & USART_FLAG_TXE));
// 
//  return (ch);
//}

/*
 * ��������itoa
 * ����  ������������ת�����ַ���
 * ����  ��-radix =10 ��ʾ10���ƣ��������Ϊ0
 *         -value Ҫת����������
 *         -buf ת������ַ���
 *         -radix = 10
 * ���  ����
 * ����  ����
 * ����  ����USART2_printf()����
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


 
#if 0   //�����ж�ֱ�ӽ������ݴ��뻺������ķ����޷�������������
//�жϻ��洮������
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

//��ȡ���յ������ݺͳ���
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
 * ��������USART2_printf
 * ����  ����ʽ�������������C���е�printf��������û���õ�C��
 * ����  ��-USARTx ����ͨ��������ֻ�õ��˴���2����USART2
 *		     -Data   Ҫ���͵����ڵ����ݵ�ָ��
 *			   -...    ��������
 * ���  ����
 * ����  ���� 
 * ����  ���ⲿ����
 *         ����Ӧ��USART2_printf( USART2, "\r\n this is a demo \r\n" );
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

	while ( *Data != 0)     // �ж��Ƿ񵽴��ַ���������
	{				                          
		if ( *Data == 0x5c )  //'\'
		{									  
			switch ( *++Data )
			{
				case 'r':							          //�س���
					USART_SendData(USARTx, 0x0d);
					Data ++;
					break;

				case 'n':							          //���з�
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
				case 's':										  //�ַ���
					s = va_arg(ap, const char *);
          for ( ; *s; s++) 
					{
						USART_SendData(USARTx,*s);
						while( USART_GetFlagStatus(USARTx, USART_FLAG_TXE) == RESET );
          }
					Data++;
          break;

        case 'd':										//ʮ����
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

