#include <Adafruit_NeoPixel.h>

// Arduino Nano v2.x, v3.x

#define TOTAL_PIXELS 99 // 大人用バスケ（緑・シルバー・黄）, 展示大人用テニス
// #define TOTAL_PIXELS 94 // 大人用バスケ（無塗装）
// #define TOTAL_PIXELS 72 // 子供用バスケ（黃）
// #define TOTAL_PIXELS 85 // 子供用スポーツ（青）

// common settings :::::::::

#define MAX_VAL 64 // 0 to 255 for brightness 多くのLEDを制御した場合に、色が安定しない問題を解決するために設けた制限

// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIXELS, 6, NEO_GRB + NEO_KHZ800);

// serial communication
char input[11];   // 文字列格納用
String input_str;   // 文字列格納用
int i = 0;  // 文字数のカウンタ

// instrument types
#define INSTRUMENT_TYPE_DEFAULT 0
#define INSTRUMENT_TYPE_ONESHOT 1
#define INSTRUMENT_TYPE_LONGSHOT 2
int i_type = INSTRUMENT_TYPE_ONESHOT;

// instrument color
int i_color_r = 255;
int i_color_g = 0;
int i_color_b = 0;
float instrument_brightness = 0.1;
// use oneshot only
float instrument_brightness_alt = 0.1;
float instrument_brightness_min = 0.1;

// effect color
int e_color_r = 0;
int e_color_g = 0;
int e_color_b = 255;
#define EFFECT_BRIGHTNESS_MIN 0.02
float effect_brightness = EFFECT_BRIGHTNESS_MIN;
float effect_brightness_target = EFFECT_BRIGHTNESS_MIN;

int divide = 2; // LEDパターンの分割数
float slit_position = 0.5; // 0~1 分割された区分の中のどこまでがinstrumentにするか。0だと全部effect color。1だと全部instrument color。

bool instrument_flag = true; // switch by sound trigger

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);

  // startup animation
  rainbow(2);
  colorWipe(0, 2);

  updateLED();
}

