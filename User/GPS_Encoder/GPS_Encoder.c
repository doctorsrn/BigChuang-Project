/**
  ********************************************
	*   @file     GPS_Encoder.c
  *   @author   SRn
  *   @date     2015.8.16
  *   @version  V1.0
  *   @brief    GPS���ݽ��뺯��
	*********************************************
	*/
	
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "nmea/nmea.h"      //��ԴGPS���뺯����
#include "GPS_Encoder.h"
#include "math.h"
void GPS_Enco(char *buff,int buff_size)    //����������������������ݳ���
{
	     nmeaINFO info;  //GPS�����õ�����Ϣ
	     nmeaPARSER parser;  //����ʱʹ�õ����ݽṹ
	    
	    /* ����nmea��ʽ���� */
       nmea_parse(&parser, (const char*)buff, buff_size, &info);
	     printf("he");
	        /* �������õ�����Ϣ */
      //  printf("\r\nʱ��%d,%d,%d,%d,%d,%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
	     // printf("\r\nʱ��%d,%d,%d,%d,%d,%d\r\n", info.utc.year, info.utc.mon,info.utc.day,info.utc.hour,info.utc.min,info.utc.sec);
        printf("\r\nγ�ȣ�%f,����%f\r\n",info.lat,info.lon);
        printf("\r\n����ʹ�õ����ǣ�%d,�ɼ����ǣ�%d",info.satinfo.inuse,info.satinfo.inview);
        printf("\r\n���θ߶ȣ�%f �� ", info.elv);
        printf("\r\n�ٶȣ�%f km/h ", info.speed);
        printf("\r\n����%f ��", info.direction);
	
}

/*
 * ����  ���ж��Ƿ�������
 */
 //////////////////////////////
static uint8_t IsLeapYear(uint8_t iYear) 
{ 
    uint16_t    Year; 
    Year    =    2000+iYear; 
    if((Year&3)==0) 
    { 
        return ((Year%400==0) || (Year%100!=0)); 
    } 
     return 0; 
}




/*
 * ����  ���Խ��������ʱ�����ת������Ϊ����ʱ��
 */
 //////////////////////////////
void    GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //������ʱ�����ݴ���� 
     
    if(GMT==0)    return;                //�������0ʱ��ֱ�ӷ��� 
    if(GMT>12)    return;                //ʱ�����Ϊ12 �����򷵻�         

    YY    =    SourceTime->year;                //��ȡ�� 
    MM    =    SourceTime->mon;                 //��ȡ�� 
    DD    =    SourceTime->day;                 //��ȡ�� 
    hh    =    SourceTime->hour;                //��ȡʱ 
    mm    =    SourceTime->min;                 //��ȡ�� 
    ss    =    SourceTime->sec;                 //��ȡ�� 

    if(AREA)                        //��(+)ʱ������ 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+GMT-24;        //�ȵó�ʱ�� 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //���·�(12�µ�������) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //С�·�2�µ�������) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //����2�·� 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //��������������2��29�� ���߲�����������2��28�� 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //����12�·� 
            { 
                if(DD<31)    DD++; 
                else        //�������һ�� 
                {               
                    DD    =    1; 
                    MM    =    1; 
                    YY    ++; 
                } 
            } 
        } 
    } 
    else 
    {     
        if(hh>=GMT)    hh    -=    GMT;    //������������ʱ�䴦��ͬһ�������Сʱ���� 
        else                        //����Ѿ����ڸ�������ʱ��1����������ڴ��� 
        { 
            hh    =    hh+24-GMT;        //�ȵó�ʱ�� 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //�����Ǵ��·�(1�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //������С�·�2�µ�������) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //�����ϸ�����2�·� 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //�������� 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //����1�·� 
            { 
                if(DD>1)    DD--; 
                else        //�����һ�� 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //������ 
    ConvertTime->mon    =    MM;                //������ 
    ConvertTime->day    =    DD;                //������ 
    ConvertTime->hour   =    hh;                //����ʱ 
    ConvertTime->min    =    mm;                //���·� 
    ConvertTime->sec    =    ss;                //������ 
}  

float userabs(float a)
{
	if (a > 0)
		return a;
	else
		return -a;
}
int GetMiles(userDATA * userdata, nmeaINFO * nmeainfo)
{
	float temp1=0;
	float temp2,temp3;
	
	//updata data
	userdata->lastlat = userdata->currentlat;
	userdata->lastlon = userdata->currentlon;
	userdata->lastalltitude = userdata->currentalltitude;
	userdata->currentlat = nmeainfo->lat/100;
	userdata->currentlon = nmeainfo->lon/100;
	userdata->currentalltitude = nmeainfo->elv;
	
	
	temp1 = userabs(userdata->currentlat - userdata->lastlat);
	temp2 = userabs(userdata->currentlon - userdata->lastlon);
	temp3 = userabs(userdata->currentalltitude - userdata->lastalltitude);
	if((temp1 > 1) || (temp3 > 10))  //judge if first start system
		return 0;
	else
	{
		userdata->allmile += sqrt(temp1*temp1+temp2*temp2)*111;
		userdata->allalltitude += userabs(userdata->currentalltitude - userdata->lastalltitude);
	}
}
