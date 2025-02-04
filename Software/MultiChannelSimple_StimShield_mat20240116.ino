#include <SPI.h>

byte valin[9] = {0};  //amp1 (0mA),  pulsewidth1 (us/5),... freq (0Hz), interleaved (true/false),
//byte valout[9] = {1,2,3,4,5,6,7,8,9};  //amp1 (0mA),  pulsewidth1 (us/5),... freq (0Hz), interleaved (true/false),
byte timearr[4] = { 0, 0, 0, 0 };

byte val = 0;


// Channel constants
const int slaveSelectPin1 = 8;
const int cathPin1 = 2;
const int anodPin1 = 5;

const int slaveSelectPin2 = 9;
const int cathPin2 = 3;
const int anodPin2 = 6;

const int slaveSelectPin3 = 10;
const int cathPin3 = 4;
const int anodPin3 = 7;

// Stim Parameters
unsigned int interleaved = int(valin[7]);
unsigned int channels = 0;
unsigned int freq = int(valin[6]);
unsigned long period = 1000000 / freq;

unsigned int amp1 = int(valin[0]);  //0, 1, 2, ..., 255 (1mA = 16.25)
unsigned int pulsewidth1 = int(valin[2]);

unsigned int amp2 = int(valin[0]);  //0, 1, 2, ..., 255 (1mA = 16.25)
unsigned int pulsewidth2 = int(valin[2]);

unsigned int amp3 = int(valin[0]);  //0, 1, 2, ..., 255 (1mA = 16.25)
unsigned int pulsewidth3 = int(valin[2]);

unsigned long time_diff = 0;
unsigned long time_curr = 0;

unsigned long timetot = 0;

unsigned long looptime = micros();







void setup() {

  Serial.begin(115200);
  SPI.begin();



  pinMode(slaveSelectPin1, OUTPUT);
  pinMode(cathPin1, OUTPUT);
  pinMode(anodPin1, OUTPUT);

  pinMode(slaveSelectPin2, OUTPUT);
  pinMode(cathPin2, OUTPUT);
  pinMode(anodPin2, OUTPUT);

  pinMode(slaveSelectPin3, OUTPUT);
  pinMode(cathPin3, OUTPUT);
  pinMode(anodPin3, OUTPUT);

  digitalPotWrite(1, 0, amp1);
  digitalPotWrite(1, 1, amp1);
  digitalPotWrite(2, 0, amp2);
  digitalPotWrite(2, 1, amp2);
  digitalPotWrite(3, 0, amp3);
  digitalPotWrite(3, 1, amp3);

  delay(300);
  serialFlush();
}


void loop() {
  timetot = millis();


  if (Serial.available() > 7) {

    //read in data
    Serial.readBytes(valin, 8);

    // send time out
    timearr[0] = timetot & 0xFF; // 0x78
    timearr[1] = (timetot >> 8) & 0xFF; // 0x56
    timearr[2] = (timetot >> 16) & 0xFF; // 0x34
    timearr[3] = (timetot >> 24) & 0xFF; // 0x12
    Serial.write(timearr, 4);
    // send debugging info
    //Serial.write(valout, 8);
    Serial.write(valin,8);
    serialFlush();
    

    amp1 = int(valin[0]);  //0, 1, 2, ..., 255 (1mA = 16.25)
    pulsewidth1 = int(valin[1]) * 5;
    digitalPotWrite(1, 0, amp1);  //set cathode amp by adjusting digital pot value
    digitalPotWrite(1, 1, amp1);  //set anode amp


    amp2 = int(valin[2]);  //0, 1, 2, ..., 255 (1mA = 16.25)
    pulsewidth2 = int(valin[3]) * 5;
    digitalPotWrite(2, 0, amp2);  //set cathode amp by adjusting digital pot value
    digitalPotWrite(2, 1, amp2);  //set anode amp


    amp3 = int(valin[4]);  //0, 1, 2, ..., 255 (1mA = 16.25)
    pulsewidth3 = int(valin[5]) * 5;
    digitalPotWrite(3, 0, amp3);  //set cathode amp by adjusting digital pot value
    digitalPotWrite(3, 1, amp3);  //set anode amp

    freq = int(valin[6]);
    period = 1000000 / freq;
    interleaved = int(valin[7]);


    //    Serial.write(48);

    // Select Channels
    //1->Channel 1, 2->channel 2, 3->channel 3, 4->channel 1&2, 5->channel 1&3, 6->channel 2&3, 7->channel 1&2&3
    if (amp1 > 0) {
      if (amp2 > 0) {
        if (amp3 > 0) {
          channels = 7;  //1,2,3
        } else {
          channels = 4;  //1,2
        }
      } else {
        if (amp3 > 0) {
          channels = 5;  //1,3
        } else {
          channels = 1;  //1
        }
      }
    } else if (amp2 > 0) {
      if (amp3 > 0) {
        channels = 6;  //2,3
      } else {
        channels = 2;  //2
      }
    } else if (amp3 > 0) {
      channels = 3;  //3
    } else {
      channels = 0;  // No channels on
    }


  }  //Serial comm

  time_diff = micros() - time_curr;
  if (freq > 0 && channels != 0) {
    if (time_diff > period) {
      time_curr = micros();
      sendInterleavedPulse(channels);
    }
  }  //freq
}  //loop


