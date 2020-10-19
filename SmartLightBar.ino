// v 2.2
/*
 * 这是一个基于Blinker和Fastled、lightpack的灯带项目，请在代码中填入自己的auth key和WiFi配置。
 */

/* *****************************************************************
 *
 * Download latest Blinker library here:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * 
 * Blinker is a cross-hardware, cross-platform solution for the IoT. 
 * It provides APP, device and server support, 
 * and uses public cloud services for data transmission and storage.
 * It can be used in smart home, data monitoring and other fields 
 * to help users build Internet of Things projects better and faster.
 * 
 * Make sure installed 2.7.4 or later ESP8266/Arduino package,
 * if use ESP8266 with Blinker.
 * https://github.com/esp8266/Arduino/releases
 * 
 * Make sure installed 1.0.4 or later ESP32/Arduino package,
 * if use ESP32 with Blinker.
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * Docs: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************
 * 
 * Blinker 库下载地址:
 * https://github.com/blinker-iot/blinker-library/archive/master.zip
 * 
 * Blinker 是一套跨硬件、跨平台的物联网解决方案，提供APP端、设备端、
 * 服务器端支持，使用公有云服务进行数据传输存储。可用于智能家居、
 * 数据监测等领域，可以帮助用户更好更快地搭建物联网项目。
 * 
 * 如果使用 ESP8266 接入 Blinker,
 * 请确保安装了 2.7.4 或更新的 ESP8266/Arduino 支持包。
 * https://github.com/esp8266/Arduino/releases
 * 
 * 如果使用 ESP32 接入 Blinker,
 * 请确保安装了 1.0.4 或更新的 ESP32/Arduino 支持包。
 * https://github.com/espressif/arduino-esp32/releases
 * 
 * 文档: https://diandeng.tech/doc
 *       
 * 
 * *****************************************************************/

#define BLINKER_WIFI
#define BLINKER_MIOT_LIGHT

#include <Blinker.h>
#include <EEPROM.h>
#define EEPROM_write(address,p) { int i = 0;byte *pp = (byte*)&(p);for(;i<sizeof(p);i++) EEPROM.write(address+i,pp[i]);EEPROM.end();}
#define EEPROM_read(address,p) { int i = 0;byte *pp = (byte*)&(p);for(;i<sizeof(p);i++) pp[i]=EEPROM.read(address+i);}

char auth[] = "Your auth key here";
char ssid[] = "Your WiFi SSID here";
char pswd[] = "Your WiFi Password here";

// Download Adafruit_NeoPixel library here:
// https://github.com/adafruit/Adafruit_NeoPixel
#include "FastLED.h"
#define NUM_LEDS 60
#define DATA_PIN 8 

CRGB leds[NUM_LEDS];

#define RGB_1 "RGBKey"

BlinkerRGB RGB1(RGB_1);

uint8_t colorR, colorG, colorB, colorW;
bool wsState;
bool computerMode=false;
uint8_t wsMode = BLINKER_CMD_MIOT_DAY;

uint8_t prefix[] = {'A', 'd', 'a'}, hi, lo, chk, i;

bool settingChanged=false;

uint32_t getColor()
{
    uint32_t color = colorR << 16 | colorG << 8 | colorB;

    return color;
}

//调亮度
void set_brightness(uint8_t bright_value)
{
    if(bright_value>0){
        wsState=true; 
     }
    //int flag=bright_value>colorW?1:-1;
    /*
    while(colorW!=bright_value){
      colorW+=flag;
      FastLED.setBrightness(colorW);
      FastLED.show();
      FastLED.delay(10);  
    }
    */
    colorW=bright_value;
    FastLED.setBrightness(int(wsState)*colorW);
    //saveData();
    settingChanged=true;
}

//控制回调
void ws2812_callback(uint8_t r_value, uint8_t g_value, uint8_t b_value, uint8_t bright_value)
{
    digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
    BLINKER_LOG("R value: ", r_value);
    BLINKER_LOG("G value: ", g_value);
    BLINKER_LOG("B value: ", b_value);
    BLINKER_LOG("Rrightness value: ", bright_value);

    colorR = r_value;
    colorG = g_value;
    colorB = b_value;
    set_brightness(bright_value);
    //saveData();
    settingChanged=true;
    FastLED.show();
}

