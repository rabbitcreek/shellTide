/*
 * 
 * This Program is designed to operate the The Gecko Tide Clock.  It's construction is found on 
 * my Instructables web site.  It uses Arduino JSON to download data from: https://www.worldtides.info/apidocs
 * changes to the  program include placing your WiFi credentials as well as the Key from the above website which 
 * requires an account.  You will also have to change out the Lat and Lon as well as calculate the amount in seconds
 * you must add or subtract to obtain the current change from GMT.
*/

#include "notes.h" //file for your password
#include <TFT_eSPI.h>

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include "time.h"
#include <ArduinoJson.h>
#include <SPI.h>
#include <Servo.h>
static const int servoPin = 17; // Where you attached your servo pin
// IO pin assignments
//hours off UTC this represents Hawaii--
int timeStamp = -10;

int mover = 0;
double epic;
double epic2;
int que = 0;
int counterBabe = 0;
const int enablePin = 22;
// create the stepper motor object
Servo servo1;
int crank;
TFT_eSPI tft = TFT_eSPI();
time_t now;
bool done  = 0;
  struct tm timeinfo;
//------- Replace the following! ------
char ssid[] = myWifi;       // your network SSID (name)
char password[] = myPassword;  // your network key
// NTP server to request epoch time
const char* ntpServer = "pool.ntp.org";
double timerTrue = 60000;
// Variable to save current epoch time
unsigned long epochTime; 
long timerNow;
float diff = 0.0;
unsigned long previousMillis = 0;
unsigned long interval = 30000;
// Function that gets current epoch time
unsigned long getTime() {
  
  if (!getLocalTime(&timeinfo)) {
    //Serial.println("Failed to obtain time");
    return(0);
  }
  time(&now);
  return now;
}
// For Non-HTTPS requests
// WiFiClient client;

// For HTTPS requests
WiFiClientSecure client;
float intermed = 0.0;
bool hiLow[15];
int epochHiLow[15];
int qr = 0;
// Just the base of the URL you want to connect to
#define TEST_HOST "www.worldtides.info"

// Root cert of server we are connecting to
// Baltimore CyberTrust Root - Expires 12 May 2025
// (FYI, from a security point of view you should not trust certs from other people, including me!)
const char *server_cert = "-----BEGIN CERTIFICATE-----\n"
                                  "MIIDdzCCAl+gAwIBAgIEAgAAuTANBgkqhkiG9w0BAQUFADBaMQswCQYDVQQGEwJJ\n"
                                  "RTESMBAGA1UEChMJQmFsdGltb3JlMRMwEQYDVQQLEwpDeWJlclRydXN0MSIwIAYD\n"
                                  "VQQDExlCYWx0aW1vcmUgQ3liZXJUcnVzdCBSb290MB4XDTAwMDUxMjE4NDYwMFoX\n"
                                  "DTI1MDUxMjIzNTkwMFowWjELMAkGA1UEBhMCSUUxEjAQBgNVBAoTCUJhbHRpbW9y\n"
                                  "ZTETMBEGA1UECxMKQ3liZXJUcnVzdDEiMCAGA1UEAxMZQmFsdGltb3JlIEN5YmVy\n"
                                  "VHJ1c3QgUm9vdDCCASIwDQYJKoZIhvcNAQEBBQADggEPADCCAQoCggEBAKMEuyKr\n"
                                  "mD1X6CZymrV51Cni4eiVgLGw41uOKymaZN+hXe2wCQVt2yguzmKiYv60iNoS6zjr\n"
                                  "IZ3AQSsBUnuId9Mcj8e6uYi1agnnc+gRQKfRzMpijS3ljwumUNKoUMMo6vWrJYeK\n"
                                  "mpYcqWe4PwzV9/lSEy/CG9VwcPCPwBLKBsua4dnKM3p31vjsufFoREJIE9LAwqSu\n"
                                  "XmD+tqYF/LTdB1kC1FkYmGP1pWPgkAx9XbIGevOF6uvUA65ehD5f/xXtabz5OTZy\n"
                                  "dc93Uk3zyZAsuT3lySNTPx8kmCFcB5kpvcY67Oduhjprl3RjM71oGDHweI12v/ye\n"
                                  "jl0qhqdNkNwnGjkCAwEAAaNFMEMwHQYDVR0OBBYEFOWdWTCCR1jMrPoIVDaGezq1\n"
                                  "BE3wMBIGA1UdEwEB/wQIMAYBAf8CAQMwDgYDVR0PAQH/BAQDAgEGMA0GCSqGSIb3\n"
                                  "DQEBBQUAA4IBAQCFDF2O5G9RaEIFoN27TyclhAO992T9Ldcw46QQF+vaKSm2eT92\n"
                                  "9hkTI7gQCvlYpNRhcL0EYWoSihfVCr3FvDB81ukMJY2GQE/szKN+OMY3EU/t3Wgx\n"
                                  "jkzSswF07r51XgdIGn9w/xZchMB5hbgF/X++ZRGjD8ACtPhSNzkE1akxehi/oCr0\n"
                                  "Epn3o0WC4zxe9Z2etciefC7IpJ5OCBRLbf1wbWsaY71k5h+3zvDyny67G7fyUIhz\n"
                                  "ksLi4xaNmjICq44Y3ekQEe5+NauQrz4wlHrQMz2nZQ/1/I6eYs9HRCwBXbsdtTLS\n"
                                  "R9I4LtD+gdwyah617jzV/OeBHRnDJELqYzmp\n"
                                  "-----END CERTIFICATE-----\n";


