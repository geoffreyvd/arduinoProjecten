#define SAMPLE_INTERVAL 0
uint16_t analogVal = 0;
uint16_t millisElapsed = 0,
  oldMillis = 0;

void setup() {
  Serial.begin(115200);
}

void loop()
{  
  millisElapsed += millis() - oldMillis;
  
  if (millisElapsed >= SAMPLE_INTERVAL)
  {
    analogVal = analogRead(0);
    Serial.write((byte *) &analogVal, sizeof(analogVal));    
    Serial.write('E');
    millisElapsed = 0;
  }
  
  oldMillis = millis();
}
