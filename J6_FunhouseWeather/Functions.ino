
//***********************************************************
// Functions
//***********************************************************
/***************************************************************************************
**                          Convert id to weather symbol
***************************************************************************************/
int getSymbol(int id) {
  //Maps weather id to symbol
  int symbol = 0;

  if ((id >= 200) && (id <= 232)) {
    symbol = THUNDER;
  }
  else if ((id >= 300) && (id <= 531)) {
    symbol = RAIN;
  }
  else if ((id >= 600) && (id <= 622)) {
    symbol = SNOW;
  }
  else if ((id >= 700) && (id <= 781)) {
    symbol = MIST;
  }
  else if (id == 800) {
    symbol = SUN;
  }
  else if ((id >= 801) && (id <= 804)) {
    symbol = CLOUD;
  }

  return symbol;
}

/********************************************************************************************************************************
   Draw Weather Symbols
 * ******************************************************************************************************************************/
void drawWeatherSymbol(int x, int y, int symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // 1x = 8x8 px, 2x = 16x16 px ... 6x = 48x48 px,  8x = 64x64 px
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic

  switch (symbol)
  {
    case SUN:
      u8display.setFont(u8g2_font_open_iconic_weather_2x_t);  // 16 px
      u8display.drawGlyph(x, y, 69);
      break;
    case SUN_CLOUD:
      u8display.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8display.drawGlyph(x, y, 65);
      break;
    case CLOUD:
      u8display.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8display.drawGlyph(x, y, 64);
      break;
    case RAIN:
      u8display.setFont(u8g2_font_open_iconic_weather_2x_t);
      u8display.drawGlyph(x, y, 67);
      break;
    case THUNDER:
      u8display.setFont(u8g2_font_open_iconic_embedded_2x_t);
      u8display.drawGlyph(x, y, 67);
      break;
    case MIST:
      u8display.setFont(u8g2_font_open_iconic_other_2x_t);
      u8display.drawGlyph(x, y, 67);
      break;
    case SNOW:
      u8display.setFont(u8g2_font_open_iconic_embedded_2x_t);
      u8display.drawGlyph(x, y, 66);
      break;
  }
}

/********************************************************************************************************************************
   Draw Big Weather Symbols
 * ******************************************************************************************************************************/
void drawWeatherSymbolBig(int x, int y, int symbol)
{
  // fonts used:
  // u8g2_font_open_iconic_embedded_6x_t
  // u8g2_font_open_iconic_weather_6x_t
  // 1x = 8x8 px, 2x = 16x16 px ... 6x = 48x48 px,  8x = 64x64 px
  // encoding values, see: https://github.com/olikraus/u8g2/wiki/fntgrpiconic

  switch (symbol)
  {
    case SUN:
      u8display.setFont(u8g2_font_open_iconic_weather_6x_t);  // 16 px
      u8display.drawGlyph(x, y, 69);
      break;
    case SUN_CLOUD:
      u8display.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8display.drawGlyph(x, y, 65);
      break;
    case CLOUD:
      u8display.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8display.drawGlyph(x, y, 64);
      break;
    case RAIN:
      u8display.setFont(u8g2_font_open_iconic_weather_6x_t);
      u8display.drawGlyph(x, y, 67);
      break;
    case THUNDER:
      u8display.setFont(u8g2_font_open_iconic_embedded_6x_t);
      u8display.drawGlyph(x, y, 67);
      break;
    case MIST:
      u8display.setFont(u8g2_font_open_iconic_other_6x_t);
      u8display.drawGlyph(x, y, 67);
      break;
    case SNOW:
      u8display.setFont(u8g2_font_open_iconic_embedded_6x_t);
      u8display.drawGlyph(x, y, 66);
      break;
  }
}

//***********************************************************
// Button Functions
//***********************************************************

void changeLocation(Button2 &btn) {
  //Serial.println("pressed");
  Serial.println("btn1 pressed");
  buttonpressed = true;  //triggers case select for location change
  caseselector < 3 ? caseselector++ : caseselector = 0;  //if more than 4 locations needed, update this line
  EEPROM.write(20, caseselector);
  EEPROM.commit();
  Serial.printf("Written to EEPROM: %d ", caseselector);
  //Serial.println(caseselector);
}

void changeUnits(Button2 &btn) {
  units == "metric" ? units = "imperial" : units = "metric";
  refreshScreen = true;  // triggers immediate weather display refresh
  writeString(0, units);  // write units to EEPROM.
  // on initial use, remember to comment out any EEPROM reads until something has been loaded in.  Otherwise readString reads the entire EEPROM contents and will error.
  Serial.printf("Written to EEPROM: %s ", units);
}

void refresh(Button2 &btn) {
  refreshScreen = true;  // triggers immediate weather display refresh
  // Serial.println("Screen Refresh set to true.");
  fhtone(SPEAKER, 988.0, 100.0);   // tone1 - B5
  fhtone(SPEAKER, 1319.0, 200.0); // tone2 - E6
  //  rainbow dotstars
  //  rainbow(1);
  //  strip.clear();
  //  strip.show();
}


/***************************************************************************************
**               EEPROM string functions from circuits4you.com
***************************************************************************************/
void writeString(char add, String data)
{
  int _size = data.length();
  int i;
  for (i = 0; i < _size; i++)
  {
    EEPROM.write(add + i, data[i]);
  }
  EEPROM.write(add + _size, '\0'); //Add termination null character for String Data
  EEPROM.commit();
  //Serial.printf("EEPROM Written: %s", data);
}

