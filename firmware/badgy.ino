#include <GxEPD.h>
#include <GxGDEH029A1/GxGDEH029A1.cpp>
#include <GxIO/GxIO_SPI/GxIO_SPI.cpp>
#include <GxIO/GxIO.cpp>
#include <Fonts/FreeSansBoldOblique24pt7b.h>
#include "hello.h"

GxIO_Class io(SPI, SS, D3, D4);
GxEPD_Class display(io);

byte buttonState = 0;
byte lastButtonState = 0;
unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;

void setup()
{  
  Serial.begin(115200);
  Serial.println();
  Serial.println("Booting Sketch...");

  display.init();
  pinMode(1,INPUT_PULLUP);  //down
  pinMode(3,INPUT_PULLUP);  //up
  pinMode(5,INPUT_PULLUP);  //center
  pinMode(12,INPUT_PULLUP); //right
  pinMode(16,INPUT);        //left GPIO16 has an external pull-up resistor
}

void loop()
{
  byte reading =  (digitalRead(1)  == 0 ? 0 : (1<<0)) |
                  (digitalRead(3)  == 0 ? 0 : (1<<1)) |
                  (digitalRead(5)  == 0 ? 0 : (1<<2)) |
                  (digitalRead(12) == 0 ? 0 : (1<<3)) |
                  (digitalRead(16) == 0 ? 0 : (1<<4));
                  
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
              //do something on down
              break;
            case 1:
              //do something on up
              break;
            case 2:
              showBadge(); //do something on center
              break;
            case 3:
              //do something on right
              break;
            case 4:
              //do something on left
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

void showBadge()
{
  display.setRotation(3);
  display.fillScreen(GxEPD_WHITE);
  display.drawBitmap(hello, 0, 0, 296, 128, GxEPD_WHITE);
  const char* name = "badgy"; //name on badge
  const GFXfont* f = &FreeSansBoldOblique24pt7b;
  display.setTextColor(GxEPD_BLACK);
  display.setFont(f);
  display.setCursor(75,100);
  display.println(name);
  display.update();
}
