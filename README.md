# FunHouse WeatherClock
Adafruit FunHouse Board for a Weather and Clock Application switchable between four cities.

## Overview
Weather forecast and NTP time for four locations along with local temperature, humidity and barometric pressure reading.

Built in buttons are used to 
1) change the location for the weather and time, 
2) select units between imperial or metric,
3) refresh the screen. 

![Front2](https://user-images.githubusercontent.com/31633408/174464779-ee85dcc4-2e8f-41f0-8518-15dc01a1880d.JPG)

![Screen_small](https://user-images.githubusercontent.com/31633408/174464920-fc87abf3-fdfa-4342-92e4-6fc905d1d3a2.JPG)

![Back](https://user-images.githubusercontent.com/31633408/174464943-9e43162b-49e0-4f00-92d4-61fe47b8f2e5.JPG)

## Parts Used
*  Adafruit Funhouse Devboard:  https://www.adafruit.com/search?q=funhouse
*  Adafruit SHT40 Temp / Humidity Sensor:  https://www.adafruit.com/search?q=sht040
*  3D printed faceplate \n
   -  Fusion 360 - FaceCover v11.f3d file
   -  3D Printer Slicer file - FaceCover v11.3mf 
  
## Code
The loop() function is structured in four tasks:
1)  Get weather forecast and update the weather display every 15 min
2)  Get time and update the time display every sec
3)  Read the SHT40 Temp / Humid sensors and update the display every 30 sec
4)  Read and update the barometric pressure every 20 min

### 1)  Weather  
The weather task takes up the most lines of code and is complex, but can be broken down as follows.  
The task first calls the getForecast function which does these three things:
*  Send a properly formatted get string to api.openweathermap.org
*  Parse the JSON response - the parsing code was created using the ArduinoJSON assistant (ref:  https://youtu.be/NYP_CxdYzLo and arduinojson.org )
*  Copy the variables to be displayed into the a global struct variable named "weather".

Then, on return from the getForecast function, the task displays the weather data row by row.  The getSymbol function maps the weather id returned from the API call into one of the weather types (SUN, CLOUDS, etc).  The drawWeatherSymbol function then maps this to an associated glyph and displays it to the screen.

### 2)  Time  
Time is updated using the network time protocal (NTP) servers and requires internet access.  The library time.h is required.  Here is what the code does:
*  Connects to a WiFi network
*  Configures the NTP servers using configTime() - example: configTime(0, 0, "pool.ntp.org"); 
*  Updates the time using time() - example: time_t tnow = time(nullptr);
*  Sets the time zone using setenv() - example:  setenv("TZ", "PST8PDT,M3.2.0/2,M11.1.0/2", 1);
*  Converts the time to local time and into a struct variable using localtime() - example:  struct tm *timeinfo = localtime(&tnow);
*  Draws the time and date to the screen

Here are some great references:
https://www.tutorialspoint.com/c_standard_library/time_h.htm
https://github.com/SensorsIot/NTP-time-for-ESP8266-and-ESP32

### 3)  Sensor Temp / Humid  