void loop() {
  if (0<Serial.available()) {
    String command = Serial.readStringUntil(':');
    String argument = Serial.readStringUntil(';');

    // set instrument type
    // "t:[0|1];"
    if(command=="t") {
      Serial.println(command);
      if(argument.toInt()==INSTRUMENT_TYPE_ONESHOT || argument.toInt()==INSTRUMENT_TYPE_LONGSHOT) {
        i_type = argument.toInt();
        updateLED();
      }
      else {
        Serial.print("Invalid argument: ");
        Serial.println(argument);
      }
    }
    // set instrument color
    // "i:[000-255].[000-255].[000-255];"
    else if(command=="i") {
      Serial.println(command);
      if(argument.length()==11) {

        i_color_r = argument.substring(0,3).toInt();
        i_color_g = argument.substring(4,7).toInt();
        i_color_b = argument.substring(8,11).toInt();
        updateLED();
      }
      else {
        Serial.print("Invalid argument: ");
        Serial.println(argument);
      }
    }
    // set effect color
    // "e:[000-255].[000-255].[000-255];"
    // else if(command=="e") {
    //   Serial.println(command);
    //   if(argument.length()==11) {
    //     e_color_r = argument.substring(0,3).toInt();
    //     e_color_g = argument.substring(4,7).toInt();
    //     e_color_b = argument.substring(8,11).toInt();
    //     updateLED();
    //   }
    //   else {
    //     Serial.print("Invalid argument: ");
    //     Serial.println(argument);
    //   }
    // }
    // set effect_brightness_target value
    // "E:[0.0-1.0];"
    else if(command=="E") {
      // TODO argument check
      // argument is float
      effect_brightness_target = max(argument.toFloat(),EFFECT_BRIGHTNESS_MIN);
      // Serial.print("set effect_brightness_target value: ");
      // Serial.println(effect_brightness_target);
      updateLED();
    }
    // sound trigger
    // "s:;"
    else if(command=="s") {
      if(instrument_flag){
        instrument_brightness = 1.0;
      }
      else {
        instrument_brightness_alt = 1.0;
      }
      instrument_flag = !instrument_flag;
    }
    // set divide number
    // "d:[0-9];"
    else if(command=="d") {

      if(argument.length()==1) {
        // argument is int
        divide = argument.toInt();
        // Serial.print("set divide number: ");
        // Serial.println(divide);
        updateLED();
      }
      else {
        Serial.print("Invalid argument: ");
        Serial.println(argument);
      }
    }
    // set position
    // "p:[0.0-1.0];"
    else if(command=="p") {
      // TODO argument check
      // argument is float
      slit_position = argument.toFloat();
      // Serial.print("set position: ");
      // Serial.println(slit_position);
      updateLED();
    }
    // set instrument_brightness_min value
    // "b:[0.0-1.0];"
    else if(command=="b") {
      // TODO argument check
      // argument is float
      instrument_brightness_min = argument.toFloat();
      // Serial.print("set instrument_brightness min value: ");
      // Serial.println(instrument_brightness_min);
      updateLED();
    }
    // set instrument_brightness value
    // "b:[0.0-1.0];"
    else if(command=="B") {
      if(i_type==INSTRUMENT_TYPE_LONGSHOT){
        // TODO argument check
        // argument is float
        instrument_brightness = argument.toFloat();
        // Serial.print("set instrument_brightness min value: ");
        updateLED();
      }
    }
    else {
      Serial.print("Command not found: ");
      Serial.println(command);
    }
  }
  else {

    // instrument brightness

    if(i_type==INSTRUMENT_TYPE_ONESHOT){
      bool needs_update = false;
      if(instrument_brightness_min+0.01 <= instrument_brightness) {
        instrument_brightness -= 0.02;
        needs_update = true;
      }
      if(instrument_brightness_min+0.01 <= instrument_brightness_alt) {
        instrument_brightness_alt -= 0.02;
        needs_update = true;
      }
      if(needs_update) {
        updateLED();
      }
    }


    // effect brightness

    // fade effect brightness to effect_brightness_target value
    float diff = abs(effect_brightness_target-effect_brightness);
    if(0.01<diff){
      if(effect_brightness_target < effect_brightness) {
        effect_brightness -= 0.01;
      }
      else {
        effect_brightness += 0.01;
      }
      updateLED();
    }
    else if(diff != 0.0){
      effect_brightness = effect_brightness_target;
      updateLED();
    }
  }
}

// limitation of brightness
// 0-255の値を0-MAX_VALの範囲に
uint32_t constrainValue(int input) {
  return map(input, 0, 255, 0, MAX_VAL);
}

