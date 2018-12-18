/*
 * ESP8266 driven matrix clock with weather
 * (c)2016-2018 Adam Kovesdi
 * version 3
 *
 * TODO: missing weather functionality... in progress of rewrite
 *
 * required libraries:
 * ArduinoJson version 5(!) for weather functionality
 */

// set these to your credentials (otherwise it will use mine - not included)

// #define WEATHERKEY	"yourapikey"	// openweathermaps API key
// #define CITYID			"yourcity"		// openweathermaps city ID

#ifndef WEATHERKEY
#include "credentials.h"
#endif

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>

#include "ntp.h"
#include "fonts.h"
#include "max7219.h"
#include "display.h"
#include "weather.h"
#include "setupwifi.h"

// ntp stuff
NTP NTPclient;
#define CET 1 // central european time

// weather related stuff
unsigned long lastwupdate=0;		// last weather update EPOCH timestamp
#define WEATHER_POLL_INTERVAL	30 // in seconds

time_t getNTPtime(void)
{
  return NTPclient.getNtpTime();
}

void refresh_weather()
{
	if(now() > (lastwupdate + WEATHER_POLL_INTERVAL))
	{
		lastwupdate = now();
		getWeatherData(WEATHERKEY, CITYID);
	}
}

void display_weather()
{
	refresh_weather();
	clr();
	int t=getTemp();
	char buf[6];
	sprintf(buf,"%dC",t);
	drawString(0,font,buf);
	drawChar(24,weather_icons,getWeatherIcon());
	drawChar(18,winddir_icons,getWinddir());
	for(int i=0;i<getWindspeed();i++) setPixel(16+i,7,true);
	refreshAll();
}

void display_clock()
{
	draw_clock(hour(), minute(), second());
}

void setup()
{
	initMAX7219();
	drawString(0,font," Conn...");

	Serial.begin(9600);
	Serial.println(); Serial.println();

	while (!startWiFi()) { delay(1500); }
	Serial.println("WiFi connected");
	Serial.println("IP address: ");
	Serial.println(WiFi.localIP());
	Serial.println();

  NTPclient.begin("hu.pool.ntp.org", CET);
  setSyncInterval(SECS_PER_HOUR);
  setSyncProvider(getNTPtime);

	drawString(0,font,"link up");
}

void loop()
{
	uint8_t	timeslice=now() % 20;
	if ((timeslice >4) && (timeslice <8))
	{
		display_weather();
	}
	else
	{
		display_clock();
	}
}


