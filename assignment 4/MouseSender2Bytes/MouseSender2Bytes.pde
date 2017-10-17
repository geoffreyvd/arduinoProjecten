import processing.serial.*;

// Find out to which port your Arduino is connected
// and replace this number
int portNr = 32;
Serial port = new Serial(this, Serial.list()[portNr], 9600);
int tempHigh = 0;
int tempLow = 0;

void setup()
{
  size(500, 255);
  textSize(32);
  println(Serial.list());
}

void draw()
{
  background(0);
  fill(#002266);
  rect(32, 0, 95, height);
  fill(#FFFFFF);
  text(mouseX + "\n" + char(mouseX), 130, 100);
  if (pmouseX != mouseX)
  {
    tempHigh = mouseX / 255;
    tempLow = mouseX % 255;
    port.write(255);  
    port.write(tempHigh);
    port.write(tempLow);
  }
}