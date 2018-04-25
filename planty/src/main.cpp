#include <WiFiManager.h>
#include <DNSServer.h>            //Local DNS Server used for redirecting all requests to the configuration portal
#include <ESP8266WebServer.h>     //Local WebServer used to serve the configuration portal
#include <ESP8266HTTPClient.h>

#include <ESP8266WiFi.h>

#include "MedianFilter.h"

#define SENSOR_PIN 17
#define ALERT_VALUE 4096
#define RTC_BASE 65
#define SLEEP_CYCLE 4294967295 // 0xffffffff

int sleepCount = 0;
byte magicNumber[2] = {0x42, 0x42};
byte buf[2];
int readings[10] = {0};

void send_notification(int value) {
  HTTPClient http;
  http.begin("http://192.168.138.180:8086/write?db=planty");
  http.addHeader("Content-Type", "text/plain");
  http.setAuthorization("planty", "test");
  int httpCode = http.POST("planty,device=planty adc=" + String(value));
  http.end();
}

void report_temp() {
  MedianFilter medianFilter(10, 0);
  int total = 0;

  for (int i = 0; i < 10; ++i) {
    medianFilter.in(analogRead(SENSOR_PIN));
    delay(1000);
  }
  int mean = medianFilter.getMean();

  Serial.printf("ADC: %i\n", mean);
  if (mean < ALERT_VALUE) {
    send_notification(mean);
  }
}


uint32_t checkForResetButtonPressed() {
  rst_info *myResetInfo;
  myResetInfo = ESP.getResetInfoPtr();
  Serial.printf("myResetInfo->reason %x \n", myResetInfo->reason);
  Serial.flush();
  return myResetInfo->reason;
}

void setup() {
  Serial.begin(9600);
  // Wait for serial to initialize.
  while(!Serial) { }
  Serial.println("------");

  WiFi.forceSleepBegin();  // send wifi to sleep to reduce power consumption

  Serial.println("wake up");
  system_rtc_mem_read(RTC_BASE, &buf, sizeof(buf));
  if (buf[0] == 0x42 && buf[1] == 0x42 && checkForResetButtonPressed() == REASON_DEEP_SLEEP_AWAKE) { // Not First initialization
    Serial.println("not first initialization");
    system_rtc_mem_read(RTC_BASE + sizeof(magicNumber), &sleepCount, sizeof(sleepCount));
  } else {
    Serial.println("first initialization");
    system_rtc_mem_write(RTC_BASE, &magicNumber, sizeof(magicNumber));
    system_rtc_mem_write(RTC_BASE + sizeof(magicNumber), &sleepCount, sizeof(sleepCount));
  }
  WiFiManager wifiManager;
  wifiManager.autoConnect("PLANTY");

  Serial.println("local ip");
  Serial.println(WiFi.localIP());
  Serial.printf("sleepCount: %i\n", sleepCount);

  if (sleepCount == 0) {
    report_temp();
  }
  if (sleepCount < 24) {
    sleepCount++;
  } else {
    sleepCount = 0;
  }
  system_rtc_mem_write(RTC_BASE + sizeof(magicNumber), &sleepCount, sizeof(sleepCount));
  ESP.deepSleep(SLEEP_CYCLE);
}



void loop() {


}
