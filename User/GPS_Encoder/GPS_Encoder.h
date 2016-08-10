/**
  ********************************************
	*   @file     GPS_Encoder.h
  *   @author   SRn
  *   @date     2015.8.16
  *   @version  V1.0
  *   @brief    GPSÊý¾Ý½âÂë
	*********************************************
	*/
#ifndef __GPS_ENCODER__
#define __GPS_ENCODER__
#include "nmea/nmea.h"
#include "stm32f10x.h"
typedef struct 
{
	float lastlon;
	float lastlat;
	float currentlon;
	float currentlat;
	float lastalltitude;
	float currentalltitude;
	double allmile;
	double allalltitude;
	
} userDATA;

void GPS_Enco(char *buff,int buff_size);
void GMTconvert(nmeaTIME *SourceTime, nmeaTIME *ConvertTime, uint8_t GMT,uint8_t AREA);
int GetMiles(userDATA * userdata, nmeaINFO * nmeaINFO);
float userabs(float a);








#endif
