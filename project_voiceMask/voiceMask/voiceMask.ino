/*
 * 灯光面罩
 * 1、显示模式
 * 显示模式有两种：动态和静态
 * 动态模式下说话时图案会根据声音大小不同进行不同的变化，
 * 当发出一个短促的声音并且一定时间内无其他声响，会显露一个“微笑”
 * 静态（禁言）模式下可以切换三种静态图像：“禁止标志”、风车、爱心
 * 
 * 2、颜色模式
 * 颜色模式有两种：温变模式和自定义模式
 * 温变模式下，颜色会时刻根据当下温度进行变化：
 * 随着温度升高灯光颜色偏向暖色调，随着温度降低灯光颜色偏向冷色调，
 * 可以间接的反应温度变化
 * （以下代码中将温度区间设置为10到40摄氏度）
 * 自定义模式下可以自定义当前颜色，这种模式下颜色固定为所调节的颜色（颜色范围为彩虹色，没有白光）
 * 
 * 以上个功能（模式）的切换和使用通过面罩上的按钮进行
 * 
 * 每次断电后开发板会记住上次断电前面罩所处状态
 * 该程序有一个附属程序“putDataIntoEEPROM.ino”用于向EEPROM写入一些状态值，以设置默认的初始状态
 * （对于未下载过该程序的开发板最好先下载“putDataIntoEEPROM.ino”来对EEPROM中的数据进行初始化）
 * 
 */ 
#include <Adafruit_GFX.h>
#include <Adafruit_NeoPixel.h>
#include <Adafruit_NeoMatrix.h>
#include <gamma.h>
#include <math.h>
#include <EEPROM.h>

