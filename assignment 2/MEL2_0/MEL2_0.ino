#include "codeArt.h"

int DIN = 2;
int CS = 3;
int CLK = 4;

int maxInUse = 1;    //change this variable to set how many MAX7219's you'll use

                     // define max7219 registers
byte max7219_reg_noop        = 0x00;
byte max7219_reg_digit0      = 0x01;
byte max7219_reg_digit1      = 0x02;
byte max7219_reg_digit2      = 0x03;
byte max7219_reg_digit3      = 0x04;
byte max7219_reg_digit4      = 0x05;
byte max7219_reg_digit5      = 0x06;
byte max7219_reg_digit6      = 0x07;
byte max7219_reg_digit7      = 0x08;
byte max7219_reg_decodeMode  = 0x09;
byte max7219_reg_intensity   = 0x0a;
byte max7219_reg_scanLimit   = 0x0b;
byte max7219_reg_shutdown    = 0x0c;
byte max7219_reg_displayTest = 0x0f;

void putByte(byte data)
{
  byte i = 8;
  byte mask;
  while(i > 0)
  {
    mask = 0x01 << (i - 1);      // get bitmask
    digitalWrite( CLK, LOW);   // tick
    if (data & mask) digitalWrite(DIN, HIGH);
    else             digitalWrite(DIN, LOW);
    digitalWrite(CLK, HIGH);   // tock
    --i;                         // move to lesser bit
  }
}

void maxSingle( byte reg, byte col)
{
  digitalWrite(CS, LOW);       // begin     
  putByte(reg);                  // specify register
  putByte(col);                  // put data
  digitalWrite(CS,HIGH); 
}

void initMax7219()
{
  //initiation of the max 7219
  maxSingle(max7219_reg_scanLimit, 0x07);      
  maxSingle(max7219_reg_decodeMode, 0x00);     // using an led matrix (not digits)
  maxSingle(max7219_reg_shutdown,   0x01);     // not in shutdown mode
  maxSingle(max7219_reg_displayTest, 0x00);    // no display test
  for (int e = 1; e <= 8; e++) maxSingle(e,0); // empty registers, turn all LEDs off 
  maxSingle(max7219_reg_intensity, 0x0f & 0x0f);  // the first 0x0f is the value you can set
}

void setup ()
{
  pinMode(DIN, OUTPUT);
  pinMode(CLK,  OUTPUT);
  pinMode(CS,   OUTPUT);
  initMax7219();
  Serial.begin(9600);
}

void loop ()
{
  M();
  delay(1000);
  E();
  delay(1000);
  L();
  delay(1000);
  sword();
  delay(1000);
}

void M()
{
  maxSingle(1, xxx__xxx);
  maxSingle(2, xx_xx_xx);
  maxSingle(3, xx____xx);
  maxSingle(4, xx____xx);
  maxSingle(5, xx____xx);
  maxSingle(6, xx____xx);
  maxSingle(7, xx____xx);
  maxSingle(8, xx____xx);
}

void E()
{
  maxSingle(1, xxxxxxxx);
  maxSingle(2, xxxxxxxx);
  maxSingle(3, xx______);
  maxSingle(4, xxxxxxxx);
  maxSingle(5, xxxxxxxx);
  maxSingle(6, xx______);
  maxSingle(7, xxxxxxxx);
  maxSingle(8, xxxxxxxx);
}

void L()
{
  maxSingle(1, 0b11000000);
  maxSingle(2, xx______);
  maxSingle(3, xx______);
  maxSingle(4, xx______);
  maxSingle(5, xx______);
  maxSingle(6, xx______);
  maxSingle(7, 255);
  maxSingle(8, 255);
}

void arrayToLED(byte inputArray[])
{
  for(int i = 0; i < 8; i++){
    maxSingle(i + 1, inputArray[i]);
  }
  Serial.print(sizeof(inputArray));
}

void sword()
{
  byte Sword [8] = {0x03 , 0x07 , 0x0E , 0x5C , 0xF8 , 0x30 , 0x58 , 0x90};
  arrayToLED(Sword);
}


