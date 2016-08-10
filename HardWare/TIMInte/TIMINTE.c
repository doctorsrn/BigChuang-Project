/**
  ********************************************
  *   @date     2016.4.16
  *   @version  V1.0
	* 硬件连接：---------------------
  *          | 									   |
  *          |      TIM1           |
  *          |                     |
  *          |                     |
  *           ---------------------
  *   @brief  打开定时器2，开启2S中断发送GPS解码之后的信息  
	*             
	*********************************************
	*/
	#include "TIMINTE.h"
	
	extern volatile userDATA userdata;
  extern volatile nmeaINFO info;  //GPS解码后得到的信息
	void TIM_Configuration (void)  //interrupt period is 1 Second    
	{
		NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		     
	  NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;	
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	        //使能优先级
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;			//配置抢断优先级	2
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		   //配置响应优先级	  0
	  NVIC_Init(&NVIC_InitStructure); 	
		
		TIM_DeInit(TIM2);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		
		TIM_TimeBaseStructure.TIM_Period = 9999;   //设置计数溢出大小，每计10000个数就产生一个更新事件
	  TIM_TimeBaseStructure.TIM_Prescaler =7200-1;	   //预分频系数为72-1，这样计数器时钟为1MHz 
	  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   //设置时钟分割 TIM_CKD_DIV1=0x0000,不分割
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;	  //设置计数器模式为向上计数模式
	  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); //将配置应用到TIM2中
		
	  //TIM_UpdateRequestConfig( TIM2, TIM_UpdateSource_Regular);	
	  TIM_Cmd(TIM2, ENABLE); //使能计数器
	  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	//使能中断
	  TIM_ClearFlag(TIM2, TIM_FLAG_Update);//清除标志位
		
	}
	
	void TIM2_IRQHandler(void)
	{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update)== SET)	   //检测是否发生溢出更新事件
		{
			/*按照HTML格式将信息进行输出*/
				printf("<?xml version=:\"1.0\" encoding=\"utf-8\"?>");
				printf("<GPSDATA>\n");
				printf("        <head>\n");
				printf("                <title>This is GPS data from STM32.</title>\n");
				printf("        </head>\n");
				
				printf("        <body>\n");
				printf("                <LA>%f</LA>\n",info.lat/100);   //纬度信息
				printf("                <LO>%f</LO>\n",info.lon/100);   //经度信息
				printf("                <AL>%f</AL>\n",info.elv);   //海拔信息
				printf("                <SP>%f</SP>\n",info.speed);   //速度信息
				printf("                <MA>%f</MA>\n",userdata.allmile);  //里程信息
				printf("                <AL>%f</AL>\n",userdata.allalltitude);  //累计海拔信息
				printf("        </body>\n");
				printf("</GPSDATA>\n");
			
			  TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		}
		
	}
	
	

	