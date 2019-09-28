#include <SPI.h>
#include "grayscale.h"

unsigned char HRES,VRES_byte1,VRES_byte2;

#define BUSY_PIN 4
#define DC_PIN 0
#define RESET_PIN 2
#define CS_PIN 15

void write_cmd(unsigned char command)
{
    digitalWrite(CS_PIN, 0);                  
    digitalWrite(DC_PIN, 0);
    SPI.transfer(command);
    digitalWrite(CS_PIN, 1);                  
    digitalWrite(DC_PIN, 1);
}
void write_data(unsigned char data)
{
    digitalWrite(CS_PIN, 0);
    SPI.transfer(data);
    digitalWrite(CS_PIN, 1);
}

void check_busy(void)
{
  unsigned char busy;
  do
  {
    write_cmd(0x71);
    busy = digitalRead(BUSY_PIN);
    busy =!(busy & 0x01);        
  }
  while(busy);   
  delay(200);                       
}

void reset(void)
{
  digitalWrite(RESET_PIN, 0);   // Module reset
  delay(1000);
  digitalWrite(RESET_PIN, 1);
  delay(1000);
}

void display_sleep(void) //Enter deep sleep mode
{
    write_cmd(0X50);
    write_data(0xf7);  
    write_cmd(0X02);   //power off
    check_busy();
    write_cmd(0X07);   //deep sleep
    write_data(0xA5);
}

void init_display_gray(void)
{
    HRES=0x80;          //128
    VRES_byte1=0x01;    //296
    VRES_byte2=0x28;
    reset();
    write_cmd(0x01);    //POWER SETTING
    write_data (0x03);
    write_data (0x00);      
    write_data (0x2b);                                  
    write_data (0x2b);   
    write_data (0x03);

    write_cmd(0x06);    //booster soft start
    write_data (0x17);  //A
    write_data (0x17);  //B
    write_data (0x17);  //C 
    
    write_cmd(0x04);
    check_busy();
    
    write_cmd(0x00);    //panel setting
    write_data(0xbf);   //LUT from register, 128x296
    write_data(0x0d);   //VCOM to 0V fast

    write_cmd(0x30);    //PLL setting
    write_data (0x3a);

    write_cmd(0x61);    //resolution setting
    write_data (HRES);          
    write_data (VRES_byte1);   
    write_data (VRES_byte2);

    write_cmd(0x82);    //vcom_DC setting
    write_data (0x08);

    write_cmd(0X50);    //VCOM AND DATA INTERVAL SETTING      
    write_data(0x97);
}

void pic_display_4bit (void)
{
  uint32_t i,j;
  uint8_t temp1,temp2,temp3;

    //old  data
    write_cmd(0x10);        

    for(i=0;i<4736;i++)
    { 
      temp3=0;
      for(j=0;j<4;j++)  
      {
        temp1 = image[i*4+j];
        temp2 = temp1&0xF0;
        if(temp2 == 0xF0)
          temp3 |= 0x01;  //white
        else if(temp2 == 0x00)
          temp3 |= 0x00;  //black
        else if((temp2>0xA0)&&(temp2<0xF0)) 
          temp3 |= 0x01;  //gray1
        else 
          temp3 |= 0x00;  //gray2
        temp3 <<= 1;  
        temp1 <<= 4;
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0) //white
          temp3 |= 0x01;
        else if(temp2 == 0x00)  //black
          temp3 |= 0x00;
        else if((temp2>0xA0)&&(temp2<0xF0))
          temp3 |= 0x01;  //gray1
        else    
            temp3 |= 0x00;  //gray2 
        if(j!=3)          
        temp3 <<= 1;  
        
      
     }  
        write_data(temp3);     
    }
    //new  data
    write_cmd(0x13);        

    for(i=0;i<4736;i++)
    { 
      temp3=0;
      for(j=0;j<4;j++)  
      {
        temp1 = image[i*4+j];
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0)
          temp3 |= 0x01;  //white
        else if(temp2 == 0x00)
          temp3 |= 0x00;  //black
        else if((temp2>0xA0)&&(temp2<0xF0)) 
          temp3 |= 0x00;  //gray1
        else 
          temp3 |= 0x01;  //gray2
        temp3 <<= 1;  
        temp1 <<= 4;
        temp2 = temp1&0xF0 ;
        if(temp2 == 0xF0) //white
          temp3 |= 0x01;
        else if(temp2 == 0x00)  //black
          temp3 |= 0x00;
        else if((temp2>0xA0)&&(temp2<0xF0)) 
          temp3 |= 0x00;  //gray1
        else    
            temp3 |= 0x01;  //gray2
        if(j!=3)        
        temp3 <<= 1;        
      }  
      write_data(temp3);     
    }
}

void pic_display(const unsigned char* picData)
{
    unsigned int i;
    write_cmd(0x10);        //Transfer old data
    for(i=0;i<4736;i++)      
    write_data(0xff); 
  
    write_cmd(0x13);        //Transfer new data
    for(i=0;i<4736;i++)      
  {
    write_data(*picData);
    picData++;
  }

}

void full_display(void pic_display(void))
{

    
    pic_display(); //picture
    lut(); //Power settings
    write_cmd(0x12);     //DISPLAY REFRESH   
    delay(200);      //!!!The delay here is necessary, 200uS at least!!!     
    check_busy();
}

void display_clean(void)
{
  unsigned int i;
    write_cmd(0x10);
    for(i=0;i<4736;i++)      
    {
        write_data(0xff);  
    }  
    delay(2);  

    write_cmd(0x13);
    for(i=0;i<4736;i++)      
    {
        write_data(0xff);  
    }  
    delay(2);     
}

void lut(void)
{
  unsigned int count;  
  {
    write_cmd(0x20);             //vcom
    for(count=0;count<44;count++)
      {write_data(lut_vcom[count]);}
    
  write_cmd(0x21);             //red not in use
  for(count=0;count<42;count++)
    {write_data(lut_ww[count]);}

    write_cmd(0x22);             //bw r
    for(count=0;count<42;count++)
      {write_data(lut_bw[count]);}

    write_cmd(0x23);             //wb w
    for(count=0;count<42;count++)
      {write_data(lut_wb[count]);}

    write_cmd(0x24);             //bb b
    for(count=0;count<42;count++)
      {write_data(lut_bb[count]);}

    write_cmd(0x25);             //vcom
    for(count=0;count<42;count++)
      {write_data(lut_ww[count]);}
  }          
}

void display_refresh(void)
{
    write_cmd(0x12);  
    delay(200);  
    check_busy();
}

void setup(){
    pinMode(RESET_PIN, OUTPUT);
    pinMode(BUSY_PIN, INPUT);
    pinMode(DC_PIN, OUTPUT);
    pinMode(CS_PIN, OUTPUT);

    SPI.begin();
    
    init_display_gray();
    full_display(pic_display_4bit); //pic1
    display_sleep(); //Enter deep sleep mode
}

void loop() {
}
