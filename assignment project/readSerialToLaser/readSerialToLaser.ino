#include <Servo.h>  //add '<' and '>' before and after servo.h
 
int servoPin = 9;
int petentioPin = A0;
 
Servo servo;  
 
int servoAngle = 0;   // servo position in degrees
int petentioValue = 0;
int incomingByte = 0;
 
void setup()
{
  Serial.begin(9600);  
  servo.attach(servoPin);
  
  pinMode(8, OUTPUT);     
  digitalWrite(8, LOW);   // turn the LED on (HIGH is the voltage level)
}
 
 
void loop()
{
//control the servo's direction                
  //petentioValue = analogRead(petentioPin);
  //check for value change first
  //servo.write(map(petentioValue, 0, 1023, 0, 180));
  
  //petentioValue++;
  //servo.write(petentioValue);
  //delay(17);
  //if(petentioValue > 180){
  //  petentioValue = 0;
  //}

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    if(incomingByte > 30 && incomingByte < 160){
      servo.write(incomingByte); 
    }else if(incomingByte == 255){ 
      digitalWrite(8, LOW);  
    }else if(incomingByte == 254){ 
      digitalWrite(8, HIGH);  
    }
    
  }
//   servo.write(0);      // Turn SG90 servo Left to 45 degrees
//   delay(600);          // Wait 1 second
//   servo.write(180);      // Turn SG90 servo back to 90 degrees (center position)
//   delay(600);          // Wait 1 second
//   servo.write(135);     // Turn SG90 servo Right to 135 degrees
//   delay(1000);          // Wait 1 second
//   servo.write(90);      // Turn SG90 servo back to 90 degrees (center position)
//   delay(1000);

//end control the servo's direction and the position of the motor


//control the servo's speed  

//if you change the delay value (from example change 50 to 10), the speed of the servo changes
//  for(servoAngle = 0; servoAngle < 180; servoAngle++)  //move the micro servo from 0 degrees to 180 degrees
//  {                                  
//    servo.write(servoAngle);              
//    delay(50);                  
//  }
//
//  for(servoAngle = 180; servoAngle > 0; servoAngle--)  //now move back the micro servo from 0 degrees to 180 degrees
//  {                                
//    servo.write(servoAngle);          
//    delay(10);      
//  }
  //end control the servo's speed  
}
