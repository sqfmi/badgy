  /* e-paper display lib */
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
/* include any other fonts you want to use https://github.com/adafruit/Adafruit-GFX-Library */
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeSansBoldOblique24pt7b.h>
/* "Hello my name is" background image */
#include "hello.h"
/* WiFi  libs*/
#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>

/* change this to your name, you may have to adjust font size and cursor location in showHello() to perfectly center the text */
const char* name = "Badgy";

/* Always include the update server, or else you won't be able to do OTA updates! */
/**/const int port = 8888;
/**/ESP8266WebServer httpServer(port);
/**/ESP8266HTTPUpdateServer httpUpdater;
/*                                                                                */

/* Configure pins for display */
GxIO_Class io(SPI, SS, 0, 2);
GxEPD_Class display(io); // default selection of D4, D2

/* A single byte is used to store the button states for debouncing */
byte buttonState = 0;
byte lastButtonState = 0;   //the previous reading from the input pin
unsigned long lastDebounceTime = 0;  //the last time the output pin was toggled
unsigned long debounceDelay = 50;    //the debounce time

void setup()
{  
  display.init();
  
  pinMode(1,INPUT_PULLUP); //down
  pinMode(3,INPUT_PULLUP); //left
  pinMode(5,INPUT_PULLUP); //center
  pinMode(12,INPUT_PULLUP); //right
  pinMode(10,INPUT_PULLUP); //up
  
  /* Enter OTA mode if the center button is pressed */
  if(digitalRead(5)  == 0){
    /* WiFi Manager automatically connects using the saved credentials, if that fails it will go into AP mode */
    WiFiManager wifiManager;
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.autoConnect("Badgy AP");
    /* Once connected to WiFi, startup the OTA update server if the center button is held on boot */
    httpUpdater.setup(&httpServer);
    httpServer.begin();
    showIP();
    while(1){
      httpServer.handleClient();
    }
  }
  showHello(); //show "Hello my name is" immediately on boot
}

void loop()
{  
  byte reading =  (digitalRead(1)  == 0 ? 0 : (1<<0)) | //down
                  (digitalRead(3)  == 0 ? 0 : (1<<1)) | //left
                  (digitalRead(5)  == 0 ? 0 : (1<<2)) | //center
                  (digitalRead(12) == 0 ? 0 : (1<<3)) | //right
                  (digitalRead(10) == 0 ? 0 : (1<<4));  //up
                  
  if(reading != lastButtonState){
    lastDebounceTime = millis();
  }
  if((millis() - lastDebounceTime) > debounceDelay){
    if(reading != buttonState){
      buttonState = reading;
      for(int i=0; i<5; i++){
        if(bitRead(buttonState, i) == 0){
          switch(i){
            case 0:
              //do something when the user presses down
              showText("You pressed the down button!");
              break;
            case 1:
              //do something when the user presses left
              showText("You pressed the left button!");
              break;
            case 2:
              //do something when the user presses center
              showText("You pressed the center button!");
              break;
            case 3:
              //do something when the user presses right
              showText("You pressed the right button!");
              break;
            case 4:
              //do something when the user presses up
              showText("You pressed the up button!");
              break;                              
            default:
              break;
          }
        }
      }
    }
  }
  lastButtonState = reading;
}

void configModeCallback (WiFiManager *myWiFiManager){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,50);
  display.println("Connect to Badgy AP");
  display.println("to setup your WiFi!");
  display.update();  
}

void showText(char *text)
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(10,70);
  display.println(text);
  display.update();
}

void showIP(){
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const GFXfont* f = &FreeMonoBold9pt7b ;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(0,10);

  String url = WiFi.localIP().toString() + ":"+String(port)+"/update";
  byte charArraySize = url.length() + 1;
  char urlCharArray[charArraySize];
  url.toCharArray(urlCharArray, charArraySize);

  display.println("You are now connected!");
  display.println("");  
  display.println("Go to:");
  display.println(urlCharArray);
  display.println("to upload a new sketch.");
  display.update();  
}

void showHello()
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(hello, 0, 0, 296, 128, GxEPD_WHITE);
  const GFXfont* f = &FreeSansBoldOblique24pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(70,100);
  display.println(name);
  display.update();
}
