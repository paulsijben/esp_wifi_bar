#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h>
#endif

#define PIN            2  //pin on the ESP
#define NUMPIXELS      9
#define BRIGHTNESS 64
const float fraction = NUMPIXELS / 100;

Adafruit_NeoPixel pixels = Adafruit_NeoPixel(NUMPIXELS, PIN, NEO_GRB + NEO_KHZ800);


#include "ESP8266WiFi.h"

const int RSSI_MAX = -50; // define maximum strength of signal in dBm
const int RSSI_MIN = -80; // define minimum strength of signal in dBm

const int displayEnc = 1; // set to 1 to display Encryption or 0 not to display

String myssid = "";
int32_t myrssi = -999999;
int32_t rssi;
void setup() {
  Serial.begin(115200);
  Serial.println("********** Wifi-O-Meter ********************");
  // Set WiFi to station mode and disconnect from an AP if it was previously connected
  WiFi.mode(WIFI_STA);
  WiFi.disconnect();
  delay(2000);

  while (myssid == "") {
    Serial.println("Wifi scan started");

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("Wifi scan ended");
    if (n > 0) {
      Serial.print(n);
      Serial.println(" networks found");
      for (int i = 0; i < n; ++i) {
        // Print SSID and RSSI for each network found
        Serial.print(i + 1);
        Serial.print(") ");
        Serial.print(WiFi.SSID(i));// SSID

        rssi = WiFi.RSSI(i);
        Serial.print(rssi);//Signal strength in dBm
        Serial.println("dBm");
        if (rssi > myrssi) {
          myrssi = rssi;
          myssid = WiFi.SSID(i);
        }

        delay(10);
      }
    }
    Serial.println("Setup done");
    Serial.print("myssid= ");
    Serial.println(myssid);
  };
  pixels.begin();
}
void loop() {
  Serial.println("Wifi scan started");
  myrssi = -999999;
  // WiFi.scanNetworks will return the number of networks found
  int n = WiFi.scanNetworks();
  Serial.println("Wifi scan ended");
  if (n == 0) {
    Serial.println("no networks found");
    setLedBar(0);
  } else {
    //Serial.print(n);
    //Serial.println(" networks found");
    for (int i = 0; i < n; ++i) {
      // Print SSID and RSSI for each network found
      rssi = WiFi.RSSI(i);
      //Serial.print(WiFi.SSID(i));
      //Serial.print(" => ");
      //Serial.println(rssi);
      if ((WiFi.SSID(i) == myssid) and (rssi > myrssi)) {
        myrssi = rssi;
      }
      //delay(10);
    }
  }
  //Serial.print("myssid=>");
  //Serial.println(myrssi);
  setLedBar(dBmtoPercentage(myrssi));
  //Serial.println("");

  // Wait a bit before scanning again
  //delay(500);
  WiFi.scanDelete();
}// loop


int r;
int g;
int b;

void setLedBar(float val) {
  //Serial.print("setledbar:");
  //Serial.println(val);
  if (val > 0.95) {
    r = 0;
    g = 0;
    b = 255;
  }
  else if (val > 0.6) {
    r = 0;
    g = 255-(255*(val-.6)*3);
    b = (255*(val-.6)*3);
  }
  else if (val > 0.3) {
    r = 255-(255*(val-.3)*3);
    g = 255;
    b = 0;
  }
  else {
    r = 255;
    g = (255*(val-.3)*3);
    b = 0;
  };
  //Serial.println(NUMPIXELS * val);
  for (int i = 0; i < NUMPIXELS; i++) {

    //Serial.print(i);

    if (i <= (NUMPIXELS * val)) {
      //Serial.println("*");
      pixels.setPixelColor(i, r, g, b);
    }
    else {
      //Serial.println(".");
      pixels.setPixelColor(i, 0, 0, 0);
    };

  };
  pixels.show();
}


float dBmtoPercentage(int dBm)
{
  float quality;
  if (dBm <= RSSI_MIN)
  {
    quality = 0;
  }
  else if (dBm >= RSSI_MAX)
  {
    quality = 1;
  }
  else
  { //so e.g. (-51dbm --80)= 29 / (-50--80) =30
    //Serial.println(float( dBm - RSSI_MIN ));
    //Serial.println(float(RSSI_MAX-RSSI_MIN));
    quality = float( dBm - RSSI_MIN )/float(RSSI_MAX-RSSI_MIN);
    //Serial.println(quality);
  }

  return quality ;
}//dBmtoPercentage