void setup() {

  Serial.begin(115200);
  servo1.attach(servoPin);
  // put your setup code here, to run once:
  pinMode(32,OUTPUT);
  servo1.attach(servoPin);
  timerNow = millis();
  tft.init();
  tft.setRotation(1);
  tft.setTextSize(2);
  tft.fillScreen(TFT_BLACK);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  // Connect to the WiFI
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  Serial.println("");

  // Wait for connection
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected to ");
  Serial.println(ssid);
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  tft.println("CONNECTED");
  //--------
  epochTime = 0;
  qr = 0;
for(int z = 0; z < 15; z++){
  epochHiLow[z] = 0;
  hiLow[z] = 0;
}
  // Checking the cert is the best way on an ESP32
  // This will verify the server is trusted.
  client.setCACert(server_cert);
  configTime(0, 0, ntpServer);
  // If you don't want to verify the server
  // Unlike the fingerprint method of the ESP8266 which expires frequently
  // the cert lasts years, so I don't see much reason to ever
  // use this on the ESP32
   client.setInsecure();
   tft.setTextSize(1);
   makeHTTPRequest();
   epochTime = getTime();
   epochTime = epochTime + (3600 * timeStamp);
   Serial.print("earlytime:");
   Serial.println(epochTime);
   done = 1;

}
// this is where you request information from worldtides site
void makeHTTPRequest() {
  //tft.fillScreen(TFT_BLACK);
  qr = 0;
for(int z = 0; z < 15; z++){
  epochHiLow[z] = 0;
  hiLow[z] = 0;
}
  // Opening connection to server (Use 80 as port if HTTP)
  if (!client.connect(TEST_HOST, 443))
  {
    Serial.println(F("Connection failed"));
    return;
  }

  // give the esp a breather
  yield();

  // Send HTTP request
  client.print(F("GET "));
  // This is the second half of a request (everything that comes after the base URL)
  client.print("/api/v3?today&extremes&lat=20.783&lon=-156.467&localtime&datum=CD&key=8688cbf2-4b2a-4e50-9bb5-9a146976dd69"); // %2C == ,
  client.println(F(" HTTP/1.1"));

  //Headers
  client.print(F("Host: "));
  client.println(TEST_HOST);

  client.println(F("Cache-Control: no-cache"));

  if (client.println() == 0)
  {
    Serial.println(F("Failed to send request"));
    return;
  }
  //delay(100);
  // Check HTTP status
  char status[32] = {0};
  client.readBytesUntil('\r', status, sizeof(status));
  if (strcmp(status, "HTTP/1.1 200 OK") != 0)
  {
    Serial.print(F("Unexpected response: "));
    Serial.println(status);
    return;
  }

  // Skip HTTP headers
  char endOfHeaders[] = "\r\n\r\n";
  if (!client.find(endOfHeaders))
  {
    Serial.println(F("Invalid response"));
    return;
  }

  // This is probably not needed for most, but I had issues
  // with the Tindie api where sometimes there were random
  // characters coming back before the body of the response.
  // This will cause no hard to leave it in
  // peek() will look at the character, but not take it off the queue
  while (client.available() && client.peek() != '{')
  {
    char c = 0;
    client.readBytes(&c, 1);
    Serial.print(c);
    Serial.println("BAD");
  }




//StaticJsonDocument<2048> doc;
DynamicJsonDocument doc(12288);
DeserializationError error = deserializeJson(doc, client);

if (error) {
  Serial.print("deserializeJson() failed: ");
  Serial.println(error.c_str());
  return;
}

int Status = doc["status"]; // 200
int callCount = doc["callCount"]; // 1
const char* copyright = doc["copyright"]; // "Tidal data retrieved from www.worldtides.info. Copyright ...
float requestLat = doc["requestLat"]; // 20.617
double requestLon = doc["requestLon"]; // -105.230187
float responseLat = doc["responseLat"]; // 20.6167
float responseLon = doc["responseLon"]; // -105.25
const char* atlas = doc["atlas"]; // "UHSLC"
const char* station = doc["station"]; // "Puerto Vallarta"
const char* timezone = doc["timezone"]; // "America/Mexico_City"
const char* requestDatum = doc["requestDatum"]; // "CD"
const char* responseDatum = doc["responseDatum"]; // "CD"

for (JsonObject extreme : doc["extremes"].as<JsonArray>()) {

  long extreme_dt = extreme["dt"]; // 1672369386, 1672394864, 1672420872, 1672441211, 1672460268, ...
  const char* extreme_date = extreme["date"]; // "2022-12-29T21:03:06-06:00", "2022-12-30T04:07:44-06:00", ...
  float extreme_height = extreme["height"]; // 0.236, 0.737, 0.288, 0.525, 0.331, 0.792, 0.147
  const char* extreme_type = extreme["type"]; // "Low", "High", "Low", "High", "Low", "High", "Low"
  Serial.print(extreme_date);
  tft.setTextColor(TFT_GREEN, TFT_BLACK);
  tft.print(extreme_date);
  tft.setTextColor(TFT_RED, TFT_BLACK);
  tft.println(extreme_type);
  Serial.println(extreme_type);
  std::string bongo =  extreme_type;
int inter = (extreme_date[11] - '0') * 10 + (extreme_date[12] - '0');
//Serial.print("here:  ");
//Serial.println(inter);
 epochHiLow[qr] = extreme_dt + (3600 * timeStamp);
 if(bongo == "High"){
  hiLow[qr] = 1;
 }else hiLow[qr] = 0;
 //hiLow[q] = extreme_height;
 qr++;
//Serial.print(extreme_height); 

//Serial.print(height_date);
}
//Serial.println(qr);
 for(int i = 0; i <= qr; i++){
Serial.print(hiLow[i]);
Serial.print("      ");
Serial.print(epochHiLow[i]);
Serial.println();


  }
}



