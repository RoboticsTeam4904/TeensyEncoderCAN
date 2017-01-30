#include <Encoder.h>
#include <TeensyCANBase.h>
#include <FlexCAN.h>

Encoder encoder(5, 6);
int buttonPin = 9;

void setup(void){
  CAN_begin();
  pinMode(buttonPin, INPUT);
  delay(1000);
  Serial.println("Teensy 3.X CAN Encoder");
}

long lastRead = 0;
long pos = -999;
long rate = 0;

void loop(void){
  CAN_update();
  
  uint8_t * data = (uint8_t *) malloc(8);
  if(data[0] == 0x72 && data[1] == 0x65 && data[2] == 0x73 && data[3] == 0x65 && data[4] == 0x74 && data[5] == 0x65 && data[6] == 0x6e && data[7] == 0x63){
    encoder.write(0);
    pos = 0;
    rate = 0;
    Serial.println("reset");
  }else if(data[0] == 0 || data[0] == 1) {
    data[0] = pos & 0xff;
    data[1] = (pos >> 8) & 0xff;
    data[2] = (pos >> 16) & 0xff;
    data[3] = (pos >> 24) & 0xff;
    Serial.print(pos);

    data[4] = rate & 0xff;
    data[5] = (rate >> 8) & 0xff;
    data[6] = (rate >> 16) & 0xff;
    data[7] = (rate >> 24) & 0xff;
    Serial.println(rate);
  
  CAN_write(0x601, data);
  }
  delete data;

  if(digitalRead(buttonPin) == HIGH){
    encoder.write(0);
  }

  long newPos = encoder.read();
  if(newPos != pos){
    rate = ((double) 1000000.0 * (newPos - pos)) / ((double) (micros() - lastRead));
    pos = newPos;
    //Serial.println(pos);
    lastRead = micros();
  }
  else{
    if((micros() - lastRead) > 1000){
      rate = 0;
    }
  }

}