//开关
void miotPowerState(const String & state)
{
    BLINKER_LOG("need set power state: ", state);

    if (state == BLINKER_CMD_ON) {
        digitalWrite(LED_BUILTIN, HIGH);

        BlinkerMIOT.powerState("on");
        BlinkerMIOT.print();
        wsState = true;
    }
    else if (state == BLINKER_CMD_OFF) {
        digitalWrite(LED_BUILTIN, LOW);

        BlinkerMIOT.powerState("off");
        BlinkerMIOT.print();
        wsState = false;
    }
    FastLED.setBrightness(int(wsState)*colorW);
    //saveData();
    settingChanged=true;
    FastLED.show();
}

//调整颜色
void miotColor(int32_t color)
{
    BLINKER_LOG("need set color: ", color);

    colorR = color >> 16 & 0xFF;
    colorG = color >>  8 & 0xFF;
    colorB = color       & 0xFF;

    BLINKER_LOG("colorR: ", colorR, ", colorG: ", colorG, ", colorB: ", colorB);

    FastLED.show();

    BlinkerMIOT.color(color);
    //saveData();
    settingChanged=true;
    BlinkerMIOT.print();
}

//模式调节
void miotMode(uint8_t mode)
{
    BLINKER_LOG("need set mode: ", mode);

    if (mode == BLINKER_CMD_MIOT_DAY) {
      
        // Your mode function
    }
    else if (mode == BLINKER_CMD_MIOT_NIGHT) {
        // Your mode function
    }
    else if (mode == BLINKER_CMD_MIOT_COLOR) {
        computerMode=false;
        // Your mode function
    }
    else if (mode == BLINKER_CMD_MIOT_WARMTH) {
        // Your mode function
    }
    else if (mode == BLINKER_CMD_MIOT_TV) {
        // Your mode function
    }
    else if (mode == BLINKER_CMD_MIOT_READING) {
        // Your mode function
        computerMode=false;
        colorR=255;
        colorG=67;
        colorB=0;
        colorW=255;
        
        FastLED.show();

        BlinkerMIOT.color(getColor());
        //saveData();
        settingChanged=true;
        BlinkerMIOT.print();
    }
    else if (mode == BLINKER_CMD_MIOT_COMPUTER) {
      computerMode=true;
        // Your mode function
    }

    wsMode = mode;

    BlinkerMIOT.mode(mode);
    BlinkerMIOT.print();
}

//调亮度
void miotBright(const String & bright)
{
    BLINKER_LOG("need set brightness: ", bright);

    set_brightness(2.55*bright.toInt());

    BLINKER_LOG("now set brightness: ", colorW);
    //FastLED.setBrightness(colorW);
    FastLED.show();

    BlinkerMIOT.brightness(colorW/2.55);
    //saveData();
    settingChanged=true;
    BlinkerMIOT.print();
}

//调色温，目前没用
void miotColoTemp(int32_t colorTemp)
{
    BLINKER_LOG("need set colorTemperature: ", colorTemp);

    BlinkerMIOT.colorTemp(colorTemp);
    BlinkerMIOT.print();
}

void miotQuery(int32_t queryCode)
{
    BLINKER_LOG("MIOT Query codes: ", queryCode);

    switch (queryCode)
    {
        case BLINKER_CMD_QUERY_ALL_NUMBER :
            BLINKER_LOG("MIOT Query All");
            BlinkerMIOT.powerState(wsState ? "on" : "off");
            BlinkerMIOT.color(0);
            BlinkerMIOT.mode(0);
            BlinkerMIOT.colorTemp(1000);
            BlinkerMIOT.brightness(1);
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_POWERSTATE_NUMBER :
            BLINKER_LOG("MIOT Query Power State");
            BlinkerMIOT.powerState(wsState ? "on" : "off");
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_COLOR_NUMBER :
            BLINKER_LOG("MIOT Query Color");
            BlinkerMIOT.color(0);
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_MODE_NUMBER :
            BLINKER_LOG("MIOT Query Mode");
            BlinkerMIOT.mode(0);
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_COLORTEMP_NUMBER :
            BLINKER_LOG("MIOT Query ColorTemperature");
            BlinkerMIOT.colorTemp(1000);
            BlinkerMIOT.print();
            break;
        case BLINKER_CMD_QUERY_BRIGHTNESS_NUMBER :
            BLINKER_LOG("MIOT Query Brightness");
            BlinkerMIOT.brightness(1);
            BlinkerMIOT.print();
            break;
        default :
            BlinkerMIOT.powerState(wsState ? "on" : "off");
            BlinkerMIOT.color(0);
            BlinkerMIOT.mode(0);
            BlinkerMIOT.colorTemp(1000);
            BlinkerMIOT.brightness(1);
            BlinkerMIOT.print();
            break;
    }
}

