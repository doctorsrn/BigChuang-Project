/**
  ********************************************
	*   @file     main.c
  *   @author   SRn
  *   @date     2015.8.14
  *   @version  V1.0
  *   @brief    ����Ŀ������
	*********************************************
	*/
	
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "GPS_Encoder.h"
#include "TIMINTE.h"

volatile userDATA userdata={0};
volatile nmeaINFO info;  //GPS�����õ�����Ϣ
volatile nmeaPARSER parser;  //����ʱʹ�õ����ݽṹ
extern volatile u8 flag1,flag2;
extern volatile uint8_t   uart_buff[UART_BUFF_SIZE];


/////////////////////////////���������Ϣ�ĺ���
void trace(const char *str, int str_size)
{
  //#ifdef __GPS_DEBUG    //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
  
    printf("\n");
 // #endif
}

/**
  * @brief  error �ڽ������ʱ�����ʾ��Ϣ
  * @param  str: Ҫ������ַ�����str_size:���ݳ���
  * @retval ��
  */
void error(const char *str, int str_size)
{
    //#ifdef __GPS_DEBUG   //��gps_config.h�ļ���������꣬�Ƿ����������Ϣ

    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
    printf("%c",*(str+i));
    printf("\n");
  //  #endif
}



int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//����ϵͳ�ж����ȼ�����2
	USART1_Config();
	USART2_Config();
	TIM_Configuration();
	
	while(1)
	{
//		nmeaINFO info;          //GPS�����õ�����Ϣ
//    nmeaPARSER parser;      //����ʱʹ�õ����ݽṹ  
    uint8_t new_parse=0;    //�Ƿ����µĽ������ݱ�־
	 
    nmeaTIME beiJingTime;    //����ʱ�� 

    /* �����������������Ϣ�ĺ��� */
   // nmea_property()->trace_func = &trace;
   // nmea_property()->error_func = &error;

    /* ��ʼ��GPS���ݽṹ */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);
    //printf("hehe");
    while(1)
    {
      if(GPS_HalfTransferEnd)     /* ���յ�GPS_RBUFF_SIZEһ������� */
      {
        /* ����nmea��ʽ���� */
        nmea_parse(&parser, (const char*)&uart_buff[0], UART_HALF_BUFF_SIZE+1, &info);
        
        GPS_HalfTransferEnd = 0;   //��ձ�־λ
        new_parse = 1;             //���ý�����Ϣ��־ 
      }
      else if(GPS_TransferEnd)    /* ���յ���һ������ */
      {
        //nmea_parse(&parser, (const char*)&dbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        nmea_parse(&parser, (const char*)&uart_buff[UART_HALF_BUFF_SIZE+1],UART_HALF_BUFF_SIZE+1, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //���µĽ�����Ϣ   
      {    
        /* �Խ�����ʱ�����ת����ת���ɱ���ʱ�� */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        GetMiles(& userdata, & info);
				
        /* �������õ�����Ϣ */
//				printf("\r\n TIME: %d Year %d Month %d Day, %d Hour %d Minute %d Second", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//        printf("\r\n LATITUDE: %f, LONGITUDE: %f",info.lat/100,info.lon/100);
//        printf("\r\n Number of visual sattlite: %d, Used sattlite: %d",info.satinfo.inview,info.satinfo.inuse);
//        printf("\r\n Altitude: %f M", info.elv);
//        printf("\r\n Velocity: %f km/h", info.speed);
//        printf("\r\n Heeding: %f Deg", info.direction);
//				printf("\r\n ALLMILES: %f KM", userdata.allmile);
//				printf("\r\n ALLALTITUDE: %f M\r\n", userdata.allalltitude);
				
			  /*����HTML��ʽ����Ϣ�������*///����������ж���
//        printf("<?xml version=:\"1.0\" encoding=\"utf-8\"?>");
//				printf("<GPSDATA>\n");
//				printf("        <head>\n");
//				printf("                <title>This is GPS data from STM32.</title>\n");
//				printf("        </head>\n");
//				
//				printf("        <body>\n");
//				printf("                <LA>%f</LA>\n",info.lat/100);   //γ����Ϣ
//				printf("                <LO>%f</LO>\n",info.lon/100);   //������Ϣ
//				printf("                <AL>%f</AL>\n",info.elv);   //������Ϣ
//				printf("                <SP>%f</SP>\n",info.speed);   //�ٶ���Ϣ
//				printf("                <MA>%f</MA>\n",userdata.allmile);  //�����Ϣ
//				printf("                <AL>%f</AL>\n",userdata.allalltitude);  //�ۼƺ�����Ϣ
//				printf("        </body>\n");
//				printf("</GPSDATA>\n");
				
				
        __NOP;
				__NOP;
				__NOP;
        new_parse = 0;
      }
	
	}
}
}
