import processing.serial.*;

// Find out to which port your Arduino is connected
// and replace this number
int portNr = 32;
Serial port = new Serial(this, Serial.list()[portNr], 9600);
int tempHigh = 0;
int tempLow = 0;

void setup()
{
  size(128, 128);
  textSize(32);
  println(Serial.list());
}

void draw()
{
  background(0);
  fill(#002266);
  fill(#FFFFFF);
  text(mouseX + "\n" + char(mouseX), 10, 10);
  if (pmouseX != mouseX)
  {
    port.write(255);  
    port.write(mouseX);
    port.write(mouseY);
  }
}