/**
  ********************************************
	*   @file     main.c
  *   @author   SRn
  *   @date     2015.8.14
  *   @version  V1.0
  *   @brief    大创项目主程序
	*********************************************
	*/
	
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "GPS_Encoder.h"
#include "TIMINTE.h"

volatile userDATA userdata={0};
volatile nmeaINFO info;  //GPS解码后得到的信息
volatile nmeaPARSER parser;  //解码时使用的数据结构
extern volatile u8 flag1,flag2;
extern volatile uint8_t   uart_buff[UART_BUFF_SIZE];


/////////////////////////////输出测试信息的函数
void trace(const char *str, int str_size)
{
  //#ifdef __GPS_DEBUG    //在gps_config.h文件配置这个宏，是否输出调试信息
    uint16_t i;
    printf("\r\nTrace: ");
    for(i=0;i<str_size;i++)
      printf("%c",*(str+i));
  
    printf("\n");
 // #endif
}

/**
  * @brief  error 在解码出错时输出提示消息
  * @param  str: 要输出的字符串，str_size:数据长度
  * @retval 无
  */
void error(const char *str, int str_size)
{
    //#ifdef __GPS_DEBUG   //在gps_config.h文件配置这个宏，是否输出调试信息

    uint16_t i;
    printf("\r\nError: ");
    for(i=0;i<str_size;i++)
    printf("%c",*(str+i));
    printf("\n");
  //  #endif
}



int main()
{
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);//设置系统中断优先级分组2
	USART1_Config();
	USART2_Config();
	TIM_Configuration();
	
	while(1)
	{
//		nmeaINFO info;          //GPS解码后得到的信息
//    nmeaPARSER parser;      //解码时使用的数据结构  
    uint8_t new_parse=0;    //是否有新的解码数据标志
	 
    nmeaTIME beiJingTime;    //北京时间 

    /* 设置用于输出调试信息的函数 */
   // nmea_property()->trace_func = &trace;
   // nmea_property()->error_func = &error;

    /* 初始化GPS数据结构 */
    nmea_zero_INFO(&info);
    nmea_parser_init(&parser);
    //printf("hehe");
    while(1)
    {
      if(GPS_HalfTransferEnd)     /* 接收到GPS_RBUFF_SIZE一半的数据 */
      {
        /* 进行nmea格式解码 */
        nmea_parse(&parser, (const char*)&uart_buff[0], UART_HALF_BUFF_SIZE+1, &info);
        
        GPS_HalfTransferEnd = 0;   //清空标志位
        new_parse = 1;             //设置解码消息标志 
      }
      else if(GPS_TransferEnd)    /* 接收到另一半数据 */
      {
        //nmea_parse(&parser, (const char*)&dbuff[0], HALF_GPS_RBUFF_SIZE, &info);
        nmea_parse(&parser, (const char*)&uart_buff[UART_HALF_BUFF_SIZE+1],UART_HALF_BUFF_SIZE+1, &info);
       
        GPS_TransferEnd = 0;
        new_parse =1;
      }
      
      if(new_parse )                //有新的解码消息   
      {    
        /* 对解码后的时间进行转换，转换成北京时间 */
        GMTconvert(&info.utc,&beiJingTime,8,1);
        GetMiles(& userdata, & info);
				
        /* 输出解码得到的信息 */
//				printf("\r\n TIME: %d Year %d Month %d Day, %d Hour %d Minute %d Second", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
//        printf("\r\n LATITUDE: %f, LONGITUDE: %f",info.lat/100,info.lon/100);
//        printf("\r\n Number of visual sattlite: %d, Used sattlite: %d",info.satinfo.inview,info.satinfo.inuse);
//        printf("\r\n Altitude: %f M", info.elv);
//        printf("\r\n Velocity: %f km/h", info.speed);
//        printf("\r\n Heeding: %f Deg", info.direction);
//				printf("\r\n ALLMILES: %f KM", userdata.allmile);
//				printf("\r\n ALLALTITUDE: %f M\r\n", userdata.allalltitude);
				
			  /*按照HTML格式将信息进行输出*///输出放置在中断中
//        printf("<?xml version=:\"1.0\" encoding=\"utf-8\"?>");
//				printf("<GPSDATA>\n");
//				printf("        <head>\n");
//				printf("                <title>This is GPS data from STM32.</title>\n");
//				printf("        </head>\n");
//				
//				printf("        <body>\n");
//				printf("                <LA>%f</LA>\n",info.lat/100);   //纬度信息
//				printf("                <LO>%f</LO>\n",info.lon/100);   //经度信息
//				printf("                <AL>%f</AL>\n",info.elv);   //海拔信息
//				printf("                <SP>%f</SP>\n",info.speed);   //速度信息
//				printf("                <MA>%f</MA>\n",userdata.allmile);  //里程信息
//				printf("                <AL>%f</AL>\n",userdata.allalltitude);  //累计海拔信息
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
