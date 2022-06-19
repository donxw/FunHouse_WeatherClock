
weather getForecast( weather w, int httpPort, String APIKEY)
{
  //*************************************  Get Weather ***********************************************************
  // Connect to HTTP server
  //char *servername = "api.openweathermap.org"; // remote server we will connect to

  if (!client.connect( "api.openweathermap.org", httpPort))
  {
    w.err = 1;
    return (w);
  }

  String latitude = w.lat;
  String longitude = w.lon;
  String units = w.units;

  Serial.println(F("Connected!"));

  //forecast URL for current and 7 days - but only works with long and lat values:  https://api.openweathermap.org/data/2.5/onecall?lat=33.441792&lon=-94.037689&exclude=minutely,hourly&appid=122c8b5cd4731038ff78486f1faa70c5

  String url = "/data/2.5/onecall?lat=" + latitude + "&lon=" + longitude + "&exclude=minutely,hourly&units=" + units + "&appid=" + APIKEY;

  //https://api.openweathermap.org/data/2.5/onecall?lat=51.5074&lon=0.1278&exclude=minutely,hourly&units=imperial&appid=122c8b5cd4731038ff78486f1faa70c5

  // Send HTTP request
  Serial.println("Requesting URL: " + url);
  // This will send the request to the server
  client.print(String("GET ") + url + " HTTP/1.1\r\n");
  client.print("Host: api.openweathermap.org\r\n");
  client.print("Connection: close\r\n\r\n");

  delay(10);

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    w.err = 2;
    return (w);
  }

  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  // It should be "HTTP/1.0 200 OK" or "HTTP/1.1 200 OK"
  if (strcmp(status + 9, "200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    w.err = 3;
    return (w);
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    w.err = 4;
    return (w);
  }


  //Parse and assign to global variables

  const size_t capacity = 9 * JSON_ARRAY_SIZE(1) + JSON_ARRAY_SIZE(8) + 17 * JSON_OBJECT_SIZE(4) + 9 * JSON_OBJECT_SIZE(6) + 8 * JSON_OBJECT_SIZE(14) + JSON_OBJECT_SIZE(15) + 520;
  DynamicJsonDocument doc(capacity);

  //const char* json = "{\"lat\":51.51,\"lon\":0.13,\"timezone\":\"Europe/London\",\"timezone_offset\":0,\"current\":{\"dt\":1606165869,\"sunrise\":1606116692,\"sunset\":1606147248,\"temp\":48.22,\"feels_like\":44.65,\"pressure\":1024,\"humidity\":81,\"dew_point\":42.67,\"uvi\":0.62,\"clouds\":75,\"visibility\":10000,\"wind_speed\":3.36,\"wind_deg\":0,\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04n\"}]},\"daily\":[{\"dt\":1606129200,\"sunrise\":1606116692,\"sunset\":1606147248,\"temp\":{\"day\":49.86,\"min\":44.04,\"max\":49.91,\"night\":48.22,\"eve\":47.44,\"morn\":44.04},\"feels_like\":{\"day\":43.65,\"night\":42.55,\"eve\":41.68,\"morn\":39.76},\"pressure\":1028,\"humidity\":61,\"dew_point\":37.06,\"wind_speed\":6.08,\"wind_deg\":214,\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03d\"}],\"clouds\":43,\"pop\":0,\"uvi\":0.62},{\"dt\":1606215600,\"sunrise\":1606203187,\"sunset\":1606233588,\"temp\":{\"day\":52.86,\"min\":48.16,\"max\":52.86,\"night\":50.22,\"eve\":50.52,\"morn\":49.37},\"feels_like\":{\"day\":45.12,\"night\":43.74,\"eve\":43.07,\"morn\":43.18},\"pressure\":1017,\"humidity\":77,\"dew_point\":45.99,\"wind_speed\":12.03,\"wind_deg\":201,\"weather\":[{\"id\":803,\"main\":\"Clouds\",\"description\":\"broken clouds\",\"icon\":\"04d\"}],\"clouds\":77,\"pop\":0,\"uvi\":0.62},{\"dt\":1606302000,\"sunrise\":1606289681,\"sunset\":1606319932,\"temp\":{\"day\":53.53,\"min\":48.2,\"max\":53.53,\"night\":50.02,\"eve\":51.78,\"morn\":48.2},\"feels_like\":{\"day\":48.63,\"night\":47.25,\"eve\":48.7,\"morn\":44.26},\"pressure\":1012,\"humidity\":84,\"dew_point\":49.01,\"wind_speed\":8.28,\"wind_deg\":201,\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":87,\"pop\":0.02,\"uvi\":0.59},{\"dt\":1606388400,\"sunrise\":1606376173,\"sunset\":1606406277,\"temp\":{\"day\":52.02,\"min\":49.3,\"max\":52.02,\"night\":49.3,\"eve\":49.82,\"morn\":51.17},\"feels_like\":{\"day\":49.1,\"night\":44.24,\"eve\":45.34,\"morn\":49.24},\"pressure\":1021,\"humidity\":81,\"dew_point\":46.54,\"wind_speed\":3.67,\"wind_deg\":46,\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":100,\"pop\":0,\"uvi\":0.55},{\"dt\":1606474800,\"sunrise\":1606462664,\"sunset\":1606492626,\"temp\":{\"day\":48.94,\"min\":43.02,\"max\":48.94,\"night\":43.02,\"eve\":45.39,\"morn\":46.11},\"feels_like\":{\"day\":42.06,\"night\":35.65,\"eve\":38.07,\"morn\":40.05},\"pressure\":1023,\"humidity\":64,\"dew_point\":37.4,\"wind_speed\":7.38,\"wind_deg\":53,\"weather\":[{\"id\":802,\"main\":\"Clouds\",\"description\":\"scattered clouds\",\"icon\":\"03d\"}],\"clouds\":42,\"pop\":0.02,\"uvi\":0.58},{\"dt\":1606561200,\"sunrise\":1606549154,\"sunset\":1606578978,\"temp\":{\"day\":43.41,\"min\":40.69,\"max\":44.17,\"night\":42.44,\"eve\":42.6,\"morn\":41.99},\"feels_like\":{\"day\":36.72,\"night\":36.09,\"eve\":35.94,\"morn\":34.92},\"pressure\":1020,\"humidity\":73,\"dew_point\":35.56,\"wind_speed\":6.46,\"wind_deg\":48,\"weather\":[{\"id\":500,\"main\":\"Rain\",\"description\":\"light rain\",\"icon\":\"10d\"}],\"clouds\":76,\"pop\":0.22,\"rain\":0.55,\"uvi\":0.53},{\"dt\":1606647600,\"sunrise\":1606635641,\"sunset\":1606665332,\"temp\":{\"day\":42.64,\"min\":38.35,\"max\":43.03,\"night\":40.08,\"eve\":40.8,\"morn\":38.35},\"feels_like\":{\"day\":36.01,\"night\":34.5,\"eve\":35.35,\"morn\":31.62},\"pressure\":1020,\"humidity\":72,\"dew_point\":34.3,\"wind_speed\":6.02,\"wind_deg\":304,\"weather\":[{\"id\":800,\"main\":\"Clear\",\"description\":\"clear sky\",\"icon\":\"01d\"}],\"clouds\":2,\"pop\":0.06,\"uvi\":0.53},{\"dt\":1606734000,\"sunrise\":1606722127,\"sunset\":1606751690,\"temp\":{\"day\":43.77,\"min\":39.16,\"max\":44.1,\"night\":41.2,\"eve\":40.64,\"morn\":39.16},\"feels_like\":{\"day\":36.93,\"night\":32.18,\"eve\":32.07,\"morn\":33.28},\"pressure\":1023,\"humidity\":78,\"dew_point\":37.38,\"wind_speed\":7.34,\"wind_deg\":7,\"weather\":[{\"id\":804,\"main\":\"Clouds\",\"description\":\"overcast clouds\",\"icon\":\"04d\"}],\"clouds\":100,\"pop\":0.22,\"uvi\":0.47}]}";

  deserializeJson(doc, client);  // remember to change doc to client

  float lat = doc["lat"]; // 51.51
  float lon = doc["lon"]; // 0.13
  const char* timezone = doc["timezone"]; // "Europe/London"
  int timezone_offset = doc["timezone_offset"]; // 0

  JsonObject current = doc["current"];
  long current_dt = current["dt"]; // 1606165869
  //Serial.println(strTime(current_dt));
  long current_sunrise = current["sunrise"]; // 1606116692
  long current_sunset = current["sunset"]; // 1606147248
  float current_temp = current["temp"]; // 48.22
  //Serial.printf("Current Temp:  %*.1f° \n", 3, current_temp);
  float current_feels_like = current["feels_like"]; // 44.65
  int current_pressure = current["pressure"]; // 1024
  int current_humidity = current["humidity"]; // 81
  float current_dew_point = current["dew_point"]; // 42.67
  float current_uvi = current["uvi"]; // 0.62
  int current_clouds = current["clouds"]; // 75
  int current_visibility = current["visibility"]; // 10000
  float current_wind_speed = current["wind_speed"]; // 3.36
  int current_wind_deg = current["wind_deg"]; // 0

  JsonObject current_weather_0 = current["weather"][0];
  int current_weather_0_id = current_weather_0["id"]; // 803
  //Symbol = getSymbol(current_weather_0_id);
  //Serial.printf("Symbol:  %d\n", Symbol);
  const char* current_weather_0_main = current_weather_0["main"]; // "Clouds"
  const char* current_weather_0_description = current_weather_0["description"]; // "broken clouds"
  const char* current_weather_0_icon = current_weather_0["icon"]; // "04n"

  w.curr_dt = current_dt;
  w.curr_temp = current_temp;
  w.curr_humid = current_humidity;
  w.curr_id = current_weather_0_id;
  strncpy(w.curr_weat, current_weather_0_main, 60);
  strncpy(w.curr_desc, current_weather_0_description, 80);

  JsonArray daily = doc["daily"];

  JsonObject daily_0 = daily[0];
  long daily_0_dt = daily_0["dt"]; // 1606129200
  long daily_0_sunrise = daily_0["sunrise"]; // 1606116692
  long daily_0_sunset = daily_0["sunset"]; // 1606147248

  JsonObject daily_0_temp = daily_0["temp"];
  float daily_0_temp_day = daily_0_temp["day"]; // 49.86
  float daily_0_temp_min = daily_0_temp["min"]; // 44.04
  float daily_0_temp_max = daily_0_temp["max"]; // 49.91
  float daily_0_temp_night = daily_0_temp["night"]; // 48.22
  float daily_0_temp_eve = daily_0_temp["eve"]; // 47.44
  float daily_0_temp_morn = daily_0_temp["morn"]; // 44.04

  JsonObject daily_0_feels_like = daily_0["feels_like"];
  float daily_0_feels_like_day = daily_0_feels_like["day"]; // 43.65
  float daily_0_feels_like_night = daily_0_feels_like["night"]; // 42.55
  float daily_0_feels_like_eve = daily_0_feels_like["eve"]; // 41.68
  float daily_0_feels_like_morn = daily_0_feels_like["morn"]; // 39.76

  int daily_0_pressure = daily_0["pressure"]; // 1028
  int daily_0_humidity = daily_0["humidity"]; // 61
  float daily_0_dew_point = daily_0["dew_point"]; // 37.06
  float daily_0_wind_speed = daily_0["wind_speed"]; // 6.08
  int daily_0_wind_deg = daily_0["wind_deg"]; // 214

  JsonObject daily_0_weather_0 = daily_0["weather"][0];
  int daily_0_weather_0_id = daily_0_weather_0["id"]; // 802
  const char* daily_0_weather_0_main = daily_0_weather_0["main"]; // "Clouds"
  const char* daily_0_weather_0_description = daily_0_weather_0["description"]; // "scattered clouds"
  const char* daily_0_weather_0_icon = daily_0_weather_0["icon"]; // "03d"

  int daily_0_clouds = daily_0["clouds"]; // 43
  int daily_0_pop = daily_0["pop"]; // 0
  float daily_0_uvi = daily_0["uvi"]; // 0.62

  w.dt[0] = daily_0_dt;
  w.temp_min[0] = daily_0_temp_min;
  w.temp_max[0] = daily_0_temp_max;
  w.humid[0] = daily_0_humidity;
  w.id[0] = daily_0_weather_0_id;

  JsonObject daily_1 = daily[1];
  long daily_1_dt = daily_1["dt"]; // 1606215600
  long daily_1_sunrise = daily_1["sunrise"]; // 1606203187
  long daily_1_sunset = daily_1["sunset"]; // 1606233588

  JsonObject daily_1_temp = daily_1["temp"];
  float daily_1_temp_day = daily_1_temp["day"]; // 52.86
  float daily_1_temp_min = daily_1_temp["min"]; // 48.16
  float daily_1_temp_max = daily_1_temp["max"]; // 52.86
  float daily_1_temp_night = daily_1_temp["night"]; // 50.22
  float daily_1_temp_eve = daily_1_temp["eve"]; // 50.52
  float daily_1_temp_morn = daily_1_temp["morn"]; // 49.37

  JsonObject daily_1_feels_like = daily_1["feels_like"];
  float daily_1_feels_like_day = daily_1_feels_like["day"]; // 45.12
  float daily_1_feels_like_night = daily_1_feels_like["night"]; // 43.74
  float daily_1_feels_like_eve = daily_1_feels_like["eve"]; // 43.07
  float daily_1_feels_like_morn = daily_1_feels_like["morn"]; // 43.18

  int daily_1_pressure = daily_1["pressure"]; // 1017
  int daily_1_humidity = daily_1["humidity"]; // 77
  float daily_1_dew_point = daily_1["dew_point"]; // 45.99
  float daily_1_wind_speed = daily_1["wind_speed"]; // 12.03
  int daily_1_wind_deg = daily_1["wind_deg"]; // 201

  JsonObject daily_1_weather_0 = daily_1["weather"][0];
  int daily_1_weather_0_id = daily_1_weather_0["id"]; // 803
  const char* daily_1_weather_0_main = daily_1_weather_0["main"]; // "Clouds"
  const char* daily_1_weather_0_description = daily_1_weather_0["description"]; // "broken clouds"
  const char* daily_1_weather_0_icon = daily_1_weather_0["icon"]; // "04d"

  int daily_1_clouds = daily_1["clouds"]; // 77
  int daily_1_pop = daily_1["pop"]; // 0
  float daily_1_uvi = daily_1["uvi"]; // 0.62

  w.dt[1] = daily_1_dt;
  w.temp_min[1] = daily_1_temp_min;
  w.temp_max[1] = daily_1_temp_max;
  w.humid[1] = daily_1_humidity;
  w.id[1] = daily_1_weather_0_id;

  JsonObject daily_2 = daily[2];
  long daily_2_dt = daily_2["dt"]; // 1606302000
  w.dt[2] = daily_2_dt;
  long daily_2_sunrise = daily_2["sunrise"]; // 1606289681
  long daily_2_sunset = daily_2["sunset"]; // 1606319932

  JsonObject daily_2_temp = daily_2["temp"];
  float daily_2_temp_day = daily_2_temp["day"]; // 53.53
  float daily_2_temp_min = daily_2_temp["min"]; // 48.2
  w.temp_min[2] = daily_2_temp_min;
  float daily_2_temp_max = daily_2_temp["max"]; // 53.53
  float daily_2_temp_night = daily_2_temp["night"]; // 50.02
  float daily_2_temp_eve = daily_2_temp["eve"]; // 51.78
  float daily_2_temp_morn = daily_2_temp["morn"]; // 48.2

  JsonObject daily_2_feels_like = daily_2["feels_like"];
  float daily_2_feels_like_day = daily_2_feels_like["day"]; // 48.63
  float daily_2_feels_like_night = daily_2_feels_like["night"]; // 47.25
  float daily_2_feels_like_eve = daily_2_feels_like["eve"]; // 48.7
  float daily_2_feels_like_morn = daily_2_feels_like["morn"]; // 44.26

  int daily_2_pressure = daily_2["pressure"]; // 1012
  int daily_2_humidity = daily_2["humidity"]; // 84
  float daily_2_dew_point = daily_2["dew_point"]; // 49.01
  float daily_2_wind_speed = daily_2["wind_speed"]; // 8.28
  int daily_2_wind_deg = daily_2["wind_deg"]; // 201

  JsonObject daily_2_weather_0 = daily_2["weather"][0];
  int daily_2_weather_0_id = daily_2_weather_0["id"]; // 804
  const char* daily_2_weather_0_main = daily_2_weather_0["main"]; // "Clouds"
  const char* daily_2_weather_0_description = daily_2_weather_0["description"]; // "overcast clouds"
  const char* daily_2_weather_0_icon = daily_2_weather_0["icon"]; // "04d"

  int daily_2_clouds = daily_2["clouds"]; // 87
  float daily_2_pop = daily_2["pop"]; // 0.02
  float daily_2_uvi = daily_2["uvi"]; // 0.59

  w.dt[2] = daily_2_dt;
  w.temp_min[2] = daily_2_temp_min;
  w.temp_max[2] = daily_2_temp_max;
  w.humid[2] = daily_2_humidity;
  w.id[2] = daily_2_weather_0_id;

  JsonObject daily_3 = daily[3];
  long daily_3_dt = daily_3["dt"]; // 1606388400
  w.dt[3] = daily_3_dt;
  long daily_3_sunrise = daily_3["sunrise"]; // 1606376173
  long daily_3_sunset = daily_3["sunset"]; // 1606406277

  JsonObject daily_3_temp = daily_3["temp"];
  float daily_3_temp_day = daily_3_temp["day"]; // 52.02
  float daily_3_temp_min = daily_3_temp["min"]; // 49.3
  float daily_3_temp_max = daily_3_temp["max"]; // 52.02
  float daily_3_temp_night = daily_3_temp["night"]; // 49.3
  float daily_3_temp_eve = daily_3_temp["eve"]; // 49.82
  float daily_3_temp_morn = daily_3_temp["morn"]; // 51.17

  JsonObject daily_3_feels_like = daily_3["feels_like"];
  float daily_3_feels_like_day = daily_3_feels_like["day"]; // 49.1
  float daily_3_feels_like_night = daily_3_feels_like["night"]; // 44.24
  float daily_3_feels_like_eve = daily_3_feels_like["eve"]; // 45.34
  float daily_3_feels_like_morn = daily_3_feels_like["morn"]; // 49.24

  int daily_3_pressure = daily_3["pressure"]; // 1021
  int daily_3_humidity = daily_3["humidity"]; // 81
  float daily_3_dew_point = daily_3["dew_point"]; // 46.54
  float daily_3_wind_speed = daily_3["wind_speed"]; // 3.67
  int daily_3_wind_deg = daily_3["wind_deg"]; // 46

  JsonObject daily_3_weather_0 = daily_3["weather"][0];
  int daily_3_weather_0_id = daily_3_weather_0["id"]; // 804
  const char* daily_3_weather_0_main = daily_3_weather_0["main"]; // "Clouds"
  const char* daily_3_weather_0_description = daily_3_weather_0["description"]; // "overcast clouds"
  const char* daily_3_weather_0_icon = daily_3_weather_0["icon"]; // "04d"

  int daily_3_clouds = daily_3["clouds"]; // 100
  int daily_3_pop = daily_3["pop"]; // 0
  float daily_3_uvi = daily_3["uvi"]; // 0.55

  w.dt[3] = daily_3_dt;
  w.temp_min[3] = daily_3_temp_min;
  w.temp_max[3] = daily_3_temp_max;
  w.humid[3] = daily_3_humidity;
  w.id[3] = daily_3_weather_0_id;

  JsonObject daily_4 = daily[4];
  long daily_4_dt = daily_4["dt"]; // 1606474800
  w.dt[4] = daily_4_dt;
  long daily_4_sunrise = daily_4["sunrise"]; // 1606462664
  long daily_4_sunset = daily_4["sunset"]; // 1606492626

  JsonObject daily_4_temp = daily_4["temp"];
  float daily_4_temp_day = daily_4_temp["day"]; // 48.94
  float daily_4_temp_min = daily_4_temp["min"]; // 43.02
  float daily_4_temp_max = daily_4_temp["max"]; // 48.94
  float daily_4_temp_night = daily_4_temp["night"]; // 43.02
  float daily_4_temp_eve = daily_4_temp["eve"]; // 45.39
  float daily_4_temp_morn = daily_4_temp["morn"]; // 46.11

  JsonObject daily_4_feels_like = daily_4["feels_like"];
  float daily_4_feels_like_day = daily_4_feels_like["day"]; // 42.06
  float daily_4_feels_like_night = daily_4_feels_like["night"]; // 35.65
  float daily_4_feels_like_eve = daily_4_feels_like["eve"]; // 38.07
  float daily_4_feels_like_morn = daily_4_feels_like["morn"]; // 40.05

  int daily_4_pressure = daily_4["pressure"]; // 1023
  int daily_4_humidity = daily_4["humidity"]; // 64
  float daily_4_dew_point = daily_4["dew_point"]; // 37.4
  float daily_4_wind_speed = daily_4["wind_speed"]; // 7.38
  int daily_4_wind_deg = daily_4["wind_deg"]; // 53

  JsonObject daily_4_weather_0 = daily_4["weather"][0];
  int daily_4_weather_0_id = daily_4_weather_0["id"]; // 802
  const char* daily_4_weather_0_main = daily_4_weather_0["main"]; // "Clouds"
  const char* daily_4_weather_0_description = daily_4_weather_0["description"]; // "scattered clouds"
  const char* daily_4_weather_0_icon = daily_4_weather_0["icon"]; // "03d"

  int daily_4_clouds = daily_4["clouds"]; // 42
  float daily_4_pop = daily_4["pop"]; // 0.02
  float daily_4_uvi = daily_4["uvi"]; // 0.58

  w.dt[4] = daily_4_dt;
  w.temp_min[4] = daily_4_temp_min;
  w.temp_max[4] = daily_4_temp_max;
  w.humid[4] = daily_4_humidity;
  w.id[4] = daily_4_weather_0_id;

  JsonObject daily_5 = daily[5];
  long daily_5_dt = daily_5["dt"]; // 1606561200
  w.dt[5] = daily_5_dt;
  long daily_5_sunrise = daily_5["sunrise"]; // 1606549154
  long daily_5_sunset = daily_5["sunset"]; // 1606578978

  JsonObject daily_5_temp = daily_5["temp"];
  float daily_5_temp_day = daily_5_temp["day"]; // 43.41
  float daily_5_temp_min = daily_5_temp["min"]; // 40.69
  float daily_5_temp_max = daily_5_temp["max"]; // 44.17
  float daily_5_temp_night = daily_5_temp["night"]; // 42.44
  float daily_5_temp_eve = daily_5_temp["eve"]; // 42.6
  float daily_5_temp_morn = daily_5_temp["morn"]; // 41.99

  JsonObject daily_5_feels_like = daily_5["feels_like"];
  float daily_5_feels_like_day = daily_5_feels_like["day"]; // 36.72
  float daily_5_feels_like_night = daily_5_feels_like["night"]; // 36.09
  float daily_5_feels_like_eve = daily_5_feels_like["eve"]; // 35.94
  float daily_5_feels_like_morn = daily_5_feels_like["morn"]; // 34.92

  int daily_5_pressure = daily_5["pressure"]; // 1020
  int daily_5_humidity = daily_5["humidity"]; // 73
  float daily_5_dew_point = daily_5["dew_point"]; // 35.56
  float daily_5_wind_speed = daily_5["wind_speed"]; // 6.46
  int daily_5_wind_deg = daily_5["wind_deg"]; // 48

  JsonObject daily_5_weather_0 = daily_5["weather"][0];
  int daily_5_weather_0_id = daily_5_weather_0["id"]; // 500
  const char* daily_5_weather_0_main = daily_5_weather_0["main"]; // "Rain"
  const char* daily_5_weather_0_description = daily_5_weather_0["description"]; // "light rain"
  const char* daily_5_weather_0_icon = daily_5_weather_0["icon"]; // "10d"

  int daily_5_clouds = daily_5["clouds"]; // 76
  float daily_5_pop = daily_5["pop"]; // 0.22
  float daily_5_rain = daily_5["rain"]; // 0.55
  float daily_5_uvi = daily_5["uvi"]; // 0.53

  w.dt[5] = daily_5_dt;
  w.temp_min[5] = daily_5_temp_min;
  w.temp_max[5] = daily_5_temp_max;
  w.humid[5] = daily_5_humidity;
  w.id[5] = daily_5_weather_0_id;

  JsonObject daily_6 = daily[6];
  long daily_6_dt = daily_6["dt"]; // 1606647600
  w.dt[6] = daily_6_dt;
  long daily_6_sunrise = daily_6["sunrise"]; // 1606635641
  long daily_6_sunset = daily_6["sunset"]; // 1606665332

  JsonObject daily_6_temp = daily_6["temp"];
  float daily_6_temp_day = daily_6_temp["day"]; // 42.64
  float daily_6_temp_min = daily_6_temp["min"]; // 38.35
  float daily_6_temp_max = daily_6_temp["max"]; // 43.03
  float daily_6_temp_night = daily_6_temp["night"]; // 40.08
  float daily_6_temp_eve = daily_6_temp["eve"]; // 40.8
  float daily_6_temp_morn = daily_6_temp["morn"]; // 38.35

  JsonObject daily_6_feels_like = daily_6["feels_like"];
  float daily_6_feels_like_day = daily_6_feels_like["day"]; // 36.01
  float daily_6_feels_like_night = daily_6_feels_like["night"]; // 34.5
  float daily_6_feels_like_eve = daily_6_feels_like["eve"]; // 35.35
  float daily_6_feels_like_morn = daily_6_feels_like["morn"]; // 31.62

  int daily_6_pressure = daily_6["pressure"]; // 1020
  int daily_6_humidity = daily_6["humidity"]; // 72
  float daily_6_dew_point = daily_6["dew_point"]; // 34.3
  float daily_6_wind_speed = daily_6["wind_speed"]; // 6.02
  int daily_6_wind_deg = daily_6["wind_deg"]; // 304

  JsonObject daily_6_weather_0 = daily_6["weather"][0];
  int daily_6_weather_0_id = daily_6_weather_0["id"]; // 800
  const char* daily_6_weather_0_main = daily_6_weather_0["main"]; // "Clear"
  const char* daily_6_weather_0_description = daily_6_weather_0["description"]; // "clear sky"
  const char* daily_6_weather_0_icon = daily_6_weather_0["icon"]; // "01d"

  int daily_6_clouds = daily_6["clouds"]; // 2
  float daily_6_pop = daily_6["pop"]; // 0.06
  float daily_6_uvi = daily_6["uvi"]; // 0.53

  w.dt[6] = daily_6_dt;
  w.temp_min[6] = daily_6_temp_min;
  w.temp_max[6] = daily_6_temp_max;
  w.humid[6] = daily_6_humidity;
  w.id[6] = daily_6_weather_0_id;

  JsonObject daily_7 = daily[7];
  long daily_7_dt = daily_7["dt"]; // 1606734000
  w.dt[7] = daily_7_dt;
  long daily_7_sunrise = daily_7["sunrise"]; // 1606722127
  long daily_7_sunset = daily_7["sunset"]; // 1606751690

  JsonObject daily_7_temp = daily_7["temp"];
  float daily_7_temp_day = daily_7_temp["day"]; // 43.77
  float daily_7_temp_min = daily_7_temp["min"]; // 39.16
  float daily_7_temp_max = daily_7_temp["max"]; // 44.1
  float daily_7_temp_night = daily_7_temp["night"]; // 41.2
  float daily_7_temp_eve = daily_7_temp["eve"]; // 40.64
  float daily_7_temp_morn = daily_7_temp["morn"]; // 39.16

  JsonObject daily_7_feels_like = daily_7["feels_like"];
  float daily_7_feels_like_day = daily_7_feels_like["day"]; // 36.93
  float daily_7_feels_like_night = daily_7_feels_like["night"]; // 32.18
  float daily_7_feels_like_eve = daily_7_feels_like["eve"]; // 32.07
  float daily_7_feels_like_morn = daily_7_feels_like["morn"]; // 33.28

  int daily_7_pressure = daily_7["pressure"]; // 1023
  int daily_7_humidity = daily_7["humidity"]; // 78
  float daily_7_dew_point = daily_7["dew_point"]; // 37.38
  float daily_7_wind_speed = daily_7["wind_speed"]; // 7.34
  int daily_7_wind_deg = daily_7["wind_deg"]; // 7

  JsonObject daily_7_weather_0 = daily_7["weather"][0];
  int daily_7_weather_0_id = daily_7_weather_0["id"]; // 804
  const char* daily_7_weather_0_main = daily_7_weather_0["main"]; // "Clouds"
  const char* daily_7_weather_0_description = daily_7_weather_0["description"]; // "overcast clouds"
  const char* daily_7_weather_0_icon = daily_7_weather_0["icon"]; // "04d"

  int daily_7_clouds = daily_7["clouds"]; // 100
  float daily_7_pop = daily_7["pop"]; // 0.22
  float daily_7_uvi = daily_7["uvi"]; // 0.47

  w.dt[7] = daily_7_dt;
  w.temp_min[7] = daily_7_temp_min;
  w.temp_max[7] = daily_7_temp_max;
  w.humid[7] = daily_7_humidity;
  w.id[7] = daily_7_weather_0_id;

  return (w);
}
