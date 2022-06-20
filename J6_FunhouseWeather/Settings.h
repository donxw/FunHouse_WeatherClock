// Max number of days to for weather forecast display
int MAX_DAYS = 5;

// this is the default for the first weather location on system reboot.  It matches setLocation switch case 0.
String currlocation = "Hayward CA";
String latitude =  "37.67211"; // 90.0000 to -90.0000 negative for Southern hemisphere
String longitude = "-122.08396"; // 180.000 to -180.000 negative for West
String units = "metric";

struct weather {
  int curr_id, id[8];
  float curr_temp, temp_min[8], temp_max[8];
  float curr_humid, humid[8];
  long curr_dt, dt[8];
  String lat;
  String lon;
  String units;
  String loc;
  int err;
  char curr_desc[80];
  char curr_weat[60];
};

//Get your key at https://home.openweathermap.org/users/sign_up
String APIKEY = "xxxxxx";  //enter your API Key

// weather variables
#define SUN  0
#define SUN_CLOUD  1
#define CLOUD 2
#define RAIN 3
#define THUNDER 4
#define MIST 5
#define SNOW 6
int8_t Symbol;

// barometer variables
#define NO_VALUE  0
#define RISING_P  1
#define FALLING_P 2
#define FAST_FALLING_P 3
#define STEADY 4
int8_t pTrend;

// Maximum of 3 servers
#define NTP_SERVERS "us.pool.ntp.org", "pool.ntp.org", "time.nist.gov"

// Change to suit your WiFi router
#define WIFI_SSID     "xxxx"
#define WIFI_PASSWORD "xxxx"

// Variables for the milli loops
unsigned long previousWeatherTime = 0;
unsigned long weatherInterval = 15 * 60 * 1000; // 15m

unsigned long previousClockTime = 0;
unsigned long clockInterval = 1 * 1000; // 1 sec
//unsigned long clockInterval = 30  * 1000; // 30 sec

unsigned long previousSensorTime = 0;
unsigned long sensorInterval = .5 * 60 * 1000; // .5 m

unsigned long omm20 = 0;
unsigned long omm20Interval = 20 * 60 * 1000; // 20 min

float prevpressure = 0;
float deltapressure = 0;
String str_pressTrend = "No Data";
String str_prev_pressTrend = "No Data";

uint16_t y_loc;  // Global screen Y location
uint8_t dd, hh, mm, ss;
uint8_t dd_p, hh_p, mm_p, ss_p, hh12_p;
float temp_p, humid_p, press_p = 1;
byte odd = 99, omm = 99, oss = 99;
byte xcolon = 0, xsecs = 0;
