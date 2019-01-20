/*********************************************************************************************************
*
* File                : DustSensor
* Hardware Environment: 
* Build Environment   : Arduino
* Version             : V1.0.5-r2
* By                  : WaveShare
*
*                                  (c) Copyright 2005-2011, WaveShare
*                                       http://www.waveshare.net
*                                       http://www.waveshare.com   
*                                          All Rights Reserved
*
*********************************************************************************************************/
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);


#define        COV_RATIO                       0.2            //ug/mmm / mv
#define        NO_DUST_VOLTAGE                 400            //mv
#define        SYS_VOLTAGE                     5000           


/*
I/O define
*/
const int iled = 12;                                            //drive the led of sensor
const int vout = A7;                                            //analog input

/*
variable
*/
float density, voltage;
int   adcvalue;

/*
private function
*/
int Filter(int m)
{
  static int flag_first = 0, _buff[10], sum;
  const int _buff_max = 10;
  int i;
  
  if(flag_first == 0)
  {
    flag_first = 1;

    for(i = 0, sum = 0; i < _buff_max; i++)
    {
      _buff[i] = m;
      sum += _buff[i];
    }
    return m;
  }
  else
  {
    sum -= _buff[0];
    for(i = 0; i < (_buff_max - 1); i++)
    {
      _buff[i] = _buff[i + 1];
    }
    _buff[9] = m;
    sum += _buff[9];
    
    i = sum / 10.0;
    return i;
  }
}


void setup(void)
{
  pinMode(iled, OUTPUT);
  digitalWrite(iled, LOW);                                     //iled default closed
  
  Serial.begin(115200);                                         //send and receive at 9600 baud
  Serial.print("*********************************** WaveShare ***********************************\n");

  display.begin(SSD1306_SWITCHCAPVCC, 0x3C);  // initialize with the I2C addr 0x3D (for the 128x64)
  display.clearDisplay();
//  display.setRotation(2);
  display.setTextSize(2);
  display.setTextColor(WHITE);
  display.setCursor(0,25);
  display.print("INDY ROBOT");
  display.display();
  delay(2000);
  display.clearDisplay();
}

void loop(void)
{
  /*
  get adcvalue
  */
  digitalWrite(iled, HIGH);
  delayMicroseconds(280);
  adcvalue = analogRead(vout);
  digitalWrite(iled, LOW);
  
  adcvalue = Filter(adcvalue);
  
  /*
  covert voltage (mv)
  */
  voltage = (SYS_VOLTAGE / 1024.0) * adcvalue * 11;
  
  /*
  voltage to density
  */
  if(voltage >= NO_DUST_VOLTAGE)
  {
    voltage -= NO_DUST_VOLTAGE;
    
    density = voltage * COV_RATIO;
  }
  else
    density = 0;
    
  /*
  display the result
  */
  Serial.print("The current dust concentration is: ");
  Serial.print(density);
  Serial.print(" ug/m3\n");  

 /*
  display oled
  */
  display.clearDisplay(); 
  
  display.setTextColor(WHITE);  //  กำหนดสีตัวอักษร
  display.setCursor(0,0);       //  กำหนดตำแหน่งที่จะแสดงผลตัวอักษร

  display.setTextSize(2);       //  กำหนดขนาดตัวอักษร
  display.print("  PM 2.5 ");              //  แสดงข้อความออกจากจอ OLED
  display.println("  ");
  
  display.setTextSize(3);       //  กำหนดขนาดตัวอักษร
  display.print(" ");
  display.print(density);

  display.display();  
  
  delay(1000);
}
