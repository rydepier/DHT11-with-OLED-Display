/******************************************************************************
OLED DHT11 Humidity/Temperature Display using U8GLIB Library

visit https://code.google.com/p/u8glib/ for full details of the U8GLIB library and
full instructions for use.

by Chris Rouse Oct 2015

DHT11 portions Credit Rob Tillaart
https://github.com/RobTillaart/Arduino/tree/master/libraries/DHTlib

Using a IIC 128x64 OLED with DHT11
 
DHT11 Humidity/Temperature Sensor

Wire OLED:
  VCC +5v
  GND GND
  SDA Analog pin 4
  SCL Analog pin 5
Wire DHT11
  Vcc to Arduino 5 volts
  Gnd to Arduino Gnd
  Data to Arduino pin 2

******************************************************************************/

// Add libraries
  #include "U8glib.h"
  #include <SPI.h>
  #include <Wire.h>
  #include <dht.h> 
  
// setup u8g object
  U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_NONE);	// I2C 
//
  dht DHT;
  #define DHT11_PIN 2
  struct
 {
   uint32_t total;
   uint32_t ok;
   uint32_t crc_error;
   uint32_t time_out;
   uint32_t connect;
   uint32_t ack_l;
   uint32_t ack_h;
   uint32_t unknown;
 } stat = { 0,0,0,0,0,0,0,0};  
  
//
  int humidity;
  double temperature;
  double temperatureF;
  int dewPoint1;
  String thisTemp = "";
  String thisHumidity = "";
  String thisDewPoint = "";
//

void draw(void) {
  u8g.setFont(u8g_font_profont12);
  u8g.drawStr(29,10, "DHT11 Sensor");
  u8g.setFont(u8g_font_profont12); 
  // display Centigrade
  thisTemp = String(temperature) + "\260C";
  const char* newTempC = (const char*) thisTemp.c_str();
  u8g.drawStr(70,25, newTempC);
  // display Fahrenheit
  temperatureF = 1.8 * temperature + 32;
  thisTemp = String(temperatureF) + "\260F";
  const char* newTempF = (const char*) thisTemp.c_str();
  u8g.drawStr(15,25, newTempF);  
  
 // now display Humidity
  u8g.setFont(u8g_font_profont29); 
  thisHumidity = String(humidity) + "%";
  const char* newHumidity = (const char*) thisHumidity.c_str();
  u8g.drawStr(15,50, newHumidity);  
  u8g.setFont(u8g_font_profont12);
  u8g.drawStr(65,38, "humidity");
  
 // now display the dew point
  thisDewPoint = String(dewPoint1) + "\260C";
  const char* newDewPoint = (const char*) thisDewPoint.c_str();
  u8g.drawStr(85,50, newDewPoint);  
  u8g.setFont(u8g_font_profont12);
  u8g.drawStr(65,50, "DP="); 
}

void setup(void) {
  Serial.begin(9600);
  Wire.begin();
  Serial.println("DHT11");
  Serial.print("LIBRARY VERSION: ");
  Serial.println(DHT_LIB_VERSION);
  Serial.println();
}

void loop(void) {
  uint32_t start = micros();
  int chk = DHT.read11(DHT11_PIN);
  uint32_t stop = micros();
  stat.total++;
  switch (chk)
  {
    case DHTLIB_OK:
      stat.ok++;
      Serial.print("OK,\t");
      break;
    case DHTLIB_ERROR_CHECKSUM:
      stat.crc_error++;
      Serial.print("Checksum error,\t");
      break;
    case DHTLIB_ERROR_TIMEOUT:
      stat.time_out++;
      Serial.print("Time out error,\t");
      break;
    case DHTLIB_ERROR_CONNECT:
      stat.connect++;
      Serial.print("Connect error,\t");
      break;
    case DHTLIB_ERROR_ACK_L:
      stat.ack_l++;
      Serial.print("Ack Low error,\t");
      break;
    case DHTLIB_ERROR_ACK_H:
      stat.ack_h++;
      Serial.print("Ack High error,\t");
      break;
    default:
      stat.unknown++;
      Serial.print("Unknown error,\t");
      break;
  }
  temperature = DHT.temperature;
  humidity = DHT.humidity;
  dewPoint(temperature,humidity);  
  // picture loop
  u8g.firstPage();  
  do {
    draw();
  } while( u8g.nextPage() );
  // delay needed between sensor reads
  delay(2000);
}

double dewPoint(double celsius, double humidity)
{
  double a = 17.271;
  double b = 237.7;
  double temp = (a * celsius) / (b + celsius) + log(humidity/100);
  dewPoint1 = (b * temp) / (a - temp);
}
