/*
   Board = Adafruit Funhouse
   TFT Colors:  ST77XX_BLACK, GREEN, WHITE, YELLOW, BLUE, MAGENTA

   Barometer reference:  https://www.wikihow.com/Set-a-Barometer#:~:text=Predicting%20weather%20using%20a%20barometer%20is%20all%20about,release%20any%20pressure%20changes%20stored%20in%20the%20mechanisms.
   NTP TZ reference:  https://randomnerdtutorials.com/esp32-ntp-timezones-daylight-saving/
   List of timezones:  https://github.com/nayarsystems/posix_tz_db/blob/master/zones.csv
   
*/

#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ST7789.h> // Hardware-specific library for ST7789
#include <U8g2_for_Adafruit_GFX.h>
#include <SPI.h>

// Neopixels
#include <Adafruit_DotStar.h>
#define NUM_DOTSTAR 5
Adafruit_DotStar strip(NUM_DOTSTAR, PIN_DOTSTAR_DATA, PIN_DOTSTAR_CLOCK, DOTSTAR_BRG);
uint16_t firstPixelHue = 0;

// Library for EEPROM
#include <EEPROM.h>
#define EEPROM_SET 1 //Set to 0 if the EEPROM has never been written.  Then to 1 once EEPROM has data written.

#include "Settings.h"  //user settings

#include <ArduinoJson.h>//must be 6.x

// WiFi and AP
#ifdef ESP8266
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
ESP8266WebServer server(80);
#else
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
WebServer server(80);
#endif

WiFiClient client;
const int httpPort = 80;

#include <time.h>
#include <WiFiManager.h>

// Setup RTC variables
char buf[30];
time_t t = time(nullptr);
struct tm *timeinfo = localtime (&t);

// Setup DPS310 Temp / Pressure Sensor
#include <Adafruit_DPS310.h>
Adafruit_DPS310 dps;

//Setup ATTXO Temp / Humid Sensor
#include <Adafruit_AHTX0.h>
Adafruit_AHTX0 aht;

//Setup SHT4x Temp / Humid Sensor
#include "Adafruit_SHT4x.h"
Adafruit_SHT4x sht4 = Adafruit_SHT4x();

weather location;  //for weather by location

// Button setup
#include <Button2.h>
Button2 btn0 = Button2(BUTTON_UP);  //change location
Button2 btn1 = Button2(BUTTON_SELECT);  //change units
Button2 btn2 = Button2(BUTTON_DOWN);  //refresh screen
int caseselector = 0;
bool buttonpressed = false;
bool refreshScreen = true;

//Create display instances
Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RESET); // Use for funhouse
U8G2_FOR_ADAFRUIT_GFX u8display;  //must match what is used in the functions

//Functions in Functions and GetForecast Tab
void writeString(char add, String data);
String read_String(char add);
String strTime(time_t unixTime);
weather getForecast(weather w, int httpPort, String APIKEY);
void changeLocation(Button2 &btn);
void changeUnits(Button2 &btn);
void setLocation(int cs);
void fhtone(uint8_t pin, float frequency, float duration);
void rainbow(int wait);
int getSymbol(int id);

