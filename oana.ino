#include <Arduino.h>
#include <U8g2lib.h>
#include <WiFi.h>
#include "time.h"
#include <HTTPClient.h>

#ifdef U8X8_HAVE_HW_SPI
#include <SPI.h>
#endif
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "DHT.h"
#define DHTPIN 21
#define DHTTYPE DHT21

U8G2_SSD1306_128X64_NONAME_F_SW_I2C u8g2(U8G2_R0, /* clock=*/ 15, /* data=*/ 4, /* reset=*/ 16);

//const char* ssid       = "UPCFB8C751";
//const char* password   = "npz6jzMpbjzw";

const char* ssid       = "Idrive";
const char* password   = "idrive@2019";

const char* ntpServer = "pool.ntp.org";
const long  gmtOffset_sec = 7200;
const int   daylightOffset_sec = 3600;

//const String endpoint = "https://api.weather.com/v3/wx/forecast/daily/5day?geocode=44.4732783,26.1243366&format=json&units=m&language=en-US&apiKey=";
//const String key = "68dd1425d11943d29d1425d11913d279";

const String endpoint = "http://api.openweathermap.org/data/2.5/weather?q=Bucharest&APPID=";
const String key = "acfaec53daed4a1a7c2506c75e673885";

DHT dht(DHTPIN, DHTTYPE);

void setup(void) {
  u8g2.begin();
  Serial.begin(115200); 
  dht.begin();

  int retries = 0;
  //connect to WiFi
  Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    if (retries == 10) {
      break;
    }
    delay(500);
    Serial.print(".");
    retries++;
  }
  Serial.println(" CONNECTED");
  
  //init and get the time
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);  
  
  printLogo();

  if ((WiFi.status() == WL_CONNECTED)) {
    printWeather();
  }

  //disconnect WiFi as it's no longer needed
  WiFi.disconnect(true);
  WiFi.mode(WIFI_OFF);
}

void printLogo(void){
    u8g2.clearBuffer();
    u8g2.setFontMode(1);  // Transparent

    u8g2.setFontDirection(0);
    //u8g2.setFont(u8g2_font_inb16_mf);
    u8g2.setFont(u8g2_font_unifont_t_extended);
    u8g2.drawStr(0,20, "Weather");
    u8g2.drawUTF8(0,40, "Pârț");

    u8g2.sendBuffer();  
    //delay(1000);
}

void printCalendar(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }

  char myCalendar[15];
  strftime(myCalendar, sizeof(myCalendar), "%a, %b %d %y %H:%M", &timeinfo);
  
  //Serial.println(myCalendar);
  u8g2.setFont(u8g2_font_chroma48medium8_8u);
  u8g2.drawStr(0, 15, myCalendar);  
}

void printTime(){
  struct tm timeinfo;
  if(!getLocalTime(&timeinfo)){
    Serial.println("Failed to obtain time");
    return;
  }
  
  char myTime[15];
  strftime(myTime, sizeof(myTime), "%H:%M:%S", &timeinfo);
  
  //Serial.println(myTime);
  u8g2.setFont(u8g2_font_chroma48medium8_8u);
  u8g2.drawStr(0, 25, myTime);  
}

void printWeather(){ 
  u8g2.clearBuffer();
  
  u8g2.setFontMode(1);  // Transparent
  u8g2.setFontDirection(0);    
  u8g2.setFont(u8g2_font_unifont_t_extended);
    
  HTTPClient http;
  
  http.begin(endpoint + key);
  int httpCode = http.GET();
  
  if (httpCode > 0) { 
    String payload = http.getString();
    Serial.println("httpCode");
    Serial.println(httpCode);
    Serial.println("payload");
    Serial.println(payload);
  
  } else {
    Serial.println("Error on HTTP request");
  }

  http.end();

  u8g2.sendBuffer();
  delay(1000);
}

void loop() {
  u8g2.clearBuffer();
  
  printCalendar();
  printTime();
  //printWeather();
  
  u8g2.sendBuffer();  
  delay(1000);
}
