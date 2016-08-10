/**
  ********************************************
  *   @date     2016.4.16
  *   @version  V1.0
	* Ӳ�����ӣ�---------------------
  *          | 									   |
  *          |      TIM1           |
  *          |                     |
  *          |                     |
  *           ---------------------
  *   @brief  �򿪶�ʱ��2������2S�жϷ���GPS����֮�����Ϣ  
	*             
	*********************************************
	*/
	#include "TIMINTE.h"
	
	extern volatile userDATA userdata;
  extern volatile nmeaINFO info;  //GPS�����õ�����Ϣ
	void TIM_Configuration (void)  //interrupt period is 1 Second    
	{
		NVIC_InitTypeDef NVIC_InitStructure;
//    TIM_ICInitTypeDef TIM_ICInitStructure;
    TIM_TimeBaseInitTypeDef  TIM_TimeBaseStructure;
		
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);		     
	  NVIC_InitStructure.NVIC_IRQChannel =TIM2_IRQn;	
	  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;	        //ʹ�����ȼ�
	  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority =2;			//�����������ȼ�	2
	  NVIC_InitStructure.NVIC_IRQChannelSubPriority =0;		   //������Ӧ���ȼ�	  0
	  NVIC_Init(&NVIC_InitStructure); 	
		
		TIM_DeInit(TIM2);
    TIM_TimeBaseStructInit(&TIM_TimeBaseStructure);
		
		TIM_TimeBaseStructure.TIM_Period = 9999;   //���ü��������С��ÿ��10000�����Ͳ���һ�������¼�
	  TIM_TimeBaseStructure.TIM_Prescaler =7200-1;	   //Ԥ��Ƶϵ��Ϊ72-1������������ʱ��Ϊ1MHz 
	  TIM_TimeBaseStructure.TIM_ClockDivision = TIM_CKD_DIV1;   //����ʱ�ӷָ� TIM_CKD_DIV1=0x0000,���ָ�
	  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up ;	  //���ü�����ģʽΪ���ϼ���ģʽ
	  TIM_TimeBaseStructure.TIM_RepetitionCounter = 0;
    TIM_TimeBaseInit(TIM2,&TIM_TimeBaseStructure); //������Ӧ�õ�TIM2��
		
	  //TIM_UpdateRequestConfig( TIM2, TIM_UpdateSource_Regular);	
	  TIM_Cmd(TIM2, ENABLE); //ʹ�ܼ�����
	  TIM_ITConfig(TIM2,TIM_IT_Update,ENABLE);	//ʹ���ж�
	  TIM_ClearFlag(TIM2, TIM_FLAG_Update);//�����־λ
		
	}
	
	void TIM2_IRQHandler(void)
	{
		if(TIM_GetITStatus(TIM2, TIM_IT_Update)== SET)	   //����Ƿ�����������¼�
		{
			/*����HTML��ʽ����Ϣ�������*/
				printf("<?xml version=:\"1.0\" encoding=\"utf-8\"?>");
				printf("<GPSDATA>\n");
				printf("        <head>\n");
				printf("                <title>This is GPS data from STM32.</title>\n");
				printf("        </head>\n");
				
				printf("        <body>\n");
				printf("                <LA>%f</LA>\n",info.lat/100);   //γ����Ϣ
				printf("                <LO>%f</LO>\n",info.lon/100);   //������Ϣ
				printf("                <AL>%f</AL>\n",info.elv);   //������Ϣ
				printf("                <SP>%f</SP>\n",info.speed);   //�ٶ���Ϣ
				printf("                <MA>%f</MA>\n",userdata.allmile);  //�����Ϣ
				printf("                <AL>%f</AL>\n",userdata.allalltitude);  //�ۼƺ�����Ϣ
				printf("        </body>\n");
				printf("</GPSDATA>\n");
			
			  TIM_ClearITPendingBit(TIM2 , TIM_FLAG_Update);
		}
		
	}
	
	

	