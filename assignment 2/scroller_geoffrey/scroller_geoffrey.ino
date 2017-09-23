#include <Arduino.h>
#include "bitmapfont.h"

/* code for max 7219 from maxim, 
reduced and optimised for useing more then one 7219 in a row,
______________________________________

 Code History:
 --------------

The orginal code was written for the Wiring board by:`
 * Nicholas Zambetti and Dave Mellis /Interaction Design Institute Ivrea /Dec 2004
 * http://www.potemkin.org/upCSs/Wiring/MAX7219.txt

First modification by:
 * Marcus Hannerstig/  K3, malmï¿½ hï¿½gskola /2006
 * http://www.xlab.se | http://arduino.berlios.de

This version is by:
 * tomek ness /FH-Potsdam / Feb 2007
 * http://design.fh-potsdam.de/ 

 * @acknowledgements: eric f. 
 * 
Small adjustments for Technology for Interaction course by:
 * Joey van der Bie / Feb 2017
 * j.h.f.van.der.bie@hva.nl
  
  

-----------------------------------

General notes: 


-if you are only using one max7219, then use the function maxSingle to control
 the little guy ---maxSingle(register (1-8), collum (0-255))

-if you are using more then one max7219, and they all should work the same, 
then use the function maxAll ---maxAll(register (1-8), collum (0-255))

-if you are using more than one max7219 and just want to change something
at one little guy, then use the function maxOne
---maxOne(Max you wane controll (1== the first one), register (1-8), 
collum (0-255))

/* During initiation, be sure to send every part to every max7219 and then
 upCS it.
For example, if you have five max7219's, you have to send the scanLimit 5 times
before you CS it-- other wise not every max7219 will get the data. the
function maxInUse  keeps track of this, just tell it how many max7219 you are
using.
*/

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
	