String read_String(char add)
{
  //int i;
  char data[100]; //Max 100 Bytes
  int len = 0;
  unsigned char k;
  k = EEPROM.read(add);
  while (k != '\0' && len < 500) //Read until null character
  {
    k = EEPROM.read(add + len);
    data[len] = k;
    len++;
  }
  data[len] = '\0';
  return String(data);
}


/***************************************************************************************
**                          Convert unix time to a time string
***************************************************************************************/
//#define TIME_OFFSET 0UL * 3600UL // UTC + 0 hour:  GMT
//#define TIME_OFFSET -8UL * 3600UL // UTC -8 hour:  Pacific Time
//#define TIME_OFFSET -6UL * 3600UL // UTC -6 hour:  Central Time
//#define TIME_OFFSET 8UL * 3600UL // UTC -6 hour:  China Time
String strTime(time_t unixTime)
{
  //unixTime += TIME_OFFSET;
  return ctime(&unixTime);
}


/***************************************************************************************
**                          Change location for Weather
***************************************************************************************/
void setLocation(int cs) {
  switch (cs)
  {

    case 0:
      {

        //https://gpscoordinates.info/state/california/san-mateo/palo-alto/

        //        currlocation = "Palo Alto CA";
        //        latitude =  "37.467338"; // 90.0000 to -90.0000 negative for Southern hemisphere
        //        longitude = "-122.115158"; // 180.000 to -180.000 negative for West

        currlocation = "Hayward CA";
        latitude =  "37.67211"; // 90.0000 to -90.0000 negative for Southern hemisphere
        longitude = "-122.08396"; // 180.000 to -180.000 negative for West

        // Set timezone to Pacific Standard Time
        setenv("TZ", "PST8PDT,M3.2.0/2,M11.1.0", 1);
        refreshScreen = true;

        break;
      }

    case 1:
      {

        //https://gpscoordinates.info/state/texas/harris/spring/

        //  currlocation = "Houston TX";
        //  latitude =  "29.83399"; // 90.0000 to -90.0000 negative for Southern hemisphere
        //  longitude = "-95.434241"; // 180.000 to -180.000 negative for West

        //        currlocation = "Spring TX";
        //        latitude =  "30.08453"; // 90.0000 to -90.0000 negative for Southern hemisphere
        //        longitude = "-95.41929"; // 180.000 to -180.000 negative for West

        currlocation = "Norman OK";
        latitude =  "35.2226"; // 90.0000 to -90.0000 negative for Southern hemisphere
        longitude = "-97.4395"; // 180.000 to -180.000 negative for West

        // Set timezone to Central Standard Time
        setenv("TZ", "CST6CDT,M3.2.0/2,M11.1.0", 1);
        refreshScreen = true;

        break;
      }

    case 2:
      {

        // Bing London Geolocation
        currlocation = "London UK";
        latitude =  "51.500153"; // 90.0000 to -90.0000 negative for Southern hemisphere
        longitude = "-0.1262362"; // 180.000 to -180.000 negative for West

        // Set timezone to London
        setenv("TZ", "GMT0BST,M3.5.0/01,M10.5.0/02", 1);
        refreshScreen = true;

        break;
      }

    case 3:
      {

        // Bing Taipei Geolocation -> 25.038412° N, 121.563705° E
        currlocation = "Taipei TW";
        latitude =  "25.038412"; // 90.0000 to -90.0000 negative for Southern hemisphere
        longitude = "121.563705"; // 180.000 to -180.000 negative for West

        //set timezone to Taiwan
        setenv("TZ", "CST-8", 1);  // Taiwan timezone
        refreshScreen = true;

        break;
      }

    default:
      {
        Serial.print("case ");
        Serial.println(cs);
        break;
      }
  }
}


/***************************************************************************************
**                          Make sound
***************************************************************************************/
void fhtone(uint8_t pin, float frequency, float duration) {
  ledcSetup(1, frequency, 8);
  ledcAttachPin(pin, 1);
  ledcWrite(1, 128);
  delay(duration);
  ledcWrite(1, 0);
}


// Rainbow cycle along whole strip. Pass delay time (in ms) between frames.
void rainbow(int wait) {
  // Hue of first pixel runs 5 complete loops through the color wheel.
  // Color wheel has a range of 65536 but it's OK if we roll over, so
  // just count from 0 to 5*65536. Adding 256 to firstPixelHue each time
  // means we'll make 5*65536/256 = 1280 passes through this outer loop:
  for (long firstPixelHue = 0; firstPixelHue < 5 * 65536; firstPixelHue += 256) {
    for (int i = 0; i < strip.numPixels(); i++) { // For each pixel in strip...
      // Offset pixel hue by an amount to make one full revolution of the
      // color wheel (range of 65536) along the length of the strip
      // (strip.numPixels() steps):
      int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
      // strip.ColorHSV() can take 1 or 3 arguments: a hue (0 to 65535) or
      // optionally add saturation and value (brightness) (each 0 to 255).
      // Here we're using just the single-argument hue variant. The result
      // is passed through strip.gamma32() to provide 'truer' colors
      // before assigning to each pixel:
      strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
    }
    strip.show(); // Update strip with new contents
    delay(wait);  // Pause for a moment
  }
}
