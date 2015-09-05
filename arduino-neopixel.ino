#include <Adafruit_NeoPixel.h>

// Arduino Nano v2.x, v3.x

#define TOTAL_PIXELS 99

// common settings :::::::::

#define MAX_VAL 64 // 0 to 255 for brightness 多くのLEDを制御した場合に、色が安定しない問題を解決するために設けた制限
#define DELAY_TIME 10
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(TOTAL_PIXELS, 6, NEO_GRB + NEO_KHZ800);

char input[11];   // 文字列格納用
String input_str;   // 文字列格納用
int i = 0;  // 文字数のカウンタ

uint32_t instrument_color = 0;
uint32_t effect_color = 0;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);

  // startup animation
  rainbow(2);
  colorWipe(0, DELAY_TIME);
}

//void loop() {
//    if (0<Serial.available()) {
//      // 255,255,255 255,255,255
//      String str = Serial.readStringUntil('\n');
//      int inst_r = str.substring(0,3).toInt();
//      int inst_g = str.substring(4,7).toInt();
//      int inst_b = str.substring(8,11).toInt();
//  
//      // あまり強く光らせると色が安定しないのでリミットを設ける
//      int _inst_r = constrainValue(inst_r);
//      int _inst_g = constrainValue(inst_g);
//      int _inst_b = constrainValue(inst_b);
//      Serial.println("::: setColor :::");
//      Serial.println("instrument:["+String(inst_r)+","+String(inst_g)+","+String(inst_b)+"] constrains to ["+String(_inst_r)+","+String(_inst_g)+","+String(_inst_b)+"]");
//      
//      instrument_color = strip.Color(_inst_r, _inst_g, _inst_b);
//      updateLED();
//    }
//}

void loop() {
  if (0<Serial.available()) {
    String command = Serial.readStringUntil(':');
    String argument = Serial.readStringUntil(';');
    if(command=="0") {
      Serial.println(command);
      if(argument.length()==11) {
        // argument likes "255.255.255"
        int r = constrainValue(argument.substring(0,3).toInt());
        int g = constrainValue(argument.substring(4,7).toInt());
        int b = constrainValue(argument.substring(8,11).toInt());
        instrument_color = strip.Color(r, g, b);
        updateLED();
      }
      else {
        Serial.print("Invalid argument:");
        Serial.println(argument);
      }
    }
    else if(command=="1") {
      if(argument.length()==11) {
        // argument likes "255.255.255"
        int r = constrainValue(argument.substring(0,3).toInt());
        int g = constrainValue(argument.substring(4,7).toInt());
        int b = constrainValue(argument.substring(8,11).toInt());
        effect_color = strip.Color(r, g, b);
        updateLED();
      }
      else {
        Serial.print("Invalid argument:");
        Serial.println(argument);
      }
    }
    else {
      Serial.print("Command not found:");
      Serial.println(command);
    }
  }
}

// limitation of brightness
// 0-255の値を0-MAX_VALの範囲に
uint32_t constrainValue(int input) {
  return float(input) / 255.0 * float(MAX_VAL);
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
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    if(i<TOTAL_PIXELS/4){
      strip.setPixelColor(i, instrument_color);
    }
    else if(i<TOTAL_PIXELS/2){
      strip.setPixelColor(i, effect_color);
    }
    else if(i<TOTAL_PIXELS*3/4){
      strip.setPixelColor(i, instrument_color);
    }
    else {
      strip.setPixelColor(i, effect_color);
    }
  }
  strip.show();
}

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
 
// Slightly different, this makes the rainbow equally distributed throughout
//void rainbowCycle(uint8_t wait) {
//  uint16_t i, j;
// 
//  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
//    for(i=0; i< strip.numPixels(); i++) {
//      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
//    }
//    strip.show();
//    delay(wait);
//  }
//}
 
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
