/*
 * putDataIntoEEFROM.ino
 * 这是一个初始化开发板EEPROM的程序，目的只是要把一个颜色值、模式状态放入
 */
#include <EEPROM.h>

void setup() {
  // put your setup code here, to run once:
  uint8_t rval = 0;
  uint8_t gval = 0;
  uint8_t bval = 255;
  int imageNum = 0;
  bool ifChange = true;
//  int address = 0;
  //写入EEPEOM
  EEPROM.put(0, rval);  
  EEPROM.put(1, gval);
  EEPROM.put(2, bval);
  EEPROM.put(3, ifChange);
  EEPROM.put(4, imageNum);
}

void loop() {
  // put your main code here, to run repeatedly:
  ;
}
