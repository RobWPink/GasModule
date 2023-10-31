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
    Serial.println(argBuf[0]);
    Serial.println(argBuf[1]);
    Serial.println(argBuf[2]);
    /*
    for(int n = 0; n < numArgs; n++){
      String argStr = argBuf[n];
      if(argStr.equalsIgnoreCase("disp")){
        String argStrVal = argBuf[++n];
        argVal = argStrVal.toDouble();
      }

      else{
        Serial.println("Invalid entry");
      }
    }*/
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
    if(tog[5]){
      Serial.print(readWord.ABUTTON);
      Serial.print(", ");
      Serial.print(readWord.RBUTTON);
      Serial.print(", ");
      Serial.print(readWord.GBUTTON);
      Serial.print(", ");
      Serial.print(readWord.SW1);
      Serial.print(", ");
      Serial.print(readWord.LSR_IN);
      Serial.print(", ");
      Serial.print(readWord.GSR_IN);
      Serial.print(", ");
      Serial.print(readWord.DMD_IN);
      Serial.print(", ");
      Serial.print(readWord.ESTOP_IN);
      Serial.print(", ");
      Serial.print(sendWord.d1);
      Serial.print(", ");
      Serial.print(sendWord.d2);
      Serial.print(", ");
      Serial.print(sendWord.c1);
      Serial.print(", ");
      Serial.print(sendWord.c2);
      Serial.print(", ");
      Serial.print(sendWord.OK);
      Serial.print(", ");
      Serial.print(sendWord.GSR);
      Serial.print(", ");
      Serial.print(sendWord.LSR);
      Serial.print(", ");
      Serial.print(sendWord.DMD_OUT1);
      Serial.print(", ");
      Serial.print(sendWord.DMD_OUT2);
      Serial.print(", ");
      Serial.print(sendWord.SUP_OUT1);
      Serial.print(", ");
      Serial.print(sendWord.SUP_OUT2);
      Serial.print(", ");
      Serial.print(sendWord.GLIGHT);
      Serial.print(", ");
      Serial.print(sendWord.ALIGHT);
      Serial.print(", ");
      Serial.print(sendWord.RLIGHT);
      Serial.print(", ");
      Serial.print(compressorPt);
      Serial.print(", ");
      Serial.print(trailer1Pt);
      Serial.print(", ");
      Serial.print(trailer2Pt);
      Serial.print(", ");
      Serial.print(dispenserPt);
      Serial.print(", ");
      Serial.print(numDisp);
      Serial.print(", ");
      for(int i = 0; i < numDisp; i++){
        Serial.print(i+1); //dispenser #
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
        Serial.print(dispensers[i].lsr?1:0);
        if(i != numDisp + 1){Serial.print(",");}
      }
      Serial.println();
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
}
