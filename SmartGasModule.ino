#include "gasModule.h"

RunningAverage compressor(MOVING_AVG_SIZE); //pt403
RunningAverage trailer1(MOVING_AVG_SIZE); //pt401
RunningAverage trailer2(MOVING_AVG_SIZE); //pt414
RunningAverage dispenser(MOVING_AVG_SIZE); //pt408

SmallMatrix smallMatrix[3] = { SmallMatrix(0x70), SmallMatrix(0x71), SmallMatrix(0x72) };
LargeMatrix bigMatrix[3] = { LargeMatrix(0x73), LargeMatrix(0x74), LargeMatrix(0x75) };
Adafruit_LiquidCrystal lcd(0);


void (*resetFunc)(void) = 0;  // Allows hard reset of arduino through software

//Slave ID's are: 1-50 Carriers, 51-100 dispensers
void setup() {
  Wire.begin();
  Wire.setWireTimeout(10000, true);
  pinModeSetup();
  Serial.begin(9600);
  Serial1.begin(9600, SERIAL_8N1);
  Serial3.begin(9600, SERIAL_8N1);

  mbRTU.begin(1, Serial1);
  mbRTU.preTransmission(preTransmission);
  mbRTU.postTransmission(postTransmission);
  delay(500);  //wait for system to boot up

  Wire.beginTransmission(0x48);
  Wire.endTransmission();

  if (!mcp2.begin_I2C(0x27)) {
    Serial.println("mcp2 Error.");
    delay(100);
    resetFunc();
  }
  if (!mcp1.begin_I2C(0x20)) {
    Serial.println("mcp1 Error.");
    delay(100);
     resetFunc();
  }
for(int i = 0;i < 16;i++){
  mcp1.pinMode(i,INPUT_PULLUP);
  mcp2.pinMode(i,OUTPUT);
  mcp2.digitalWrite(i,HIGH);
}
  adc.init();
  for (int i = 0; i < MOVING_AVG_SIZE; i++) {
    compressor.addValue(5.81252*adc.read(COMPRESSOR_PT, SD) - 4427.68126);
    trailer1.addValue(5.81252*adc.read(TRAILER1_PT, SD) - 4427.68126);
    trailer2.addValue(5.81252*adc.read(TRAILER2_PT, SD) - 4427.68126);
    dispenser.addValue(5.81252*adc.read(DISPENSER_PT, SD) - 4427.68126);
  }
  sendWord.word = 0;
  readWord.word = 0;
  matrixSetup("SmartGasModule", "v0.0.5");  //M master
  for(int i = 0; i < numDisp;i++){
    dispensers[i].rtuID = i+1;
  }
  Serial.println("OK");
  //tog[5] = true;
  delay(100);
}

void loop() {
  SerialCLI();
  for(int i = 0; i < 16;i++){
    bitWrite(readWord.word,i,!mcp1.digitalRead(i));
    mcp2.digitalWrite(i,bitRead(sendWord.word,i)?LOW:HIGH);
  }

  if (!timer[2]) { timer[2] = millis(); }
  if (millis() - timer[2] > 250) {
    compressor.addValue(5.81252*adc.read(COMPRESSOR_PT, SD) - 4427.68126);
    trailer1.addValue(5.81252*adc.read(TRAILER1_PT, SD) - 4427.68126);
    trailer2.addValue(5.81252*adc.read(TRAILER2_PT, SD) - 4427.68126);
    dispenser.addValue(5.81252*adc.read(DISPENSER_PT, SD) - 4427.68126);
    timer[2] = millis();
  }

  compressorPt = compressor.getAverage();
  trailer1Pt = trailer1.getAverage();
  trailer2Pt = trailer2.getAverage();
  dispenserPt = dispenser.getAverage();

  transceiveRTU();
  daughterPrint(500);
  printOuts(1000);

  if(!manual){
    sendWord.d2 = readWord.DMD_IN;
    sendWord.ALIGHT = readWord.DMD_IN;
    sendWord.GLIGHT = !readWord.DMD_IN;

    if(compressorPt >= 1000){
      if(!readWord.DMD_IN && readWord.SW1){
        sendWord.c1 = true;
        sendWord.c2 = false;
        sendWord.d1 = false;
        sendWord.d2 = false;
      }
      else{
        sendWord.c1 = false;
        sendWord.c2 = true;
        sendWord.d1 = false;
      }
      timer[0] = 0;
    }

    if((sendWord.c1 || sendWord.c2) && compressorPt < 1000){//read xv406
      if(!timer[0]){timer[0] = millis();}
      if(millis() - timer[0] > 60000*10 && timer[0]){
        sendWord.c2 = false;
        timer[0] = 0;
      }
    }
  }
}
