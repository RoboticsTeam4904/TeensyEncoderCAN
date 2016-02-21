#include <Encoder.h>
#include <TeensyCANBase.h>

Encoder encoder(5, 6);
TeensyCANBase can(0x602);
int buttonPin = 9;

void setup(void){
  can.begin();
  pinMode(buttonPin, INPUT);
  delay(1000);
  Serial.println("Teensy 3.X CAN Encoder");
}

long lastRead = 0;
long pos = -999;
long rate = 0;

void loop(void){
  if(can.available() > 0){
    uint8_t * data = (uint8_t *) malloc(8);
    if(can.read(data) == 0) {
      Serial.print(data[0]);
      Serial.print("\t");
      if(data[0] == 0){
        data[0] = pos & 0xff;
        data[1] = (pos >> 8) & 0xff;
        data[2] = (pos >> 16) & 0xff;
        data[3] = (pos >> 24) & 0xff;
        Serial.print(pos);
        Serial.print("\t");
        Serial.println(data[2]);

        data[4] = 0; // Mode

        for(int i = 5; i < 8; i++){
          data[i] = 0;
        }
      }
      else if(data[0] == 1){
        data[0] = rate & 0xff;
        data[1] = (rate >> 8) & 0xff;
        data[2] = (rate >> 16) & 0xff;
        data[3] = (rate >> 24) & 0xff;
        Serial.println(rate);
        
        data[4] = 1; // Mode
        
        for(int i = 5; i < 8; i++){
          data[i] = 0;
        }
      }
      else if(data[0] == 0x72 && data[1] == 0x65 && data[2] == 0x73 && data[3] == 0x65 && data[4] == 0x74 && data[5] == 0x65 && data[6] == 0x6e && data[7] == 0x63){
        encoder.write(0);
        pos = 0;
        rate = 0;
        Serial.println("reset");
      }
      can.write(data);
    }
    delete data;
  }

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

