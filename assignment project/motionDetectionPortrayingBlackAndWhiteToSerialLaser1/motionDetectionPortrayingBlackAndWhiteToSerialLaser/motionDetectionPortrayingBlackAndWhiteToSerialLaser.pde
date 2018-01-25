import de.humatic.dsj.*; //<>// //<>//
import java.awt.image.BufferedImage;
import java.nio.ByteBuffer;
import java.util.concurrent.TimeUnit;
import java.util.Arrays;
import processing.serial.*;
import processing.net.*;

// Initialize webcam
DCapture cap;
CaptureDeviceControls controls;
Serial myPort;

//server settings
final int getLaserStatus = 0;
final int setLaserStatusOn = 1;
final int setLaserStatusOff = 2;
final int getLaserAngle = 3; // returns range between 60-130
final int getCalibrationMinimum = 4;
final int getCalibrationMaximum = 5;
final int resetLaserAngle = 90;
final int bad = 255;
final int ack = 254;

int port = 10002;
boolean myServerRunning = true;

Server myServer;

//settings
public int treshhold = 90;
public int maximumAngle = 140;
public int minimumAngle = 50;

boolean manual = false;

int mapInt(int x, int in_min, int in_max, int out_min, int out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

class DCapture implements java.beans.PropertyChangeListener {
  
  private DSCapture capture;
  public int width, height;
  private byte [][] firstFramePixels;

  DCapture() {
    DSFilterInfo[][] dsi = DSCapture.queryDevices();
    capture = new DSCapture(DSFiltergraph.DD7, dsi[0][0], false, 
      DSFilterInfo.doNotRender(), this);
    width = capture.getDisplaySize().width; //<>//
    height = capture.getDisplaySize().height;
    firstFramePixels = new byte[width*height][4];
    System.out.println("width: " + width);
  }

  public PImage updateImage() {
    PImage img = createImage(width, height, RGB);
    BufferedImage bimg = capture.getImage();
    bimg.getRGB(0, 0, img.width, img.height, img.pixels, 0, img.width);
    img.updatePixels();

    int currPixel;
    int totalDiff;
    int cRed;
    int cGreen;
    int cBlue;
    
    //keep track of amount of motion in each pixel column in a frame
    int[] amountOfMotionPerPixelColumn = new int[width];
    int indexOfMostMotion = 0;

    //loop all pixels
    for (int iPixel=0; iPixel<img.pixels.length; iPixel++)
    {   
      currPixel = img.pixels[iPixel]; // fetch a pixel from the array      

      cRed = (((currPixel >> 16) & 0x000000FF) - (firstFramePixels[iPixel][1] & 0x000000FF)); // extract the red component 
      cGreen = (((currPixel >> 8) & 0x000000FF) - (firstFramePixels[iPixel][2] & 0x000000FF)); // extract the red component 
      cBlue = ((currPixel & 0x000000FF) - (firstFramePixels[iPixel][3] & 0x000000FF)); // extract the red component 

      if (cRed < 0) {
        cRed = 0 - cRed;
      }
      if (cGreen < 0) {
        cGreen = 0 - cGreen;
      }      
      if (cBlue < 0) {
        cBlue = 0 - cBlue;
      }

      //calculate total difference pixel difference compatred to calibrated frame
      totalDiff = cRed + cGreen + cBlue;     

      // make pixels black if no motion detected(inside threshold) otherwise white
      img.pixels[iPixel] = color(0, 0, 0);
      if (totalDiff > THRESHOLD) {          
        img.pixels[iPixel] = color(255, 255, 255);
        //update the value of the correspondig column with an addition of one if theres motion
        amountOfMotionPerPixelColumn[iPixel - width * (iPixel/width)]++;
      }
    }
    
    //evaluate which column had most motion
    for (int index=0; index<amountOfMotionPerPixelColumn.length; index++){
      if(amountOfMotionPerPixelColumn[index] > amountOfMotionPerPixelColumn[indexOfMostMotion]){
        indexOfMostMotion = index; //<>//
      }
    }
    
    sendMotionColumnToSerial(indexOfMostMotion);
    
    //paint column with most motion red
    for (int iPixel=indexOfMostMotion; iPixel<img.pixels.length; iPixel += width)
    {   
      currPixel = img.pixels[iPixel]; // fetch a pixel from the array 
      img.pixels[iPixel] = color(255, 0, 0);
    }
    
    //print column number
    //System.out.println("most motion: " + amountOfMotionPerPixelColumn[indexOfMostMotion] + "index: " + indexOfMostMotion);
    
    img.updatePixels();
    return img;
  }

  public void calibrate() { 
    PImage img = createImage(width, height, RGB);
    BufferedImage bimg = capture.getImage();
    bimg.getRGB(0, 0, img.width, img.height, img.pixels, 0, img.width);
    img.updatePixels(); // bit tricky but needed because img.pixels is being updated in the above call
    
    for (int iPixel=0; iPixel<img.pixels.length; iPixel++) {
      ByteBuffer b = ByteBuffer.allocate(4);
      b.putInt(img.pixels[iPixel]); // conversion from pixel integer to 4 bytes, this is so it has to be done ones instead of each frame
      firstFramePixels[iPixel] = b.array(); // our calibrated frame
    }
    ////debugging
    //for (int iPixel2 = 0; iPixel2 < firstFramePixels.length; iPixel2++){
    //  System.out.println(" pixel "+ iPixel2 + " waarde: " + ByteBuffer.wrap(firstFramePixels[iPixel2]).getInt());
    //}
  }
  
  
  
  public void sendMotionColumnToSerial(int index){
    byte angle = (byte) mapInt(index, 0, width, maximumAngle, minimumAngle);
    println("motion detected index: " + index + " outcoming angle" + angle);
    myPort.write(angle);  
  }

  public void propertyChange(java.beans.PropertyChangeEvent e) {
    switch (DSJUtils.getEventType(e)) {
    }
  }

  public CaptureDeviceControls getControls() {
    return capture.getActiveVideoDevice().getControls();
  }
}


void settings()
{
  cap = new DCapture();
  size(cap.width, cap.height); // this needs to be in settings method because size has to be the first call in setup()
}

void setup()
{
  background(0);  
  controls = cap.getControls();
  String portName = "COM3";
  myPort = new Serial(this, portName, 9600);
  try{
    TimeUnit.SECONDS.sleep(3);
  }catch(Exception e){}
  cap.calibrate();
  
  //turn on laser
  myPort.write(254);
  
  myServer = new Server(this, port); // Starts a myServer on port 10002
  
  //controls.setKSProperty(CaptureDeviceControls.KS_CAMCTRL, CaptureDeviceControls.KS_CAMCTRL_FOCUS, new int[0]);
}  

void draw()
{  
  if(!manual){
    image(cap.updateImage(), 0, 0, cap.width, cap.height);
  }
  //try{
  //  TimeUnit.MILLISECONDS.sleep(200);
  //}catch(Exception e){}
  if (myServerRunning == true)
  {
    Client thisClient = myServer.available();
    if (thisClient != null) {
      if (thisClient.available() > 0) {
        int waarde = thisClient.read();
        println("mesage from: " + thisClient.ip() + " : " + waarde);
        if(waarde == getCalibrationMinimum){
          thisClient.write(minimumAngle);       
        }else if(waarde == getCalibrationMaximum){
          thisClient.write(maximumAngle);       
        }else if(waarde >= minimumAngle && waarde <= maximumAngle){
          //serial write waarde naar laser 
          myPort.write(waarde);  
          manual = true;
          thisClient.write(ack);       
        }else{
          thisClient.write(bad);
        }
      }
    }
  } 
  else 
  {
    println("Server stoppped");
  }
}

void exit() {
  //turn off laser
  myPort.write(255);
} 