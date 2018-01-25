/**
 * Chat Server 
 * by Tom Igoe. 
 * 
 * Press the mouse to stop the server.
 */
 

import processing.net.*;

int port = 10002;
boolean myServerRunning = true;
int bgColor = 0;
int direction = 1;
int textLine = 60;


final int getLaserStatus = 0;
final int setLaserStatusOn = 1;
final int setLaserStatusOff = 2;
final int getLaserAngle = 3; // returns range between 60-130
final int getCalibrationMinimum = 4;
final int getCalibrationMaximum = 5;
final int resetLaserAngle = 90;
final int bad = 255;
final int ack = 254;

public int maximumAngle = 140;
public int minimumAngle = 50;

Server myServer;

void setup()
{
  size(400, 400);
  textFont(createFont("SanSerif", 16));
  myServer = new Server(this, port); // Starts a myServer on port 10002
  background(0);
}

void mousePressed()
{
  // If the mouse clicked the myServer stops
  myServer.stop();
  myServerRunning = false;
}

void draw()
{
  if (myServerRunning == true)
  {
    text("server", 15, 45);
    Client thisClient = myServer.available();
    if (thisClient != null) {
      if (thisClient.available() > 0) {
        int waarde = thisClient.read();
        text("mesage from: " + thisClient.ip() + " : " + waarde, 15, textLine);
        if(waarde == getCalibrationMinimum){
          thisClient.write(minimumAngle);       
        }else if(waarde == getCalibrationMaximum){
          thisClient.write(maximumAngle);       
        }else if(waarde >= minimumAngle && waarde <= maximumAngle){
          //serial write waarde naar laser 
          thisClient.write(ack);       
        }else{
          thisClient.write(bad);
        }
          
        textLine = textLine + 35;
      }
    }
  } 
  else 
  {
    text("server", 15, 45);
    text("stopped", 15, 65);
  }
}