void setup() {
  Serial.begin(115200);

  // start newpixels
  strip.begin(); // Initialize pins for output
  strip.show();  // Turn all LEDs off ASAP
  strip.setBrightness(20);

  // assing button modes
  pinMode(BUTTON_DOWN, INPUT_PULLDOWN);
  pinMode(BUTTON_SELECT, INPUT_PULLDOWN);
  pinMode(BUTTON_UP, INPUT_PULLDOWN);

  //Map buttonaction to function
  btn2.setTapHandler(refresh);
  btn1.setTapHandler(changeLocation);
  btn0.setTapHandler(changeUnits);

  // Start TFT
  tft.init(240, 240); // Initialize ST7789 screen
  u8display.begin(tft);
  u8display.setFontMode(1);                 // use u8g2 transparent mode (this is default)
  u8display.setFontDirection(0);            // left to right (this is default)
  tft.fillScreen(ST77XX_BLUE);                              // clear screen
  u8display.setForegroundColor(ST77XX_BLACK);      // apply Adafruit GFX color
  u8display.setBackgroundColor(ST77XX_BLUE);

  // Initialize wifi
  WiFi.mode(WIFI_STA);
  Serial.printf("\n\nConnecting to %s\n", WIFI_SSID);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  //WiFiManager wifiManager;
  //wifiManager.autoConnect("FunhouseConnectAP");

  Serial.println(F("Connecting..."));
  u8display.setForegroundColor(ST77XX_BLACK);      // apply Adafruit GFX color
  u8display.setFont(u8g2_font_helvR12_tf);
  u8display.setCursor(5, 20);
  u8display.print("Starting WiFi .");
  u8display.setCursor(5, 40);
  // Wait for connection
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
    u8display.print(".");
  }

  // connect to NTP servers
  configTime(0, 0, NTP_SERVERS);  // must be executed after wifi
  //setenv("TZ", "GMT0BST,M3.5.0/1,M10.5.0", 1);  // set to GMT - match with the location, lon, lat set before setup()
  setenv("TZ", "PST8PDT,M3.2.0/2,M11.1.0", 1);  // set to Pacific Time - match to case select 0

  // start EEPROM
  EEPROM.begin(512);
  // read EEPROM for units and location setting - see settings.h
  //************************************************************************************************************************************
  // note:  if EEPROM has not been written with valid data, CPU may crash when reading from EEPROM.  To prevent this,
  // set EEPROM_SET false until intial EEPROM data is loaded.  Press both buttons to load EEPROM with valid data,
  // then then set EEPROM_SET true and reflash.
#if EEPROM_SET
  units = read_String(0);
  caseselector = EEPROM.read(20);
  setLocation(caseselector);
#endif
  //************************************************************************************************************************************

  // Set pin modes for TFT Backlight, LED, Speaker
  pinMode(TFT_BACKLIGHT, OUTPUT);
  digitalWrite(TFT_BACKLIGHT, HIGH); // Backlight on
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(SPEAKER, OUTPUT);

  // Config internal led
  ledcSetup(0, 2000, 8);
  ledcAttachPin(LED_BUILTIN, 0);

  // Config speaker
  ledcSetup(1, 2000, 8);
  ledcAttachPin(SPEAKER, 1);
  ledcWrite(1, 0);

  // Config DPS310 Temp / Pressure Sensor
  if (! dps.begin_I2C()) {
    Serial.println("Failed to find DPS");
    while (1) yield();
  }
  Serial.println("DPS OK!");
  dps.configurePressure(DPS310_64HZ, DPS310_64SAMPLES);
  dps.configureTemperature(DPS310_64HZ, DPS310_64SAMPLES);

  // Config AHTX0 Temp / Humidity Sensor
  if (! aht.begin()) {
    Serial.println("Could not find AHT? Check wiring");
    while (1) delay(10);
  }
  Serial.println("AHT10 or AHT20 found");

  // Config SHT4x Temp / Humidity Sensor
  if (! sht4.begin()) {
    Serial.println("Couldn't find SHT4x");
    while (1) delay(1);
  }
  Serial.println("Found SHT4x sensor");
  sht4.setPrecision(SHT4X_HIGH_PRECISION);
  sht4.setHeater(SHT4X_NO_HEATER);

  // rainbow dotstars
  rainbow(1);
  strip.clear();
  strip.show();
}

