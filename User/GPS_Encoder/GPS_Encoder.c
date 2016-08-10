/**
  ********************************************
	*   @file     GPS_Encoder.c
  *   @author   SRn
  *   @date     2015.8.16
  *   @version  V1.0
  *   @brief    GPS数据解码函数
	*********************************************
	*/
	
#include "stm32f10x.h"
#include "bsp_usart1.h"
#include "bsp_usart2.h"
#include "nmea/nmea.h"      //开源GPS解码函数库
#include "GPS_Encoder.h"
#include "math.h"
void GPS_Enco(char *buff,int buff_size)    //传入待解码的数据数组和数据长度
{
	     nmeaINFO info;  //GPS解码后得到的信息
	     nmeaPARSER parser;  //解码时使用的数据结构
	    
	    /* 进行nmea格式解码 */
       nmea_parse(&parser, (const char*)buff, buff_size, &info);
	     printf("he");
	        /* 输出解码得到的信息 */
      //  printf("\r\n时间%d,%d,%d,%d,%d,%d\r\n", beiJingTime.year+1900, beiJingTime.mon+1,beiJingTime.day,beiJingTime.hour,beiJingTime.min,beiJingTime.sec);
	     // printf("\r\n时间%d,%d,%d,%d,%d,%d\r\n", info.utc.year, info.utc.mon,info.utc.day,info.utc.hour,info.utc.min,info.utc.sec);
        printf("\r\n纬度：%f,经度%f\r\n",info.lat,info.lon);
        printf("\r\n正在使用的卫星：%d,可见卫星：%d",info.satinfo.inuse,info.satinfo.inview);
        printf("\r\n海拔高度：%f 米 ", info.elv);
        printf("\r\n速度：%f km/h ", info.speed);
        printf("\r\n航向：%f 度", info.direction);
	
}

/*
 * 描述  ：判断是否是闰年
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
 * 描述  ：对解码出来的时间进行转换，变为北京时间
 */
 //////////////////////////////
void    GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA) 
{ 
    uint32_t    YY,MM,DD,hh,mm,ss;        //年月日时分秒暂存变量 
     
    if(GMT==0)    return;                //如果处于0时区直接返回 
    if(GMT>12)    return;                //时区最大为12 超过则返回         

    YY    =    SourceTime->year;                //获取年 
    MM    =    SourceTime->mon;                 //获取月 
    DD    =    SourceTime->day;                 //获取日 
    hh    =    SourceTime->hour;                //获取时 
    mm    =    SourceTime->min;                 //获取分 
    ss    =    SourceTime->sec;                 //获取秒 

    if(AREA)                        //东(+)时区处理 
    { 
        if(hh+GMT<24)    hh    +=    GMT;//如果与格林尼治时间处于同一天则仅加小时即可 
        else                        //如果已经晚于格林尼治时间1天则进行日期处理 
        { 
            hh    =    hh+GMT-24;        //先得出时间 
            if(MM==1 || MM==3 || MM==5 || MM==7 || MM==8 || MM==10)    //大月份(12月单独处理) 
            { 
                if(DD<31)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==4 || MM==6 || MM==9 || MM==11)                //小月份2月单独处理) 
            { 
                if(DD<30)    DD++; 
                else 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
            } 
            else if(MM==2)    //处理2月份 
            { 
                if((DD==29) || (DD==28 && IsLeapYear(YY)==0))        //本来是闰年且是2月29日 或者不是闰年且是2月28日 
                { 
                    DD    =    1; 
                    MM    ++; 
                } 
                else    DD++; 
            } 
            else if(MM==12)    //处理12月份 
            { 
                if(DD<31)    DD++; 
                else        //跨年最后一天 
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
        if(hh>=GMT)    hh    -=    GMT;    //如果与格林尼治时间处于同一天则仅减小时即可 
        else                        //如果已经早于格林尼治时间1天则进行日期处理 
        { 
            hh    =    hh+24-GMT;        //先得出时间 
            if(MM==2 || MM==4 || MM==6 || MM==8 || MM==9 || MM==11)    //上月是大月份(1月单独处理) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    31; 
                    MM    --; 
                } 
            } 
            else if(MM==5 || MM==7 || MM==10 || MM==12)                //上月是小月份2月单独处理) 
            { 
                if(DD>1)    DD--; 
                else 
                { 
                    DD    =    30; 
                    MM    --; 
                } 
            } 
            else if(MM==3)    //处理上个月是2月份 
            { 
                if((DD==1) && IsLeapYear(YY)==0)                    //不是闰年 
                { 
                    DD    =    28; 
                    MM    --; 
                } 
                else    DD--; 
            } 
            else if(MM==1)    //处理1月份 
            { 
                if(DD>1)    DD--; 
                else        //新年第一天 
                {               
                    DD    =    31; 
                    MM    =    12; 
                    YY    --; 
                } 
            } 
        } 
    }         

    ConvertTime->year   =    YY;                //更新年 
    ConvertTime->mon    =    MM;                //更新月 
    ConvertTime->day    =    DD;                //更新日 
    ConvertTime->hour   =    hh;                //更新时 
    ConvertTime->min    =    mm;                //更新分 
    ConvertTime->sec    =    ss;                //更新秒 
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
