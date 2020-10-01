#define ENABLE_GxEPD2_GFX 1

#include <GxEPD2_4G.h> // needs be first include
GxEPD2_4G<GxEPD2_290_T5, GxEPD2_290_T5::HEIGHT> display(GxEPD2_290_T5(/*CS=D8*/ SS, /*DC=D3*/ 0, /*RST=D4*/ 2, /*BUSY=D2*/ 4));

#include "greyscale_easy.h"

void setup() {
  display.init(115200);

  display.setFullWindow();
  display.setRotation(3); 
  display.fillScreen(GxEPD_WHITE);

  // PREPARE PAGE
  display.firstPage();
  do{
      display.setCursor(193, 115);
      display.setTextColor(GxEPD_BLACK);
      display.println("... initializing");
  }
  // RENDER PAGE
  while (display.nextPage());

  delay(3000);

  // DISPLAY IMAGE
  display.drawImage_4G(image, 4, 0, 0, 128, 296);
}

void loop() {

}