void loop() {

  // update interval timer
  unsigned long currentTime = millis();

  // update button loop
  btn2.loop();
  btn1.loop();
  btn0.loop();

  // Set desired location for weather
  location.lat = latitude;
  location.lon = longitude;
  location.loc = currlocation;
  location.units = units;

  // Update weather

  if (currentTime - previousWeatherTime > weatherInterval || refreshScreen == true)
  {
    previousWeatherTime = currentTime;  //reset timer

    location = getForecast( location, httpPort, APIKEY );
    Symbol = getSymbol(location.curr_id);  // required to serial print the weather forecast
    SerialPrintForecast();

    tft.fillScreen(ST77XX_WHITE);                              // clear screen every 15 min

    //    u8display.setFontMode(1);                 // use u8g2 transparent mode (this is default)
    //    u8display.setFontDirection(0);            // left to right (this is default)
    u8display.setForegroundColor(ST77XX_BLACK);      // apply Adafruit GFX color
    u8display.setBackgroundColor(ST77XX_WHITE);

    // Draw Forecast DOW (Mon, Tues, etc) : row 15
    u8display.setFont(u8g2_font_helvR14_tf);  // select u8g2 font from here: https://github.com/olikraus/u8g2/wiki/fntlistall
    uint16_t y_top = 15;  // row 15
    // issue:  if these for loops are combined, the days info only shows the first day...
    for (int i = 0; i <= (MAX_DAYS - 1); i++) {
      u8display.setCursor(3 + i * 51, y_top); //set to row 10 for DOW
      u8display.print(strTime(location.dt[i]).substring(0, 3));
    }
    // Draw Forecast Glyphs  - row 35
    // font is defined in drawWeatherSymbol()
    uint16_t y_symb = y_top + 20;
    for (int i = 0; i <= (MAX_DAYS - 1); i++) {
      drawWeatherSymbol(12 + i * 50, y_symb, getSymbol(location.id[i])); //drawGlyph(x,y, Glyph number), x, y is the lower left corner of the glyph.
    }

    // Draw Forecast Hi / Lo temps - row 42
    u8display.setFont( u8g2_font_helvR08_tf  );
    uint16_t y_hilo = y_symb + 12;
    for (int i = 0; i <= (MAX_DAYS - 1); i++) {
      String highTemp = String(location.temp_max[i], 0);
      String lowTemp = String(location.temp_min[i], 0);
      u8display.setCursor(6 + i * 50, y_hilo); //set to row 42 for Max / Min temp
      u8display.print(highTemp + " " + lowTemp);
    }

    //********************************************************
    // Draw Current Weather  rows 62 - 100
    //********************************************************

    // Draw Current Weather Glyph - row 100, left margin
    u8display.setForegroundColor(0x1F);  // set to blue foreground
    uint16_t y_glyph = y_hilo + 58;
    drawWeatherSymbolBig(5, y_glyph, getSymbol(location.curr_id));
    //u8display.setForegroundColor(ST77XX_BLACK);  // set to black foreground

    // Draw location - row 62 - right half
    u8display.setFont( u8g2_font_helvB12_tf  );
    y_loc = y_hilo + 20;
    u8display.setCursor(130, y_loc);
    u8display.print(location.loc);

    // Draw Current Condition - row 67 - right half
    u8display.setForegroundColor(0x1F);  // set to blue foreground
    u8display.setFont( u8g2_font_helvB12_tf  );
    uint16_t y_main = y_hilo + 25;
    u8display.setCursor(60, y_main);
    // Thunderstorm is too long for the space available, so replace with T-storm
    String currentWeather = location.curr_weat;
    if (currentWeather == "Thunderstorm") {
      currentWeather = "T-storm";
    }
    u8display.print(currentWeather);

    // Draw Current Temp - row 83 - right half
    uint16_t y_temp = y_main + 16;
    u8display.setCursor(60, y_temp);
    if (location.units == "imperial") {
      u8display.print(String(location.curr_temp, 1) + "°F");   //Alt + 0176 for degree symbol
    } else
    {
      u8display.print(String(location.curr_temp, 1) + "°C");
    }

    // Draw Current Humidity - row 98 - right half
    uint16_t y_humid = y_temp + 15;
    u8display.setCursor(60, y_humid);
    u8display.printf("%*.0f%%\n", 2, location.curr_humid);  //printf works too

    refreshScreen = true;  // set to true for clock update after the 15 min mark
  }

  //  ********************************************************
  //   Draw Time Info :  Col 140, Row 74
  //  ********************************************************
  if (currentTime - previousClockTime > clockInterval || refreshScreen == true)
  {
    // cannot clear screen every second so erase prev before drawing new info
    previousClockTime = currentTime;  //reset timer
    t = time(nullptr);
    tm *timeinfo = localtime (&t);  //Update time from local RTC

    dd = timeinfo->tm_mday;
    hh = timeinfo->tm_hour;
    mm = timeinfo->tm_min;
    ss = timeinfo->tm_sec;

    String month[12] = {"Jan", "Feb", "Mch", "Apr", "May", "Jun", "Jly", "Aug", "Sep", "Oct", "Nov", "Dec"};

    uint16_t xpos = 130;
    uint16_t ypos = y_loc + 30;
    int ysecs = ypos + 0;

    //  ********************************************************
    //   Draw Hour, Colon, Minutes every minute
    //  ********************************************************
    if (omm != mm || refreshScreen == true) {
      omm = mm;  // reset old mm
      uint8_t hh12 = (hh + 11) % 12 + 1; // take care of noon and midnight-- converts 24h time to 12h time
      u8display.setFont(  u8g2_font_crox5h_tr  );                    // Select the font

      // Hours ********************************************
      if (units == "imperial") {
        u8display.setForegroundColor(ST77XX_WHITE);
        sprintf(buf, "%02d", hh12_p);
        u8display.drawStr( xpos, ypos, buf);     //erase previous hours
        u8display.setForegroundColor(0x1F);      // Set colour to blue
        sprintf(buf, "%02d", hh12);
        hh12_p = hh12;
        xpos += u8display.drawStr(xpos, ypos, buf); // Draw new hours
      } else {
        u8display.setForegroundColor(ST77XX_WHITE);
        sprintf(buf, "%02d", hh_p);
        u8display.drawStr( xpos, ypos, buf);     //erase previous hours
        u8display.setForegroundColor(0x1F);      // Set colour to blue
        sprintf(buf, "%02d", hh);
        hh_p = hh;
        xpos += u8display.drawStr(xpos, ypos, buf); // Draw new hours
      }

      // Colon***********************************************
      xcolon = xpos;                                         // Save colon coord for later to flash on/off later
      u8display.setForegroundColor(ST77XX_WHITE);
      xpos += u8display.drawStr( xpos, ypos, ":");           // Draw placeholder, flash later

      // Minutes*******************************************
      sprintf(buf, "%02d", mm_p);
      u8display.drawStr( xpos, ypos, buf);          // Erase previous
      u8display.setForegroundColor(0x1F);           // Set colour to blue
      sprintf(buf, "%02d", mm);
      mm_p = mm;
      xpos += u8display.drawStr(xpos, ypos, buf);   // Draw minutes
      xsecs = xpos;                                 // Save seconds 'x' position for later display updates

      // am - pm***************************************
      if (units == "imperial") {
        u8display.setFont( u8g2_font_helvB12_tf  ); // Select the font
        u8display.drawStr( xpos + 2, ypos + 0, hh >= 12 ? "pm" : "am");
      }
      //Serial.printf("%d:%d\n", hh12, mm);
    }

    //  ********************************************************
    //   Flash colon, draw seconds every second
    //  ********************************************************
    if (oss != ss)
    {
      oss = ss;
      // Flash the colons on/off
      xpos = xsecs;
      u8display.setFont(  u8g2_font_crox5h_tr );            // Select the font
      if (ss % 2)
      {
        u8display.setForegroundColor(ST77XX_WHITE);         // Set colour to white
        u8display.drawStr( xcolon, ypos, ":");              // Erase hour:minute colon
        u8display.setForegroundColor(0x1F);                 // Set colour back to blue
      }
      else
      {
        u8display.setForegroundColor(0x1F);      // Set colour back to blue
        u8display.drawStr( xcolon, ypos, ":");   // Hour:minute colon
      }
      //Draw seconds
      u8display.setFont( u8g2_font_helvB12_tf );                // Select the font
      u8display.setForegroundColor(ST77XX_WHITE);               // erase prev ss
      sprintf(buf, "%02d", ss_p);
      u8display.drawStr( xpos + 2, ysecs - 12, buf);            // erase old seconds
      u8display.setForegroundColor(0x1F);                       // Set colour to blue
      sprintf(buf, "%02d", ss);
      ss_p = ss;
      u8display.drawStr( xpos + 2, ysecs - 12, buf);            // Draw new seconds
    }

    //  ********************************************************
    //   Draw date once a day
    //  ********************************************************
    if (odd != dd || refreshScreen == true) {
      odd = dd;
      u8display.setFont( u8g2_font_helvB10_te );
      u8display.setCursor(130, ypos + 15);
      u8display.printf("%02d %s %04d", timeinfo->tm_mday, month[timeinfo->tm_mon], timeinfo->tm_year + 1900);
    }
  }

  //  *****************************************************************
  //   Draw Temp, Humidity and Pressure from Sensors
  //  *****************************************************************
  if (currentTime - previousSensorTime > sensorInterval  || refreshScreen == true)
  {
    previousSensorTime = currentTime;
    sensors_event_t humidity, aht_temp, pressure, dps_temp, sht_temp, sht_humid;

    aht.getEvent(&humidity, &aht_temp);  // populate temp and humidity objects with fresh data
    dps.getEvents( &dps_temp, &pressure);
    Serial.printf("Sensors: %0.1f *C  %0.1f %%  %0.2f hPa\n", aht_temp.temperature, humidity.relative_humidity, pressure.pressure);

    /*
       Falling
      If the reading is over 30.2 inches of mercury and falling rapidly, this indicates cloudy, but warmer weather.
      If the reading is between 29.8 and 30.2 inches of mercury and falling rapidly, rain is most likely on the way.
      If it's under 29.8 inches of mercury and falling slowly, rain is likely; if it's falling rapidly, a storm is imminent.

       Rising
      Readings over 30.2 inches of mercury that rise indicate that the weather will continue to be fair.
      Readings between 29.8 and 30.2 inches of mercury that rise indicate that the weather will remain whatever it presently is.
      Readings under 29.8 inches of mercury that rise indicate that the weather is clearing, but will be cooler.

       Steady
      A strong high pressure system is around 30.4 inches of mercury. Anything above 30 is considered high pressure.  Expect more warm weather.
      A typical low pressure system is around 29.5 inches of mercury. Anything below 29.9 is considered low pressure.  Expect cooler weather.
    */

    Serial.println("currentTime / omm20:");
    Serial.println(.001 * currentTime);
    Serial.println(.001 * omm20);
    Serial.println(.001 * (currentTime - omm20));

    if (currentTime - omm20 >= omm20Interval || refreshScreen == true) {
      omm20 = currentTime;
      deltapressure = pressure.pressure - prevpressure;
      prevpressure = pressure.pressure;
      Serial.printf("prevpressure:  %0.2f\n", prevpressure);
      if (deltapressure > 0) pTrend = RISING_P;  //str_pressTrend = "Rising";
      if (deltapressure > 900) pTrend = NO_VALUE;  //str_pressTrend = "No Data";
      if (deltapressure < 0) pTrend = FALLING_P;  //str_pressTrend = "Falling";
      if (deltapressure <= -0.85)  pTrend = FAST_FALLING_P;  //str_pressTrend = "Rapidly Falling";  // <-.025 inHg in 20 min is falling rapidly (.025 inHg = .85 hPa)
      if (deltapressure <= 0.1 && deltapressure >= -0.1) pTrend = STEADY;  //str_pressTrend = "Steady";

      switch (pTrend) {
        case NO_VALUE:
          str_pressTrend = "No Data";
          break;
        case RISING_P:
          if (pressure.pressure * 0.02953 > 30.2 /*inHg*/) str_pressTrend = "Rising: Fair Weather";
          if (pressure.pressure * 0.02953 > 29.8 && pressure.pressure * 0.02953 <= 30.2 /*inHg*/) str_pressTrend = "Rising: Steady";
          if (pressure.pressure * 0.02953 <= 29.8 /*inHg*/) str_pressTrend = "Rising: Cooling";
          break;
        case FALLING_P:
          if (pressure.pressure * 0.02953 < 28.9 /*inHg*/)
          {
            str_pressTrend = "Mercury Falling: Rain";
          } else {
            str_pressTrend = "Mercury Falling: Change";
          }
          break;
        case FAST_FALLING_P:
          if (pressure.pressure * 0.02953 > 30.2 /*inHg*/) str_pressTrend = "Falling Fast: Cloudy and Warming";
          if (pressure.pressure * 0.02953 > 29.8 && pressure.pressure * 0.02953 <= 30.2 /*inHg*/) str_pressTrend = "Falling Fast: Rain";
          if (pressure.pressure * 0.02953 >= 29.8 /*inHg*/) str_pressTrend = "Falling Fast: Storm";
          break;
        case STEADY:
          if (pressure.pressure * 0.02953 > 30 /*inHg*/) str_pressTrend = "Steady: Hi Press - Warming";
          if (pressure.pressure * 0.02953 <= 30 /*inHg*/) str_pressTrend = "Steady: Lo Press - Cooling";
          break;
        default:
          str_pressTrend = "Unpredicable";
          break;
      }

      Serial.printf("Delta pressure:  %0.2f\n", deltapressure);
      Serial.println("Pressure Trend: " + str_pressTrend);

      uint16_t xpos = 2;  uint16_t ypos = y_loc + 70;
      u8display.setFont( u8g2_font_helvB14_tf );                // Select the font
      u8display.setForegroundColor(ST77XX_BLACK);               // Set to bacckground
      // Draw divider line
      tft.drawFastHLine(xpos, ypos - 6, 80, ST77XX_BLACK);
      u8display.drawStr(xpos + 90, ypos, " local ");
      tft.drawFastHLine(xpos + 160 , ypos - 6, 80, ST77XX_BLACK);
      // Draw barometric pressure
      xpos = 2;
      ypos += 20;
      if (units == "imperial") {
        u8display.setForegroundColor(ST77XX_WHITE);            // Set to bacckground
        sprintf(buf, "Barometer: %0.2f inHg", press_p);
        u8display.drawStr( xpos, ypos, buf);                   // erase old
        u8display.setForegroundColor(ST77XX_MAGENTA);          // Set colour to magenta
        sprintf(buf, "Barometer: %0.2f inHg", pressure.pressure * 0.02953);
        u8display.drawStr( xpos, ypos, buf);                   // Draw new
        press_p = 0.02953 * pressure.pressure;
      } else {
        u8display.setForegroundColor(ST77XX_WHITE);            // Set to bacckground
        sprintf(buf, "Barometer: %0.2f hPA", press_p );
        u8display.drawStr( xpos, ypos, buf);                   // erase old
        u8display.setForegroundColor(ST77XX_MAGENTA);          // Set colour to magenta
        sprintf(buf, "Barometer: %0.2f hPA", pressure.pressure);
        u8display.drawStr( xpos, ypos, buf);                   // Draw new
        press_p = pressure.pressure;
      }


      // Draw forcast from pressure trend
      ypos = ypos + 20;
      xpos = 2;
      u8display.setFont( u8g2_font_helvB12_tf );                // Select the font
      u8display.setForegroundColor(ST77XX_WHITE);               // Set to bacckground
      u8display.drawStr( xpos, ypos, str_prev_pressTrend.c_str());  // Erase old
      u8display.setForegroundColor(ST77XX_MAGENTA);             // Set colour to magenta
      u8display.drawStr( xpos, ypos, str_pressTrend.c_str());   // Draw new
      str_prev_pressTrend = str_pressTrend;

    }


    // Draw inside temp / humidity
    sht4.getEvent(&sht_humid, &sht_temp);// populate temp and humidity objects with fresh data
    Serial.printf("Sensors: %0.1f *C  %0.1f %%  %0.2f hPa\n", sht_temp.temperature, sht_humid.relative_humidity, pressure.pressure);

    //    u8display.setFont( u8g2_font_helvB14_tf );              // Select the font
    //    uint16_t ypos = y_loc + 120;
    //    uint16_t xpos = 6;
    //    u8display.setForegroundColor(ST77XX_BLACK);             // Set colour to black
    //    u8display.drawStr( xpos, ypos, "Inside Temp / Humidity");

    u8display.setFont( u8g2_font_inb16_mf );                       // Select the font
    uint16_t xpos = 16; uint16_t ypos = y_loc + 140;
    u8display.setCursor(xpos, ypos);

    if (location.units == "imperial") {
      u8display.setForegroundColor(ST77XX_WHITE);                  // Set to bacckground
      u8display.printf("%0.1f°F | ", temp_p);                      // erase old  // had to use printf because drawStr mangles the degree symbol
      u8display.setCursor(xpos, ypos);
      u8display.setForegroundColor(ST77XX_BLUE);                          // Set colour to blue
      u8display.printf("%0.1f°F | ", 1.8 * sht_temp.temperature + 32);    // Draw new
      temp_p = 1.8 * sht_temp.temperature + 32;
    } else
    {
      u8display.setForegroundColor(ST77XX_WHITE);                  // Set to bacckground
      u8display.printf("%0.1f°C | ", temp_p);                      // erase old  // had to use printf because drawStr mangles the degree symbol
      u8display.setCursor(xpos, ypos);
      u8display.setForegroundColor(ST77XX_BLUE);                   // Set colour to blue
      u8display.printf("%0.1f°C | ", sht_temp.temperature);        // Draw new
      temp_p = sht_temp.temperature;
    }

    //    u8display.setForegroundColor(ST77XX_WHITE);              // Set to bacckground
    //    u8display.printf("%0.1f°C | ", temp_p);                  // erase old  // had to use printf because drawStr mangles the degree symbol
    //    u8display.setCursor(xpos, ypos);
    //    u8display.setForegroundColor(ST77XX_BLUE);               // Set colour to blue
    //    u8display.printf("%0.1f°C | ", sht_temp.temperature);    // Draw new
    //    temp_p = sht_temp.temperature;

    xpos += 120;
    u8display.setForegroundColor(ST77XX_WHITE);                    // Set to bacckground
    sprintf(buf, "%0.1f%%  ", humid_p);
    u8display.drawStr( xpos, ypos, buf);                           // erase old
    u8display.setForegroundColor(ST77XX_BLUE);                     // Set colour to blue
    sprintf(buf, "%0.1f%%  ", sht_humid.relative_humidity);
    u8display.drawStr( xpos, ypos, buf);                           // Draw new
    humid_p = sht_humid.relative_humidity;

  }

  if (refreshScreen == true) {
    //fhtone(SPEAKER, 988.0, 100.0);   // tone1 - B5
    //fhtone(SPEAKER, 1319.0, 200.0);  // tone2 - E6
    // rainbow dotstars
    rainbow(1);
    strip.clear();
    strip.show();
  }

  if (refreshScreen == true) refreshScreen = false;  // after first pass set refresh to false

  // this has to be at the end because setLocation() sets refreshScreen = true
  if (buttonpressed == true)  //set to true when BUTTON_SELECT is pressed
  {
    buttonpressed = false;
    setLocation(caseselector);
    Serial.print("refreshScreen = "); Serial.println(refreshScreen);
  }

}
