#include <RunningAverage.h>
#include <LedHelper.h>
#include <SPI.h>
#include <Wire.h> //For I2C comms
#include "Adafruit_LEDBackpack.h"
#include "Adafruit_LiquidCrystal.h"
#include <ModbusMaster_oneH2.h>
#include <Adafruit_MCP23X17.h>
#include <ADS7828.h>
#include "I2C_API.h"

#define ESTOP_BREAK 40
#define LED_PWR 22
#define TRACO_24VDC 23

#define CS 4     //SD CS pin
int selected = 0;
union mcp2word{
  uint16_t word;
  struct{
    //GPIOA
    uint8_t d1 : 1;//dispenser channel 1
    uint8_t c1 : 1;//compressor channel 1
    uint8_t d2 : 1;
    uint8_t c2 : 1;
    uint8_t OK : 1;
    uint8_t GSR : 1;
    uint8_t LSR : 1;
    uint8_t NAA : 1;
    //GPIOB
    uint8_t DMD_OUT1 : 1;
    uint8_t DMD_OUT2 : 1;
    uint8_t SUP_OUT1 : 1;
    uint8_t SUP_OUT2 : 1;
    uint8_t GLIGHT : 1;
    uint8_t ALIGHT : 1;
    uint8_t RLIGHT : 1;
    uint8_t NAB : 1;
  };
}sendWord;

union mcp1word{
  uint16_t word;
  struct{
    uint8_t ABUTTON : 1;
    uint8_t RBUTTON : 1;
    uint8_t NA1 : 1;
    uint8_t NA2 : 1;
    uint8_t NA3 : 1;
    uint8_t NA4 : 1;
    uint8_t NA5 : 1;
    uint8_t NAA : 1;

    uint8_t SW1 : 1;
    uint8_t SW2 : 1;
    uint8_t LSR_IN : 1;
    uint8_t DMD_IN : 1;
    uint8_t GSR_IN : 1;
    uint8_t ESTOP_IN : 1;
    uint8_t GBUTTON : 1;
    uint8_t NAB : 1;
  };
}readWord;
#define COMPRESSOR_PT 1
#define TRAILER1_PT 2
#define TRAILER2_PT 3
#define DISPENSER_PT 0

#define TT_AMBIENT 4
#define SPARE 5
#define SELECTOR1 6
#define SELECTOR2 7

#define RE_DE1 12
#define RE_DE3 31

ModbusMaster mbRTU;
ADS7828 adc(0x48);
Adafruit_MCP23X17 mcp1;
Adafruit_MCP23X17 mcp2;

volatile int  MOVING_AVG_SIZE      = 20;     // Number of data points to average together to smooth pressure readings
const int numDisp = 1;//6;

unsigned long flashTimer1 = 0;
unsigned long flashTimer2 = 0;
unsigned long daughterTimer = 0;

struct DISPENSER{
  int rtuID;
  unsigned long fillTime;
  double currentPsi;
  double initialPsi;
  double finalPsi;
  double targetPsi;
  int rtuFailCnt;
  bool outOfOrder;
  bool outOfOrderSend;
  bool valve;
  bool valveSend;
  bool lsr;
  bool print;
};

DISPENSER dispensers[numDisp] = {0,0,0,0,0,0,0,false,false,false,false,false,false};

unsigned long timer[13] = {0};
double compressorPt = 0;
double trailer1Pt = 0;
double trailer2Pt = 0;
double dispenserPt = 0;
bool manual = false;
bool tog[10] = {false};
bool err_timeout = false;
int rtuCnt = 0;