void putByte(byte data) {
  byte i = 8;
  byte mask;
  while(i > 0) {
    mask = 0x01 << (i - 1);      // get bitmask
    digitalWrite( CLK, LOW);   // tick
    if (data & mask){            // choose bit
      digitalWrite(DIN, HIGH);// send 1
    }else{
      digitalWrite(DIN, LOW); // send 0
    }
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

void maxAll (byte reg, byte col) {    // initialize  all  MAX7219's in the system
  int c = 0;
  digitalWrite(CS, LOW);  // begin     
  for ( c =1; c<= maxInUse; c++) {
  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data
    }
  digitalWrite(CS,HIGH);
}

void maxOne(byte maxNr, byte reg, byte col) {    
//maxOne is for adressing different MAX7219's, 
//whilele having a couple of them cascaded

  int c = 0;
  digitalWrite(CS, LOW);  // begin     

  for ( c = maxInUse; c > maxNr; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }

  putByte(reg);  // specify register
  putByte(col);//((data & 0x01) * 256) + data >> 1); // put data 

  for ( c =maxNr-1; c >= 1; c--) {
    putByte(0);    // means no operation
    putByte(0);    // means no operation
  }
  digitalWrite(CS,HIGH); 
}

void initMax7219()
{
  //initiation of the max 7219
  maxAll(max7219_reg_scanLimit, 0x07);      
  maxAll(max7219_reg_decodeMode, 0x00);  // using an led matrix (not digits)
  maxAll(max7219_reg_shutdown, 0x01);    // not in shutdown mode
  maxAll(max7219_reg_displayTest, 0x00); // no display test
  for (int e = 1; e <= 8; e++) {    // empty registers, turn all LEDs off 
    maxAll(e,0);
  }
  maxAll(max7219_reg_intensity, 0x0f & 0x0f);    // the first 0x0f is the value you can set
                                                  // range: 0x00 to 0x0f
}

/////////////////////////////////////////////////////////////
// Framebuffer stuff

#define NROWS 8
#define NCOLS 8

// Definieert de framebuffer. Deze array met bytes wordt constant getekend in de loop.
// De frameBuffer wordt geinitialiseerd met een diagonale lijn als testpatroon
byte frameBuffer[8] = {1, 2, 4, 8, 16, 32, 64, 128};

// Stuur de bytes van de frameBuffer naar de MAX7219
void drawFrameBuffer()
{
  for (int iRow = 0; iRow < NROWS; iRow++) maxSingle(iRow + 1, frameBuffer[iRow]);   
}

// Zet alle bytes in de frameBuffer op 0
void clearFrameBuffer()
{
  for (int iRow = 0; iRow < NROWS; iRow++) frameBuffer[iRow] = 0;
}

// Kopieer een bitmap (array van 8 bytes) naar de frameBuffer
void putBitmap(byte *bitmap)
{
  for (int iRow = 0; iRow < NROWS; iRow++) frameBuffer[iRow] = bitmap[iRow];
}

// Kopieer een bitmap (array van 8 bytes) naar de frameBuffer MAAR:
// behoud alle LEDs die aanstaan 
void ORPutBitmap(byte *bitmap)
{
  for (int iRow = 0; iRow < NROWS; iRow++) frameBuffer[iRow] |= bitmap[iRow];
}

// Zet een bepaalde LED in de matrix aan
// x vanaf links
// y vanaf boven
void putPixel(byte x, byte y)
{
  frameBuffer[y] |=  (1 << x);
}

// Zet een bepaalde LED in de matrix uit
// x vanaf links
// y vanaf boven
void clearPixel(byte x, byte y)
{
  frameBuffer[y] &= ~(1 << x);
}

/////////////////////////////////////////////////////////////
// Bitmap manipulatie

// Kopieer een bitmap. Zorg dat de destination 
void copyBitmap(byte *source, byte *dest)
{
  for (int iRow = 0; iRow < NROWS; iRow++) dest[iRow] = source[iRow];
}

// Shift alle bytes in een bitmap (mag ook frameBuffer zijn)
// als nDigits negatief is, schuif naar links
// anders naar rechts. De bitmap wordt permanent gewijzigd!
void shiftBitmap(byte *bitmap, char nDigits)
{
  for (byte iRow=0; iRow < NROWS; iRow++)
  {
    if (nDigits < 0) bitmap[iRow] <<= -nDigits; // Clear the sign bit
    else             bitmap[iRow] >>=  nDigits;		
  }
}

void scroll(char *text, float charsPerSecond)
{
  byte temp[NROWS];
  int duration = charsPerSecond == 0 ? 0 : 1000 / (NCOLS * charsPerSecond);
  int index = 0;
  while (text[index] != 0)
  {
    for (int offset = NCOLS - 1; offset >= 0; offset--)
    {
      shiftBitmap(frameBuffer, -1);          // Shift existing content out
      copyBitmap(bitmapFont[text[index] - 32], temp); // Make a copy of the new character
      shiftBitmap(temp, offset);             // Shift the new character away
      ORPutBitmap(temp);                     // Combine the shifted character with old content
      drawFrameBuffer();
      delay(duration);
    }
    index++;
  }
}

/////////////////////////////////////////////////////////////
// Arduino setup en loop

void setup ()
{  
  pinMode(DIN, OUTPUT);
  pinMode(CLK,  OUTPUT);
  pinMode(CS,   OUTPUT);
  initMax7219();
}

void loop ()
{
  // Part 1: characters /////////////////////////////////
  // Show "Demo "
  //byte demo[] = {36, 69, 77, 79, 0};
  //byte demo[] = {68, 101, 109, 111, 32}; // ASCII array
  //char demo[] = {'D', 'e', 'm', 'o', 0}; // char array (is ook ASCII)
  /*char *demo = "Demo";                   // C-style string (is ook een ASCII array)
  for (int i = 0; i < 5; i++)
  {
    putBitmap(bitmapFont[demo[i] - 32]);
    drawFrameBuffer();
    delay(300);
  }*/
  
  // Part 2: font display /////////////////////////////////
  /*
  for (int i = 0; i < 95; i++)
  {
    putBitmap(bitmapFont[i]);
    drawFrameBuffer();
    delay(300);
  }
  */
  
  // Part 3: scrolling /////////////////////////////////
  scroll("Geoffrey ", 1);
}

