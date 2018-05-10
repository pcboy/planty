# Planty

# BOM
You basically need a moisture sensor like [this one](https://www.aliexpress.com/item/Crowtail-Moisture-Sensor-Module-DIY-Kit-with-3-Pin-Cable-Open-Source-Free-Shipping/32605171440.html?spm=2114.search0104.3.86.233c62e1jlUu0r&ws_ab_test=searchweb0_0,searchweb201602_2_10152_10151_10065_10344_10130_10068_5722815_10324_10342_10547_10343_10340_5722915_10341_10545_5722615_10696_10084_10083_10618_10307_5722715_5711215_10059_308_100031_10103_10624_10623_10622_5711315_5722515_10621_10620,searchweb201603_31,ppcSwitch_5&algo_expid=bdfea328-2821-43f2-b724-253a9e82eccd-12&algo_pvid=bdfea328-2821-43f2-b724-253a9e82eccd&priceBeautifyAB=0)

And an esp8266. Code works right now with the Huzzah Esp8266 from Adafruit. You may have to change the SENSOR\_PIN and SENSOR\_POWER\_PIN values in src/main.cpp if you have different pinouts.
You have to add your secrets (influxdb username and password to the planty database) in src/secrets.h  
Important thing to note. Because these moisture sensors tend to corrode easily you have to be careful for the sensor not to draw current continously. So the sensor is actually attached to a gpio pin (called SENSOR\_POWER\_PIN in my source) set to LOW 99% of the time, I set the pin to HIGH when I actually want a reading and set it back to LOW immediatly after to preserve the sensor.

# Upload to esp8266
Install [PlatformIO](https://platformio.org/) and then from the root directory do:

  pio run -t upload

# Influxdb
To init the database properly:
make influx password=whateveryouwant

# Grafana
For the nice graphs you can setup grafana to use InfluxDB data.

  docker-compose up -d grafana

Then you should be able to access grafana on http://localhost:4242/
user: admin, password: admin
