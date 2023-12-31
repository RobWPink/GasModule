#define SPTR_SIZE   20
void printBits(uint16_t myWord,bool nl){
  for(uint16_t mask = 0x8000; mask; mask >>= 1){
    if(mask & myWord)
    Serial.print('1');
    else
    Serial.print('0');
  }
  if(nl){Serial.println();}
}
void SerialCLI(){
  String str;
  char *argBuf[20] = {0};
  double argVal = 0;

  while(Serial.available()) { str = Serial.readStringUntil('\n'); }
  if (str.length() > 0) {
    int numArgs = parseString(str, argBuf, (int)sizeof(argBuf)); // arguments are stored in argBuf
    for(int n = 0; n < numArgs; n++){
      String argStr = argBuf[n];
      if(argStr.equalsIgnoreCase("reset")){
        resetFunc();  
      }
      else if(argStr.equalsIgnoreCase("disp")){
        String argStrVal = argBuf[++n];
        argVal = argStrVal.toInt();
        dispensers[argVal+1].print = !dispensers[argVal+1].print;
      }
      else if(argStr.equalsIgnoreCase("read")){
        tog[2] = !tog[2];
      }
      else if(argStr.equalsIgnoreCase("send")){
        tog[3] = !tog[3];
      }
      else if(argStr.equalsIgnoreCase("psi")){
        tog[4] = !tog[4];
      }
      else if(argStr.equalsIgnoreCase("d1")){
        sendWord.d1 = !sendWord.d1;
      }
      else if(argStr.equalsIgnoreCase("c1")){
        sendWord.c1 = !sendWord.c1;
      }
      else if(argStr.equalsIgnoreCase("d2")){
        sendWord.d2 = !sendWord.d2;
      }
      else if(argStr.equalsIgnoreCase("c2")){
        sendWord.c2 = !sendWord.c2;
      }
      else if(argStr.equalsIgnoreCase("ok")){
        sendWord.OK = !sendWord.OK;
      }
      else if(argStr.equalsIgnoreCase("dmd1")){
        sendWord.DMD_OUT1 = !sendWord.DMD_OUT1;
      }
      else if(argStr.equalsIgnoreCase("dmd2")){
        sendWord.DMD_OUT2 = !sendWord.DMD_OUT2;
      }
      else if(argStr.equalsIgnoreCase("sup1")){
        sendWord.SUP_OUT1 = !sendWord.SUP_OUT1;
      }
      else if(argStr.equalsIgnoreCase("sup2")){
        sendWord.SUP_OUT2 = !sendWord.SUP_OUT2;
      }
      else if(argStr.equalsIgnoreCase("green")){
        sendWord.GLIGHT = !sendWord.GLIGHT;
      }
      else if(argStr.equalsIgnoreCase("amber")){
        sendWord.ALIGHT = !sendWord.ALIGHT;
      }
      else if(argStr.equalsIgnoreCase("red")){
        sendWord.RLIGHT = !sendWord.RLIGHT;
      }

      else{
        Serial.println("Invalid entry");
      }
    }
  }
}

int parseString(String str, char **p, int size){
  char s[100] = {0};
  int n = 0;
  strcpy(s, str.c_str());
  *p++ = strtok(s, " ");
  for(n = 1; NULL != (*p++ = strtok(NULL, " ")); n++){
    if(size == n){ break; }
  }
  return n;
}

void printOuts(unsigned long inter){
  if(!timer[3]){timer[3] = millis();}
  if(millis() - timer[3] > inter && timer[3]){
    timer[3] = millis();
    if(tog[2]){Serial.print("READWORD: "); printBits(readWord.word,true);}
    if(tog[3]){Serial.print("SENDWORD: "); printBits(sendWord.word,true);}
    if(tog[4]){
        Serial.print("PSI: ");
        Serial.print(compressorPt);
        Serial.print(", ");
        Serial.print(trailer1Pt);
        Serial.print(", ");
        Serial.print(trailer2Pt);
        Serial.print(", ");
        Serial.println(dispenserPt);
      }
    for(int i = 0; i < numDisp;i++){
      if(dispensers[i].print){
        Serial.print("DISPENSER ");
        Serial.print(i+1);
        Serial.print(": ");
        Serial.print(dispensers[i].currentPsi);
        Serial.print(",");
        Serial.print(dispensers[i].targetPsi);
        Serial.print(",");
        Serial.print(dispensers[i].initialPsi);
        Serial.print(",");
        Serial.print(dispensers[i].finalPsi);
        Serial.print(",");
        Serial.print(dispensers[i].fillTime);
        Serial.print(",");
        Serial.print(dispensers[i].valve?1:0);
        Serial.print(",");
        Serial.print(dispensers[i].outOfOrder?1:0);
        Serial.print(",");
        Serial.println(dispensers[i].lsr?1:0);
      }
    }
  }