void dataRead(const String & data)
{
    BLINKER_LOG("Blinker readString: ", data);

    Blinker.vibrate();
    
    uint32_t BlinkerTime = millis();
    
    Blinker.print("millis", BlinkerTime);
}

void recoverData(){
  EEPROM.begin(4);
  int32_t color;
  EEPROM_read(0, color);
  BLINKER_LOG("Recover data from eeprom:");
  BLINKER_LOG(color);
  colorR = color >> 24 & 0xFF;
  colorG = color >>  16 & 0xFF;
  colorB = color >>  8 & 0xFF;
  colorW = color       & 0xFF;
  BLINKER_LOG("R: ",colorR,"G: ",colorG,"B: ",colorB,"W: ",colorW);
}

void saveData(){
  EEPROM.begin(4);
  int32_t color=getColor();
  color=color<<8|colorW;
  EEPROM_write(0, color);
  EEPROM.commit();
}

void setup()
{
    Serial.begin(115200); 
    BLINKER_DEBUG.stream(Serial);
    BLINKER_DEBUG.debugAll();

    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, LOW);
    

    Blinker.begin(auth, ssid, pswd);
    Blinker.attachData(dataRead);
    BLINKER_LOG("0000");
    recoverData();
    BlinkerMIOT.attachPowerState(miotPowerState);
    BlinkerMIOT.attachColor(miotColor);
    BlinkerMIOT.attachMode(miotMode);
    BlinkerMIOT.attachBrightness(miotBright);
    BlinkerMIOT.attachColorTemperature(miotColoTemp);
    BlinkerMIOT.attachQuery(miotQuery);
  BLINKER_LOG("1111");

    pinMode(14, OUTPUT);
    digitalWrite(14, HIGH);
    pinMode(15, OUTPUT);
    digitalWrite(15, HIGH);

/*
    colorR = 255;
    colorG = 255;
    colorB = 255;
    colorW = 0;
*/
    wsState = true;
  
    // Use NEOPIXEL to keep true colors
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);

  /*
  // Initial RGB flash
  LEDS.showColor(CRGB(255, 0, 0));
  delay(500);
  LEDS.showColor(CRGB(0, 255, 0));
  delay(500);
  LEDS.showColor(CRGB(0, 0, 255));
  delay(500);
  LEDS.showColor(CRGB(0, 0, 0));
  */
  FastLED.setBrightness(colorW);
  RGB1.attach(ws2812_callback);
   leds[0].fadeToBlackBy( 64 );
}

void waitForSerial(){
  while (!Serial.available()) Blinker.run();
}


void loop()
{
    Blinker.run();
    if(computerMode){
      // Wait for first byte of Magic Word
      for(i = 0; i < sizeof prefix; ++i) {
        waitLoop: waitForSerial();;
        // Check next byte in Magic Word
        if(prefix[i] == Serial.read()) continue;
        // otherwise, start over
        i = 0;
        goto waitLoop;
      }
      
      // Hi, Lo, Checksum  
      waitForSerial();;
      hi=Serial.read();
      waitForSerial();;
      lo=Serial.read();
      waitForSerial();;
      chk=Serial.read();
      
      // If checksum does not match go back to wait
      if (chk != (hi ^ lo ^ 0x55)) {
        i=0;
        goto waitLoop;
      }
      
      memset(leds, 0, NUM_LEDS * sizeof(struct CRGB));
      // Read the transmission data and set LED values
      for (uint8_t i = 0; i < NUM_LEDS; i++) {
        byte r, g, b;    
        waitForSerial();;
        r = Serial.read();
        waitForSerial();;
        g = Serial.read();
        waitForSerial();;
        b = Serial.read();
        leds[i].r = r;
        leds[i].g = g;
        leds[i].b = b;
      }
      
      // Shows new values
      FastLED.show();  
    }else{
      LEDS.showColor(CRGB(colorR, colorG, colorB));    
    }
    if(settingChanged){
      saveData();
      settingChanged=false;
    }
}