// in_min～in_max内のxをout_min〜out_max内の値に変換して返す
uint32_t map(int x, int in_min, int in_max, int out_min, int out_max) {
  // restrict 'x' in 'in' range
  int x_in_range = in_min < in_max ? max(min(x, in_max), in_min) : max(min(x, in_min), in_max);
  return (x_in_range - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

String getValue(String data, char separator, int index) {
 int found = 0;
  int strIndex[] = { 0, -1 };
  int maxIndex = data.length()-1;
  for(int i=0; i<=maxIndex && found<=index; i++){
  if(data.charAt(i)==separator || i==maxIndex){
    found++;
    strIndex[0] = strIndex[1]+1;
    strIndex[1] = (i == maxIndex) ? i+1 : i;
  }
 }
  return found>index ? data.substring(strIndex[0], strIndex[1]) : "";
}

void updateLED() {
  uint32_t _r = constrainValue(i_color_r);
  uint32_t _g = constrainValue(i_color_g);
  uint32_t _b = constrainValue(i_color_b);
  uint32_t instrument_color = multiplicateBrightness(_r, _g, _b, instrument_brightness);
  int32_t instrument_color_alt = multiplicateBrightness(_r, _g, _b, instrument_brightness_alt);

  _r = constrainValue(e_color_r);
  _g = constrainValue(e_color_g);
  _b = constrainValue(e_color_b);
  uint32_t effect_color = multiplicateBrightness(_r, _g, _b, effect_brightness);
  uint32_t effect_color_base = multiplicateBrightness(_r, _g, _b, EFFECT_BRIGHTNESS_MIN);

  switch (i_type) {

    case INSTRUMENT_TYPE_DEFAULT:
      // control divide and slit_position from iOS
      {
        float unit = (float) TOTAL_PIXELS / (float) divide;
        int unit_number = 1;
        int unit_start = 0;
        for(int i=0; i<TOTAL_PIXELS;) {
          if(i<unit*unit_number){
            if(i<unit_start+unit*slit_position) {
              strip.setPixelColor((uint16_t) i, instrument_color);
            }
            else {
              strip.setPixelColor((uint16_t) i, effect_color);
            }
            i++;
          }
          else {
            unit_number++;
            unit_start = i;
          }
        }
      }
      break;

    case INSTRUMENT_TYPE_ONESHOT:
      {
        float unit = (float) TOTAL_PIXELS / 2.0;
        int unit_number = 1;
        int unit_start = 0;

        bool instrument_on = true;
        bool effect_on = true;

        int inst_divide = 5;
        float innerunit = unit / ((float) inst_divide * 2.0); // inner unit in instrument unit

        for(int i=0; i<TOTAL_PIXELS;) {

          if(i<unit*unit_number){

            if(i<unit_start+unit*0.5) {

              // instrument section

              float index_in_innerunit = ((float) i) - ((float) unit_start);
              int innerunit_index = (int)(index_in_innerunit / innerunit);

              bool condition = (innerunit_index%2==0);
              // if(instrument_flag){ condition = !condition; } // instrument_flag の値に応じて条件を反転させる
              uint32_t _color = condition ? instrument_color : instrument_color_alt;
              strip.setPixelColor((uint16_t) i, _color);
            }
            else {
              // effect section

              // 交互に色付く
              uint32_t _color = effect_on ? effect_color : effect_color_base;
              strip.setPixelColor((uint16_t) i, _color);
              effect_on = !effect_on;
            }
            i++;
          }
          else {
            unit_number++;
            unit_start = i;
          }
        }
      }
      break;

    case INSTRUMENT_TYPE_LONGSHOT:
      {
        divide = 5;
        slit_position = 0.5;
        float unit = (float) TOTAL_PIXELS / (float) divide;
        int unit_number = 1;
        int unit_start = 0;
        bool effect_on = true;
        for(int i=0; i<TOTAL_PIXELS;) {
          if(i<unit*unit_number){
            if(i<unit_start+unit*slit_position) {
              // instrument section
              strip.setPixelColor((uint16_t) i, instrument_color);
            }
            else {
              // effect section
              // 交互に色付く
              uint32_t _color = effect_on ? effect_color : effect_color_base;
              strip.setPixelColor((uint16_t) i, _color);
              effect_on = !effect_on;
            }
            i++;
          }
          else {
            unit_number++;
            unit_start = i;
          }
        }
      }
      break;

    default:
      Serial.println("[error] i_type is invalid");
      break;
  }
  strip.show();
}

uint32_t multiplicateBrightness(int _r, int _g, int _b, float _brightness) {
  int r = (int) _r*_brightness;
  int g = (int) _g*_brightness;
  int b = (int) _b*_brightness;
  return strip.Color(r, g, b);
}

// from Adafruit example

// Fill the dots one after the other with a color
void colorWipe(uint32_t c, uint8_t wait) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, c);
      strip.show();
      delay(wait);
  }
}

void rainbow(uint8_t wait) {
  uint16_t i, j;

  for(j=0; j<256; j++) {
    for(i=0; i<strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel((i+j) & 255));
    }
    strip.show();
    delay(wait);
  }
}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if(WheelPos < 85) {
   return strip.Color((WheelPos * 3)*MAX_VAL/255, (255 - WheelPos * 3)*MAX_VAL/255, 0);
  } else if(WheelPos < 170) {
   WheelPos -= 85;
   return strip.Color((255 - WheelPos * 3)*MAX_VAL/255, 0, (WheelPos * 3)*MAX_VAL/255);
  } else {
   WheelPos -= 170;
   return strip.Color(0, (WheelPos * 3)*MAX_VAL/255, (255 - WheelPos * 3)*MAX_VAL/255);
  }
}
