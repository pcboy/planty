#include <WiFi.h>
#include <HTTPClient.h>

#include "MedianFilter.h"
#include "secrets.h"

#include <Wire.h>
#include <DS3232RTC.h>



#define SENSOR_PIN 17
#define SENSOR_POWER_PIN 14
#define ALERT_VALUE 4096

DS3232RTC rtc;

void send_notification(int value) {
  WiFi.disconnect();
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi..");
  }
  Serial.println("local ip");
  Serial.println(WiFi.localIP());

  HTTPClient http;
  http.begin(INFLUXDB_URI);
  http.addHeader("Content-Type", "text/plain");
  http.setAuthorization(INFLUXDB_USER, INFLUXDB_PASSWORD);
  http.POST("planty,device=planty adc=" + String(value));
  http.end();
  WiFi.disconnect();
}

void report_temp() {
  MedianFilter medianFilter(5, 0);
  digitalWrite(SENSOR_POWER_PIN, HIGH);

  for (int i = 0; i < 5; ++i) {
    medianFilter.in(map(analogRead(SENSOR_PIN), 0, 1023, 0, 1000));
  }
  digitalWrite(SENSOR_POWER_PIN, LOW);

  int mean = medianFilter.getMean();

  Serial.printf("ADC: %i\n", mean);
  if (mean < ALERT_VALUE) {
    send_notification(mean);
  }
}


void init_rtc() {
  /*
  const char *compile_time = __DATE__" "__TIME__;
  struct tm tm;
  Serial.println(compile_time);
  strptime(compile_time, "%b %d %Y %H:%M:%S", &tm);
  time_t t = mktime(&tm);

  setTime(t);
  rtc.set(now());
  */
}

void setup() {
  Serial.begin(9600);
  while(!Serial) { }

  pinMode(SENSOR_POWER_PIN, OUTPUT);
  digitalWrite(SENSOR_POWER_PIN, LOW);//Set to LOW so no power is flowing through the sensor

  rtc.alarm(ALARM_2);
  rtc.setAlarm(ALM2_MATCH_HOURS, 13, 0, 0);
  rtc.squareWave(SQWAVE_NONE);
  rtc.alarmInterrupt(ALARM_2, true);

  report_temp();

  //digitalWrite(27, HIGH);
  pinMode(27, INPUT_PULLUP);

  // init_rtc(); //set the RTC at compile time
  esp_sleep_enable_ext0_wakeup(GPIO_NUM_27, 0);
  esp_deep_sleep_start();
}



void loop() {
  // Remove the esp_deep_sleep() in setup() to print the RTC time.
  tmElements_t myTime;
  rtc.read(myTime);
  Serial.printf("%i %i %i %i:%i:%i\n", myTime.Day, myTime.Month,
      myTime.Year, myTime.Hour, myTime.Minute, myTime.Second);
  delay(1000);
}
