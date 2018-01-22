import processing.serial.*;

Serial port;
int serialIn = 0;
int []inputBuffer = new int[400];
int maxSamples = 1600; // maximum amount of samples on the screen
int bytePointer = 0;
int horMargin = 50;
int nChannels = 1;
float rawValue = 0;
float EMAValue = 0; // EMA stands for Exponential Moving Average

// Moving average filter setting
float filterValue = 0.1;

// store all converted values in ArrayLists
ArrayList<Float> rawSignalX = new ArrayList<Float>();
ArrayList<Float> EMASignalX = new ArrayList<Float>();
ArrayList<Float> diffSignalX = new ArrayList<Float>();

void setup()
{
  size(800, 600);
  port = new Serial(this, "/dev/ttyUSB0", 115200);
}

// Draw a graph of a signal
void drawSignal(ArrayList<Float> signal)
{
    for (int iSample=0; iSample<signal.size()-1; iSample++)
    line(
      map(iSample, 0, maxSamples, 0, width), 
      map(signal.get(iSample), 256, 768, horMargin, height-horMargin), 
      map(iSample+1, 0, maxSamples, 0, width), 
      map(signal.get(iSample+1), 256, 768, horMargin, height-horMargin));
}

void draw()
{
  background(0);
  stroke(255);
  
  // Read bytes from the serial port
  while (port.available () > 0)
  {
    serialIn = port.read();
    // if a complete packet is received
    if ((serialIn == 69) && (bytePointer > (nChannels * 2)-1))
    {
      rawValue = inputBuffer[0] + inputBuffer[1]*256;    
      EMAValue = filterValue * rawValue + (1.0 - filterValue) * EMAValue;
      EMASignalX.add(EMAValue);
      rawSignalX.add(rawValue);
      diffSignalX.add(EMAValue - rawValue);
          
      if (rawSignalX.size() > maxSamples)
      {
        rawSignalX.remove(0);     
        EMASignalX.remove(0);
      }

      bytePointer = 0;
    }
    else
      inputBuffer[bytePointer++] = serialIn;
  }

  stroke(255,255,255);
  drawSignal(rawSignalX);
  stroke(255,0, 0);
  drawSignal(EMASignalX); 
  stroke(0,255, 0);
  drawSignal(diffSignalX); 
}

void keyPressed() {
  if (key == 'z') {
    filterValue += 0.1;
  } else {
    filterValue -= 0.1;
  }
}