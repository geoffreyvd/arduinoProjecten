// This is not a complete program, but you can use it as a base or
// copy parts of it to your own program.

import processing.net.*;
import processing.serial.*;

// You can store fixed messages like this
byte[] reqKeepAlive = {1, 2, 5, 0, 23};
byte[] ackKeepAlive = {1, 2, 6, 0, 23};

Client client;
Serial myPort; 

// We might not receive a whole message at a time. If we only get half of it then
// msgPointer remembers how far we got. If it is -1 it means we haven't started yet
int msgPointer = -1;
byte petentiometer = 0;

// This is the buffer that will store the message
byte[] msg;

void setup()
{
  client = new Client(this, "127.0.0.1", 6543);
  String portName = "/dev/ttyUSB0";
  myPort = new Serial(this, portName, 9600);
}

void draw()
{
  //client = new Client(this, "127.0.0.1", 6543);  dit crasht hem
  // Do stuff here....
  // Send stuff to the server...
  
  // We will receive much more than one byte per draw() update! Therefore
  // it is important that we read ALL the bytes that are available,
  // so use this while loop to process them all until there are no more.
  while(client != null && client.available() > 0)
  {
    int data = client.read();
    if (msgPointer < 0)     // msgPointer = -1 means we are expecting a new message
    {
      if (data == 1)        // The protocol says that all messages must start with a 1, so we only act if we receive a 1!
      {
        msgPointer = 0;     // By making msgPoint not -1 anymore we know that we have started to receive a message
      }
    }
    else if (msg == null)   // We received the start byte but we don't know the message length yet...
    {
      msg = new byte[data]; // ...so this new byte must be the message length!
    }
    else if (msgPointer == msg.length) // How do we know the message is complete? Because the message said how long it would be!
    {
      if (data == 23)      // The message should be complete now, so the new byte should be the end byte.
      {
        processMsg(msg);   // Only process this message if we actually received the end byte.
      }
      msg = null;          // After we processed the message or if we decided it was not valid we start over. Throw
      msgPointer = -1;     // away the old message and make msgPoint = -1 again to signal that we expect a new message
    }
    else                   // This is not the beginning, we already know the message length and it is not the end eiterh...
    {
      msg[msgPointer++] = (byte)data; // So we are in the middle of reading a message. Just store the new byte.
    }
  }
  
  if ( myPort.available() == 3) {  // If data is available,
    if (myPort.read() == 'S'){
      petentiometer = (byte) myPort.read();
      if(myPort.read() == 'E'){
        sendPaddle(petentiometer);
        System.out.println("sending petentiometer value: " + petentiometer);
      }
    }
  }
  rect(petentiometer, 50, 100, 25);
}

void sendPaddle(byte X)
{
  byte[] sendPaddle = {1, 3, 6, 73, X, 23};
  client.write(sendPaddle);
}

void processMsg(byte[] msg)
{
  if (msg[0] == 5) // Request
  {
    if(msg[1] == 0){
      client.write(ackKeepAlive);
      System.out.println("sending ack keep alive");
    // Stuff
    }
  }
  else if (msg[0] == 6) // Information
  {
    // More stuff
  }
  else if (msg[0] == 21) // Error
  {
    // Even more stuff
  }
}