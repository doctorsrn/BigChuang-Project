#ifndef __USART2_H
#define	__USART2_H

#include "stm32f10x.h"
#include <stdio.h>

#define UART_BUFF_SIZE      512
#define UART_HALF_BUFF_SIZE 255



void USART2_Config(void);
//int fputc(int ch, FILE *f);
void USART2_printf(USART_TypeDef* USARTx, char *Data,...);


extern char *get_rebuff(uint8_t *len);
extern void clean_rebuff(void);

extern __IO uint8_t GPS_TransferEnd ;
extern __IO uint8_t GPS_HalfTransferEnd;

#endif /* __USART2_H */
