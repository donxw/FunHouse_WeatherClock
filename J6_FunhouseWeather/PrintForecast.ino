/***************************************************************************************
**                          Convert id to weather symbol
***************************************************************************************/
/*  Defined in Functions Tab
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
*/

void SerialPrintForecast() {
  // Print Current
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println("Current Weather");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println(location.loc);
  Serial.println(location.units);
  Serial.print(strTime(location.curr_dt));
  switch (Symbol) {
    case SUN:
      Serial.println("Sunny");
      break;
    case SUN_CLOUD:
      Serial.println("Partly Sunny");
      break;
    case CLOUD:
      Serial.println("Cloudy");
      break;
    case RAIN:
      Serial.println("Rainy");
      break;
    case THUNDER:
      Serial.println("Thunder");
      break;
    case MIST:
      Serial.println("Mist");
      break;
    case SNOW:
      Serial.println("Snow");
      break;
    default:
      // statements
      break;
  }
  Serial.println(location.curr_desc);
  Serial.printf("Current Temp:  %*.2f°\n", 4, location.curr_temp);
  Serial.printf("Current Humidity: %*.0f%%\n", 2, location.curr_humid);

  // Print Forecast
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  Serial.println("Forecast Weather");
  Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  for (int i = 0; i <= 7; i++) {
    Serial.print(strTime(location.dt[i]));
    Serial.println("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
    Serial.printf("T min: %*.2f°\n", 4, location.temp_min[i] ); //Alt+0176 for degree symbol
    Serial.printf("T max: %*.2f°\n", 4, location.temp_max[i] );
    Serial.printf("Humidity: %*.0f%%\n", 2, location.humid[i] );
    Symbol = getSymbol(location.id[i]);
    //Serial.print(Symbol);
    switch (Symbol) {
      case SUN:
        Serial.println("Sunny");
        break;
      case SUN_CLOUD:
        Serial.println("Partly Sunny");
        break;
      case CLOUD:
        Serial.println("Cloudy");
        break;
      case RAIN:
        Serial.println("Rainy");
        break;
      case THUNDER:
        Serial.println("Thunder");
        break;
      case MIST:
        Serial.println("Mist");
        break;
      case SNOW:
        Serial.println("Snow");
        break;
      default:
        // statements
        break;
    }
    Serial.println(" ");
  }
}