void loop() {
  digitalWrite(32,HIGH);
  tft.drawNumber(que, 180, 30, 7);
  //check epic time every 60 minutes
  if(done == 1 && epochHiLow[0] != 0) {
    for(int zone = 0; zone <= qr; zone++){
      if( epochHiLow[zone + 1] == 0){
      restarter();     
      }
      que = zone; 
      if(epochHiLow[zone] > epochTime){
        if(hiLow[zone] ==  1){ 
          crank = -1;         
          delay(100);              
            moving(crank);
                   
        } else  {
          crank = 1;         
    
            //servo1.write(0);
            delay(1000);
            moving(crank);
        
        }         
         done = 0;
         zone = qr;
      }     
    }
  }
  //This checks the Time  server every minute for correct time 
  if( millis() - timerNow >= timerTrue) {
  //tft.fillScreen(TFT_BLACK);
  //makeHTTPRequest(); 
  epochTime = getTime();
  epochTime = epochTime + (3600 * timeStamp);
  tft.drawNumber(epochTime, 0, 100, 2);
  Serial.print("Epoch Time: ");
  Serial.println(epochTime);
  showTime(timeinfo);
  timerNow = millis();
  if(epochTime > epochHiLow[que]){
  done = 1;
  }
  moving(crank);
  delay(1000);

  }
}
void showTime(tm localTime) {
  Serial.print(localTime.tm_mday);
  Serial.print('/');
  Serial.print(localTime.tm_mon + 1);
  Serial.print('/');
  Serial.print(localTime.tm_year - 100);
  Serial.print('-');
  Serial.print(localTime.tm_hour);
  Serial.print(':');
  Serial.print(localTime.tm_min);
  Serial.print(':');
  Serial.print(localTime.tm_sec);
  Serial.print(" Day of Week ");
  if (localTime.tm_wday == 0)   Serial.println(7);
  else Serial.println(localTime.tm_wday);
}
void moving( int drug){
  if(que !=0){
            epic = ( epochTime - epochHiLow[que - 1] );
            epic2 = (epochHiLow[que] - epochHiLow[que - 1]);
            diff = 100.0 * float(( epic / epic2));
  } else diff = 100.0 * float((epochHiLow[que] - epochTime)/ 21600);
         diff = constrain(diff, 1,100);
         Serial.print( "Percentage of time before next: ");
         Serial.println(diff);
         Serial.print("epochTimeofTide");
         Serial.println(epochHiLow[que]); 
         tft.drawString("      ",100, 100, 4);
         tft.drawNumber(diff, 100, 100, 4);
         //tft.drawNumber(diff, 100, 100, 4);
         if(hiLow[que] == 0) { 
           tft.drawString("% to low   ", 130, 100, 4);
         } else tft.drawString("% to High",  130, 100, 4);
         if(drug){
           mover = map(diff, 1,100, 1, 180);
           digitalWrite(32, LOW);
         }else {
           mover = map(diff, 1,100, 180, 1);
           digitalWrite(32, HIGH);
          
         }
         servo1.write(mover);
           
}
void restarter(){
      Serial.println("Restarting in 10 seconds");
      delay(10000);  
      ESP.restart();
       
      
}
