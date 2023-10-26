float fmap(float x, float a, float b, float c, float d){
  float f=x/(b-a)*(d-c)+c;
  return f;
}

void daughterPrint(unsigned long inter){
  if(!timer[1]){timer[1] = millis();}
  if (!daughterTimer) { daughterTimer = millis(); }
  if (millis() - daughterTimer > inter && daughterTimer) {
    if(tog[5]){
      Serial.print(compressorPt);
      Serial.print(", ");
      Serial.print(trailer1Pt);
      Serial.print(", ");
      Serial.print(trailer2Pt);
      Serial.print(", ");
      Serial.println(dispenserPt);
    }
    daughterTimer = millis();
    bigMatrix[0].writeInt(compressorPt);  // Write local pressure values to 7-seg screen
    smallMatrix[0].squareDisplay(0);
    bigMatrix[1].writeInt(dispenserPt);
    smallMatrix[1].squareDisplay(0);
    if(millis() - timer[1] > 3000){
      tog[1] = !tog[1];
      timer[1] = millis();
    }
    bigMatrix[2].writeInt(tog[1]?trailer1Pt:trailer2Pt);//add alternating with pt414
    smallMatrix[2].displayChar(tog[1]?"1":"2",false);
  }
}
  
void transceiveRTU() {
  if(rtuCnt < numDisp){ //send/receive carrier
    mbRTU.clearTransmitBuffer();
    mbRTU.setTransmitBuffer(0, MOVING_AVG_SIZE);
    /*
    mbRTU.setTransmitBuffer(1, STALL_THRESHOLD);
    mbRTU.setTransmitBuffer(2, MIN_ALLOWED_PRESSURE);
    mbRTU.setTransmitBuffer(3, DELTA_PSI_TIME);
    mbRTU.setTransmitBuffer(4, STALL_CONFIRMATION_COUNT);
    mbRTU.setTransmitBuffer(5, STALL_TIMEOUT);
    */

    uint8_t result = mbRTU.readWriteMultipleRegisters(dispensers[rtuCnt].rtuID,0,8,20,1);
    if (result == mbRTU.ku8MBSuccess) {
      dispensers[rtuCnt].fillTime = mbRTU.getResponseBuffer(0);
      dispensers[rtuCnt].currentPsi = mbRTU.getResponseBuffer(1);
      dispensers[rtuCnt].initialPsi = mbRTU.getResponseBuffer(2);
      dispensers[rtuCnt].finalPsi = mbRTU.getResponseBuffer(3);
      dispensers[rtuCnt].targetPsi = mbRTU.getResponseBuffer(4);
      dispensers[rtuCnt].valve = mbRTU.getResponseBuffer(5);
      dispensers[rtuCnt].outOfOrder = mbRTU.getResponseBuffer(6);
      dispensers[rtuCnt].lsr = mbRTU.getResponseBuffer(7);
      
      mbRTU.clearResponseBuffer();
      dispensers[rtuCnt].rtuFailCnt = 0;

      String key = "D" + rtuCnt;
      String key_ = key + "TME";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].fillTime);
      key_ = key + "CUR";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].currentPsi);
      key_ = key + "INI";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].initialPsi);
      key_ = key + "FIN";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].finalPsi);
      key_ = key + "TGT";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].targetPsi);
      key_ = key + "VLV";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].valve);
      key_ = key + "OOO";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].outOfOrder);
      key_ = key + "LSR";
      telWriteValue(TEL_ADDR, key_.c_str(),dispensers[rtuCnt].lsr);
    }
    else{dispensers[rtuCnt].rtuFailCnt++;}
    if (dispensers[rtuCnt].rtuFailCnt > 10) {
      Serial.print("Failure to read/write data from slave dispenser #");
      Serial.println(dispensers[rtuCnt].rtuID);
      Serial.print("Modbus Error Code: ");
      Serial.println(result,HEX);
      dispensers[rtuCnt].rtuFailCnt = 0;
    }
    rtuCnt++;
  }
  else{rtuCnt = 0;}

  telWriteValue(TEL_ADDR, GSRER,readWord.GSR_IN);
  telWriteValue(TEL_ADDR, PTCMP,compressorPt);
  telWriteValue(TEL_ADDR, PTTRA,trailer1Pt);
  telWriteValue(TEL_ADDR, PTTRB,trailer2Pt);
  telWriteValue(TEL_ADDR, PTDSP,dispenserPt);
}

void matrixSetup(String s1, String s2) {
  //begin matrix
  for (int i = 0; i < 3; i++) {
    smallMatrix[i].begin();
    bigMatrix[i].begin();
  }
  lcd.begin(16, 2);
  lcd.clear();
  lcd.print(s1);
  lcd.setCursor(0, 1);
  lcd.print(s2);
  lcd.setBacklight(HIGH);
  lcd.display();
}

void preTransmission() {digitalWrite(RE_DE1, 1);}
void postTransmission() {digitalWrite(RE_DE1, 0);}

void pinModeSetup() {
  pinMode(7, OUTPUT);
  digitalWrite(7, HIGH);
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  digitalWrite(LED_BUILTIN, LOW);

  pinMode(CS, OUTPUT);
  digitalWrite(SS, LOW);
  digitalWrite(CS, HIGH);
  pinMode(12, OUTPUT);
  digitalWrite(12, 0);
  pinMode(ESTOP_BREAK, OUTPUT);
  digitalWrite(ESTOP_BREAK, HIGH);

  pinMode(LED_PWR, OUTPUT);
  digitalWrite(LED_PWR, HIGH);

  pinMode(TRACO_24VDC, OUTPUT);
  digitalWrite(TRACO_24VDC, HIGH);
}