void digitalPotWrite(int SS, int address, int value) {
  if (SS == 1) {
    // take the SS pin low to select the chip:
    digitalWrite(slaveSelectPin1, LOW);
    //  send in the address and value via SPI:
    SPI.transfer(address);
    SPI.transfer(value);
    // take the SS pin high to de-select the chip:
    digitalWrite(slaveSelectPin1, HIGH);
  } else if (SS == 2) {
    // take the SS pin low to select the chip:
    digitalWrite(slaveSelectPin2, LOW);
    //  send in the address and value via SPI:
    SPI.transfer(address);
    SPI.transfer(value);
    // take the SS pin high to de-select the chip:
    digitalWrite(slaveSelectPin2, HIGH);
  } else if (SS == 3) {
    // take the SS pin low to select the chip:
    digitalWrite(slaveSelectPin3, LOW);
    //  send in the address and value via SPI:
    SPI.transfer(address);
    SPI.transfer(value);
    // take the SS pin high to de-select the chip:
    digitalWrite(slaveSelectPin3, HIGH);
  }
}


void sendInterleavedPulse(int channels) {
  //1->Channel 1, 2->channel 2, 3->channel 3, 4->channel 1&2, 5->channel 1&3, 6->channel 2&3, 7->channel 1&2&3
  switch (channels) {
    case 1:
      digitalWrite(cathPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(cathPin1, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(anodPin1, LOW);
      break;

    case 2:
      digitalWrite(cathPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(cathPin2, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(anodPin2, LOW);
      break;

    case 3:
      digitalWrite(cathPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(cathPin3, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(anodPin3, LOW);
      break;

    case 4:
      digitalWrite(cathPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(cathPin1, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(anodPin1, LOW);

      delayMicroseconds(period / 2 - pulsewidth1 * 2 - 50);

      digitalWrite(cathPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(cathPin2, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(anodPin2, LOW);
      break;

    case 5:
      digitalWrite(cathPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(cathPin1, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(anodPin1, LOW);

      delayMicroseconds(period / 2 - pulsewidth1 * 2 - 50);

      digitalWrite(cathPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(cathPin3, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(anodPin3, LOW);
      break;

    case 6:
      digitalWrite(cathPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(cathPin2, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(anodPin2, LOW);

      delayMicroseconds(period / 2 - pulsewidth2 * 2 - 50);

      digitalWrite(cathPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(cathPin3, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(anodPin3, LOW);
      break;

    case 7:
      digitalWrite(cathPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(cathPin1, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin1, HIGH);
      delayMicroseconds(pulsewidth1);
      digitalWrite(anodPin1, LOW);

      delayMicroseconds(period / 3 - pulsewidth1 * 2 - 50);

      digitalWrite(cathPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(cathPin2, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin2, HIGH);
      delayMicroseconds(pulsewidth2);
      digitalWrite(anodPin2, LOW);

      delayMicroseconds(period / 3 - pulsewidth2 * 2 - 50);

      digitalWrite(cathPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(cathPin3, LOW);
      delayMicroseconds(50);
      digitalWrite(anodPin3, HIGH);
      delayMicroseconds(pulsewidth3);
      digitalWrite(anodPin3, LOW);
      break;
  }
}

void serialFlush(){
  while(Serial.available() > 0) {
    char t = Serial.read();
  }
}
