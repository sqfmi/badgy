// include library, include base class, make path known
#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/FreeSansBoldOblique24pt7b.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include "hello.h"

GxIO_Class io(SPI, SS, D3, D4);
GxEPD_Class display(io); // default selection of D4, D2

//ESP WiFi
#include <ESP8266WiFi.h>
#include <DNSServer.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPUpdateServer.h>
#include <WiFiManager.h>

ESP8266WebServer httpServer(80);
ESP8266HTTPUpdateServer httpUpdater;

byte buttonState = 0;
byte lastButtonState = 0;   // the previous reading from the input pin
unsigned long lastDebounceTime = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers
byte updateCounter = 0;
bool update = false;

void setup()
{  
  display.init();
  showName();

  pinMode(1,INPUT_PULLUP); //down
  pinMode(3,INPUT_PULLUP); //up
  pinMode(5,INPUT_PULLUP); //center
  pinMode(12,INPUT_PULLUP); //right
  pinMode(10,INPUT_PULLUP); //left
}

void loop()
{
  if(update){
    httpServer.handleClient();
  }
  
  byte reading =  (digitalRead(1)  == 0 ? 0 : (1<<0)) | //down
                  (digitalRead(3)  == 0 ? 0 : (1<<1)) | //up
                  (digitalRead(5)  == 0 ? 0 : (1<<2)) | //center
                  (digitalRead(12) == 0 ? 0 : (1<<3)) | //right
                  (digitalRead(10) == 0 ? 0 : (1<<4)); //left
                  //v1 board (digitalRead(16) == 0 ? 0 : (1<<4)); //left
                  
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
              //
              break;
            case 1:
              //
              break;
            case 2:
              updateCounter++;
              if(updateCounter > 4){
                updateCounter = 0;
                wifiUpdate();
              }
              break;
            case 3:
              //
              break;
            case 4:
              //
              break;                              
            default:
              break;
          }
        }
      }
    }
  }
  //showTwitter();
  lastButtonState = reading;
}



void showName()
{
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  //image dimensions must be multiples of 8!!!
  //convert at http://javl.github.io/image2cpp/
  display.drawBitmap(hello, 0, 0, 296, 128, GxEPD_WHITE);
  const char* name = "Badgy"; //update your name here
  const GFXfont* f = &FreeSansBoldOblique24pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(85,100);
  display.println(name);
  display.update();
}

void wifiUpdate(){
  //update screen
  display.setRotation(3); //even = portrait, odd = landscape
  display.fillScreen(GxEPD_WHITE);
  const char* message = "Staring Up Badgy WiFi AP...";
  const GFXfont* f = &FreeMonoBold9pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(5,20);
  display.println(message);
  display.update();
  delay(1000);  
    
  WiFiManager wifiManager;
  wifiManager.autoConnect("Badgy AP");
  httpUpdater.setup(&httpServer);
  httpServer.begin();
  update = true;
  
  display.fillScreen(GxEPD_WHITE);
  display.setCursor(5,20);
  display.println(WiFi.localIP());
  display.update(); 
}
