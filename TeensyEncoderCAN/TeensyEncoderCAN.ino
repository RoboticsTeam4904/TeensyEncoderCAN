#include <Encoder.h>
#include <FlexCAN.h>
#include <TeensyCANBase.h>

Encoder encoder(5, 6);
TeensyCANBase encoderCAN(0x610, &sendEncoder);

void setup(void){
	TeensyCANBase::begin();
	delay(1000);
	Serial.println("Teensy 3.X CAN Encoder");
}

long lastRead = 0;
long pos = -999;
long rate = 0;

int sendEncoder(byte* msg, byte* resp) {\
	if (msg[0] == 0) {
		resp[0] = pos & 0xff;
		resp[1] = (pos >> 8) & 0xff;
		resp[2] = (pos >> 16) & 0xff;
		resp[3] = (pos >> 24) & 0xff;\

		resp[4] = 0; // Mode

		for (int i = 5; i < 8; i++) {
			resp[i] = 0;
		}

		return 0;
	}
	else if (msg[0] == 1) {
		resp[0] = rate & 0xff;
		resp[1] = (rate >> 8) & 0xff;
		resp[2] = (rate >> 16) & 0xff;
		resp[3] = (rate >> 24) & 0xff;\

		resp[4] = 1; // Mode

		for (int i = 5; i < 8; i++) {
			resp[i] = 0;
		}

		return 0;
	}
	else if (msg[0] == 0x72 && msg[1] == 0x65 && msg[2] == 0x73 && msg[3] == 0x65 && msg[4] == 0x74 && msg[5] == 0x65 && msg[6] == 0x6e && msg[7] == 0x63) {
		encoder.write(0);
		pos = 0;
		rate = 0;
		Serial.println("reset");
		return 1;
	}
	return 1;
}

void loop(void){
	TeensyCANBase::update();

	long newPos = encoder.read();
	if(newPos != pos){
		rate = ((double) 1000000.0 * (newPos - pos)) / ((double) (micros() - lastRead));
		pos = newPos;
		lastRead = micros();
	}
	else{
		if((micros() - lastRead) > 1000){
			rate = 0;
		}
	}

}
