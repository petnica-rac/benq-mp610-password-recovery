#include <Arduino.h>
#include <IRremote.h>

#define IR_SEND_PIN 12
#define RECEIVER_PIN 2 


#define PASS_START 5000
#define PASS_END 6000


IRrecv receiver(RECEIVER_PIN); 
decode_results results;

void setup() {
    pinMode(LED_BUILTIN, OUTPUT);
    Serial.begin(9600);
    //Serial.println(F("START " _FILE_ " from " _DATE_ "\r\n"));
    IrSender.begin(IR_SEND_PIN, DISABLE_LED_FEEDBACK);
    receiver.enableIRIn();
}

uint32_t commandPower = 0x40BF;   // power on/off
uint32_t commandArrowUp = 0xD02F; // number increment and fail atempt exit
uint32_t commandArrowDown = 0x30CF; // number decrement
uint32_t commandArrowRight = 0x708F; // next digit (to lower)
uint32_t commandArrowLeft = 0xB04F; // last digit (to lower)
uint32_t commandAuto = 0x10EF; // next digit (to lower)


unsigned int revBits(unsigned int data){
    unsigned char totalBits = sizeof(data) * 8;
    unsigned int reverse = 0, i, temp;

    for (i = 0; i < totalBits; i++) {
        temp = (data & (1 << i));
        if (temp)
            reverse |= (1 << ((totalBits - 1) - i));
    }

    return reverse;
}

void sender(uint32_t command){
    IrSender.sendNEC(revBits(0x000c), revBits(command), 0);
    delay(50);
}


void send_digit(int number) {
  if (number == 0)
  {
    sender(commandArrowUp);
    sender(commandArrowDown);
  }
  else if (number < 5) {
    for (int i = 0; i<number; i++) {
      sender(commandArrowUp);
      sender(commandArrowRight);
      sender(commandArrowLeft);
    }
  }
  else {
    for (int i = 0; i<10-number; i++) {
      sender(commandArrowDown);
      sender(commandArrowRight);
      sender(commandArrowLeft);
    }
  }
  sender(commandArrowRight);
}

void sendPassword(int password) {
  int c = 1000;
  for (int i = 0; i<4; i++) {
    int digit = password / c;
    password %= c;
    c /= 10;
    send_digit(digit);
  }
  sender(commandAuto);
  delay(25);
  sender(commandArrowUp);
  delay(25);
}

void loop() {
  for (int i = PASS_START; i<PASS_END; i++) {
    sendPassword(i);
    Serial.println(i);
  }  
  /*if (receiver.decode(&results)) { // decode the received signal and store it in results
    Serial.println(results.value, HEX); // print the values in the Serial Monitor
    receiver.resume(); // reset the receiver for the next code
  }*/
}