//以下为表情图案
const PROGMEM uint8_t mouth_0[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {6,6,6,6,6,6,6,6},
  {6,6,6,6,6,6,6,6},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_1[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,6,6,0,0,0},
  {6,6,6,0,0,6,6,6},
  {6,6,6,0,0,6,6,6},
  {0,0,0,6,6,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_2[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {0,6,6,6,6,6,6,0},
  {6,0,0,0,0,0,0,6},
  {6,0,0,0,0,0,0,6},
  {0,6,6,6,6,6,6,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_3[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,6,6,6,6,0,0},
  {0,6,0,0,0,0,6,0},
  {6,0,0,0,0,0,0,6},
  {6,0,0,0,0,0,0,6},
  {0,6,0,0,0,0,6,0},
  {0,0,6,6,6,6,0,0},
  {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_4[8][8] = {
  {0,0,6,6,6,6,0,0},
  {0,6,0,0,0,0,6,0},
  {6,0,0,0,0,0,0,6},
  {6,0,0,0,0,0,0,6},
  {6,0,0,0,0,0,0,6},
  {6,0,0,0,0,0,0,6},
  {0,6,0,0,0,0,6,0},
  {0,0,6,6,6,6,0,0}
};

const PROGMEM uint8_t mouth_smile[8][8] = {
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0},
  {6,0,0,0,0,0,0,6},
  {6,6,0,0,0,0,6,6},
  {0,6,6,6,6,6,6,0},
  {0,0,6,6,6,6,0,0},
  {0,0,0,0,0,0,0,0},
  {0,0,0,0,0,0,0,0}
};

const PROGMEM uint8_t mouth_ban[8][8] = {
  {0,0,6,6,6,6,0,0},
  {0,6,6,0,0,0,6,0},
  {6,6,6,6,0,0,0,6},
  {6,0,6,6,6,0,0,6},
  {6,0,0,6,6,6,0,6},
  {6,0,0,0,6,6,6,6},
  {0,6,0,0,0,6,6,0},
  {0,0,6,6,6,6,0,0}
};

const PROGMEM uint8_t mouth_cross[8][8] = {
  {6,6,6,0,0,0,6,6},
  {6,6,6,0,0,6,6,6},
  {0,6,6,0,6,6,6,6},
  {0,0,6,6,6,0,0,0},
  {0,0,0,6,6,6,0,0},
  {6,6,6,6,0,6,6,0},
  {6,6,6,0,0,6,6,6},
  {6,6,0,0,0,6,6,6}
};

const PROGMEM uint8_t mouth_heart[8][8] {
  {0,6,6,0,0,6,6,0},
  {6,6,6,0,0,6,6,6},
  {6,6,6,6,6,6,6,6},
  {6,6,6,6,6,6,6,6},
  {6,6,6,6,6,6,6,6},
  {0,6,6,6,6,6,6,0},
  {0,0,6,6,6,6,0,0},
  {0,0,0,6,6,0,0,0}
};

const PROGMEM int imageTotal = 4; //图案显示种类, mouth_(num|smile)  mouth_ban  mouth_cross  mouth_heart

const PROGMEM int micPin = A7;  //声音传感器模拟输入引脚
const PROGMEM int matrixPin = 6;  //对WS2812点阵的数据输出引脚

//显示模式转换按钮相关变量
int imageNum;  //图像模式
const PROGMEM int displayModePin = 2; //显示模式切换按钮引脚

//颜色模式转换按钮相关变量
bool ifChange;  //颜色是否随温度改变
const PROGMEM int colorModePin = 3; //颜色模式切换按钮引脚
const PROGMEM int tempPin = A6; //温度传感器模拟值输入
//颜色随温度的变化范围
const PROGMEM int minTemp = 10; //celsius
const PROGMEM int maxTemp = 40;
//颜色变换按钮相关变量
const PROGMEM int colorChangePin = 4; //颜色值改变按钮引脚

//定义一个8x8WS2812点阵对象
Adafruit_NeoMatrix matrix = Adafruit_NeoMatrix(8, 8, matrixPin,
  NEO_MATRIX_TOP     + NEO_MATRIX_RIGHT +
  NEO_MATRIX_ROWS    + NEO_MATRIX_ZIGZAG,
  NEO_GRB            + NEO_KHZ800);
  
//颜色值已经用另一个程序“putColorIntoEEPROM.ino”写入EEPROM

void setup() {
  //设置各功能按钮引脚状态
  pinMode(displayModePin, INPUT_PULLUP);
  pinMode(colorModePin, INPUT_PULLUP);
  pinMode(colorChangePin, INPUT_PULLUP);

  //从EEPROM中读取数据
  EEPROM.get(3, ifChange);
  EEPROM.get(4, imageNum);

  //初始化点阵
  matrix.begin();

  //Serial.begin(9600);
}

void loop() {
  //动态模式
  imageNum = dynamicMode(imageNum, displayModePin);
  //禁言模式
  imageNum = banningMode(imageNum, displayModePin);
} 


//通过NTC热敏电阻的输入模拟值计算当前温度
double themistor(int rawADC) {
  double temp;
  temp = log((10240000/(rawADC)) - 10000);
  temp = 1/(0.001129148 + (0.000234125 + (0.0000000876741 * temp * temp)) * temp);
  temp = temp - 273.15;
  return temp;
}

//获取颜色
uint16_t getColor(bool ifChange) {
  uint8_t rval, gval, bval;
  //按不同的颜色模式处理rgb值
  if(ifChange) {
    //减小温度值的波动
    int samples = 128;
    long analog = 0;
    for(int i = 0; i < samples; i ++) {
      analog += analogRead(tempPin);
    }
    analog = analog / samples;
    double temperature = themistor((int)analog); //去多个模拟值的平均值，减小温度值波动
    //根据温度不同进行不同处理
    if(temperature < minTemp) {
      rval = 0;
      gval = 0;
      bval = 255;
    }else if(temperature > maxTemp) {
      rval = 255;
      gval = 0;
      bval = 0;
    }else {
      rval = map(temperature, minTemp, maxTemp, 0, 255);
      bval = map(temperature, minTemp, maxTemp, 255, 0);
      gval = 510 - rval - bval;
    }
  }else {
    //取得颜色值
    EEPROM.get(0, rval);  
    EEPROM.get(1, gval);
    EEPROM.get(2, bval);
    int ifValChange = digitalRead(colorChangePin);
    if(ifValChange == 0){
      if(bval == 255 && gval != 255 && rval == 0) {
        gval += 5;
      }else if(bval != 0 && gval == 255) {
        bval -= 5;
      }else if(gval == 255 && rval !=255) {
        rval += 5;
      }else if(gval != 0 && rval == 255) {
        gval -= 5;
      }else if(rval == 255 && bval != 255) {
        bval += 5;
      }else if(rval != 0 && bval == 255) {
        rval -= 5;
      }
      //记录新的颜色值
      EEPROM.put(0, rval);  
      EEPROM.put(1, gval);
      EEPROM.put(2, bval);
      
      //delay(10); //debounce
    }
  }
  uint16_t colorval = matrix.Color(rval, gval, bval);
  return colorval;
}

//显示相应图案
void drawImage(short image_addr, bool ifChange){
  uint16_t currentColor; //当前颜色
  //判断颜色是否随温度变化,并按对应模式给予颜色
  currentColor = getColor(ifChange);
  //给灯珠填充颜色
  for(int x = 0; x<8; x++){
    for(int y = 0; y<8; y++){
      //注意以下代码中来自avr/pgmspace.h的pgm_byte_xx()函数
      uint8_t index = pgm_read_byte_near(image_addr+x+y*8); //从flash程序空间中读取表情图案数据
      //uint8_t index = pgm_read_byte((short)mouth_ban+x+y*8);
      //uint16_t currentColor;// = pgm_read_word_near(palette + index);
      
      //向位于(x,y)的点写入颜色
      if(index == 0) {
        matrix.drawPixel(x, y, (uint16_t)0x0); 
      }else {
        matrix.drawPixel(x, y, currentColor);
      }
    }
  }
  matrix.show();
}

//动态模式
int dynamicMode(int imageNum, int disPlayModePin) {
  int popDetection = 0; //检测到符合条件的峰值的次数
  const static int PROGMEM noDetectionT = 500; //ms 没有检测到符合条件峰值的间隔时间阈值
  bool smiling = false; //是否微笑状态
  const static int PROGMEM smileLasting = 2000; //ms 微笑保持时间
  unsigned long smiletimer = 0; //微笑保持时间计时
  unsigned long last_face = 0;  //上一次表情状态时间点
  
  const static int PROGMEM peakThreshold = 150; //峰值绝对值的阈值
  const static int PROGMEM mouth1Threshold = 150; //mouth_1的显示阈值
  const static int PROGMEM mouth2Threshold = 200; //mouth_2的显示阈值
  const static int PROGMEM mouth3Threshold = 300; //mouth_3的显示阈值
  const static int PROGMEM mouth4Threshold = 400; //mouth_4的显示阈值
  const static uint16_t PROGMEM samples = 128; //取128个模拟值样本
  const static int PROGMEM micThreshold = 512; //声音传感器阈值，实际操作时阈值大小大概在500到550间

  //切换按钮状态值
  int oldStateDisplay = 1;
  int newStateDisplay;

  int oldStateColor = 1;
  int newStateColor;
  
  while(imageNum == 0) {
    float micAbsPeak = 0; //峰值的绝对值
    for (int i = 0; i < samples; i ++){
      auto analog = analogRead(micPin);
      auto micLine = abs(analog - micThreshold);
      
      micAbsPeak = max(micLine, micAbsPeak);
    }
    
    if(micAbsPeak > peakThreshold){
      popDetection += 1;
      if(popDetection > 2) {
        smiling = false;
        last_face = millis();
      }
    }else{
      if(popDetection > 0 && popDetection <= 2) {
        if(millis() > last_face + noDetectionT){
          smiling = true;
          smiletimer = millis() + smileLasting;
        }
      }
      popDetection = 0; //这个很关键
    }
  
    if(millis() > smiletimer) 
      smiling = false;
  
    //以下的micAbsVal全部改用micAbsPeak
    if(smiling){
      drawImage((short)mouth_smile, ifChange);
    }else if(micAbsPeak < mouth1Threshold){
      drawImage((short)mouth_0, ifChange);
    }else if(micAbsPeak < mouth2Threshold){
      drawImage((short)mouth_1, ifChange);
    }else if(micAbsPeak < mouth3Threshold){
      drawImage((short)mouth_2, ifChange);
    }else if(micAbsPeak < mouth4Threshold){
      drawImage((short)mouth_3, ifChange);
    }else {
      drawImage((short)mouth_4, ifChange);
    }
    
    //显示模式切换按钮
    newStateDisplay = digitalRead(displayModePin);
    if(newStateDisplay == 1 && oldStateDisplay == 0){
      imageNum = (imageNum + 1) % imageTotal;
      EEPROM.put(4, imageNum);
    }
    oldStateDisplay = newStateDisplay;

    //颜色模式切换按钮
    newStateColor = digitalRead(colorModePin);
    if(newStateColor == 1 && oldStateColor == 0) {
      ifChange = !ifChange;
      EEPROM.put(3, ifChange);
    }
    oldStateColor = newStateColor;
  }
  return imageNum;
}

//禁言模式
int banningMode(int imageNum, int displayModePin) {
  //切换按钮状态值
  int oldStateDisplay = 1;
  int newStateDisplay;

  int oldStateColor = 1;
  int newStateColor;
  
  while(imageNum != 0) {
    if(imageNum == 1) {
      drawImage((short)mouth_ban, ifChange);  //禁止标志图案
    }else if(imageNum == 2) {
      drawImage((short)mouth_cross, ifChange); //X图案
    }else if(imageNum == 3) {
      drawImage((short)mouth_heart, ifChange); //X图案
    }
    //显示模式切换按钮
    newStateDisplay = digitalRead(displayModePin);
    if(newStateDisplay == 1 && oldStateDisplay == 0){
      imageNum = (imageNum + 1) % imageTotal;
      EEPROM.put(4, imageNum);
    }
    oldStateDisplay = newStateDisplay;

    //颜色模式切换按钮
    newStateColor = digitalRead(colorModePin);
    if(newStateColor == 1 && oldStateColor == 0) {
      ifChange = !ifChange;
    }
    oldStateColor = newStateColor;
  }
  return imageNum;
}
