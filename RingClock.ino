#define FASTLED_INTERRUPT_RETRY_COUNT 0 // Fix for ESP8266
#include <NTPClient.h>                  // NTP time client
#include <ESP8266WiFi.h>                // ESP8266 WiFi library
#include <WiFiManager.h>                // Smart WiFi Manager to handle networking
#include <FastLED.h>                    // FastLED library for displaying LEDs
#include <WiFiUdp.h>                    // UDP library to transport NTP data
#include <ArduinoJson.h>                // json library for parsing http results
#include <ESP8266HTTPClient.h>          // ESP8266 http library
#include <TimeLib.h>                    // Advanced time functions.

#define NUM_LEDS 24               // How many leds are in the strip?
#define DATA_PIN D1               // Data pin that led data will be written out over
#define LED_TYPE WS2812B          //LED chip type
#define BRIGHTNESS   8            // Set LED brightness - 0 - 255
#define COLOR_ORDER  GRB          // Define LED colour order
#define KEY "XXXXXXXXXX"          // timezonedb.com API Key
int ntpUpdate = 600;

byte hour_hand, minute_hand, second_hand, previous_second;
char urlData[180]; //array for holding complied URL
int  offset; // UTC offset in seconds

WiFiUDP ntpUDP;                   //initialise UDP NTP

// You can specify the time server pool and the offset (in seconds, can be
// changed later with setTimeOffset() ). Additionaly you can specify the
// update interval (in milliseconds, can be changed using setUpdateInterval() ).
// NTPClient ntpClient(ntpUDP, "time.google.com", 3600, 0);

NTPClient ntpClient(ntpUDP, "time.google.com", ntpUpdate, 0); // initialist NTP client with server name 
hourFormat12();               // Set to 12 hour AM.PM time 

HTTPClient http;// Initialise HTTP Client

CRGB leds[NUM_LEDS];              // This is an array of leds.  One item for each led in your strip.

void setup() {
  Serial.begin(115200);
  
  
  WiFiManager wifiManager; // wifi configuration wizard
  wifiManager.autoConnect("Clock"); // configuration for the access point
  Serial.println("WiFi Client connected!)");

  ntpClient.begin();  // Start NTP client
  
  FastLED.addLeds<LED_TYPE, DATA_PIN, COLOR_ORDER >(leds, NUM_LEDS);
  FastLED.setBrightness(BRIGHTNESS);
  
  getIPtz(); // get ip-api.com Timezone data for your location
  getOffset(); // get timezonedb.com UTC offset
  
  setSyncProvider(getNTPTime); // Specify function to sync time_t with NTP
  setSyncInterval(600);        // Set time_t sync interval - 10 minutes
  
  time_t getNTPTime();        
     if(timeStatus()==timeNotSet) // test for time_t set
     Serial.println("Unable to sync with NTP");
  else
     Serial.print("NTP has set the offset (local) system time to: "); Serial.print(hour()); Serial.print(":"); Serial.print(minute()); Serial.print(":"); Serial.println(second());;
     hour_hand = hour();
     minute_hand = minute();
     second_hand = second();
  
}

void loop() {

  drawHands();

}

void getIPtz() { // pull timezone data from ip-api.com and create URL string to query http://api.timezonedb.com

  http.begin("http://ip-api.com/json/?fields=timezone");       //get gelocation timezone data from ip-api
  int httpCode = http.GET();                                  //Send the request
  String ipapi = http.getString();                            // Return to the string
  http.end();   //Close connection                            // Close http client
  DynamicJsonBuffer jsonBuffer;                               //Set up dynamic json buffer
  JsonObject& root = jsonBuffer.parseObject(ipapi);           // Parse ip-api data
  if (!root.success()) {
    Serial.println(F("Parsing failed!"));
    return;
  }

  char* tz = strdup(root["timezone"]); // copy timezone data from json parse

  urlData[0] = (char)0;// clear array
  // build query url for timezonedb.com
  strcpy (urlData, "http://api.timezonedb.com/v2/get-time-zone?key=");
  strcat (urlData, KEY); // api key
  strcat (urlData, "&format=json&by=zone&zone=");
  strcat (urlData, tz); // timezone data from ip-api.com
}

void clearHands() {
  for (byte i = 0; i <= NUM_LEDS; i++) {
    leds[i] = CRGB::Black;
  }
}

void getOffset() {
 
  http.begin(urlData);
  int httpCode = http.GET();                              //Send the request
  String timezonedb = http.getString();
  http.end();   //Close connection
  DynamicJsonBuffer jsonBuffer;   // allocate json buffer

  JsonObject& root = jsonBuffer.parseObject(timezonedb); // parse return from timzonedb.com

  offset = (root["gmtOffset"]); // get offset from UTC/GMT 
}

time_t getNTPTime(){  // Return current UTC time to time_t
  ntpClient.update();
  return ntpClient.getEpochTime() + offset; 
}

void drawHands() {

  clearHands();     // clear LEDs

  leds[int(hour_hand * (NUM_LEDS/12))] = CRGB::White; leds[(int(hour_hand *(NUM_LEDS / 12)+1)) ] = CRGB::White;//Serial.print(int(hour_hand *(NUM_LEDS / 12)));


  if (hour_hand == minute_hand) { // if hour and minute are the same led, use a different color
    leds[int(minute_hand*(60/NUM_LEDS)/5)] = CRGB::Purple;
  } else {
    leds[int(minute_hand*(60/NUM_LEDS)/5)] = CRGB::Red;
  }
  // draw the second_hand LED - 60 seconds divided by the number of LEDs, divided by five
  leds[int(second_hand*(60/NUM_LEDS))] += CRGB(0,0,255);
  leds[int(second_hand*(60/NUM_LEDS))] %= 64; // reduce second hand output to 64/256'ths - 1/4 or 25%
 
  FastLED.show();
}
