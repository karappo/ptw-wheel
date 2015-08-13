#include <Adafruit_NeoPixel.h>
#define MAX_VAL 64  // 0 to 255 for brightness
#define DELAY_TIME 50 
#define DELAY_TIME2 20
 
// Parameter 1 = number of pixels in strip
// Parameter 2 = pin number (most are valid)
// Parameter 3 = pixel type flags, add together as needed:
//   NEO_RGB     Pixels are wired for RGB bitstream
//   NEO_GRB     Pixels are wired for GRB bitstream
//   NEO_KHZ400  400 KHz bitstream (e.g. FLORA pixels)
//   NEO_KHZ800  800 KHz bitstream (e.g. High Density LED strip)
Adafruit_NeoPixel strip = Adafruit_NeoPixel(60, 6, NEO_GRB + NEO_KHZ800);

char input[11];   // 文字列格納用
String input_str;   // 文字列格納用
int i = 0;  // 文字数のカウンタ
boolean serialAvailable = true;

void setup() {
  strip.begin();
  strip.show(); // Initialize all pixels to 'off'
  Serial.begin(9600);
  colorWipe(strip.Color(MAX_VAL, MAX_VAL, MAX_VAL), DELAY_TIME2); // White
  colorWipe(strip.Color(0, 0, 0), DELAY_TIME2); // White
}
 
void loop() {
  // Some example procedures showing how to display to the pixels:
//    colorWipe(strip.Color(0, 0, 0), DELAY_TIME); // Black
//  colorWipe(strip.Color(MAX_VAL, 0, 0), DELAY_TIME); // Red
//  colorWipe(strip.Color(0, MAX_VAL, 0), DELAY_TIME); // Green
//  colorWipe(strip.Color(MAX_VAL, MAX_VAL, 0), DELAY_TIME); // Yellow
//  colorWipe(strip.Color(0, 0, MAX_VAL), DELAY_TIME); // Blue
//  colorWipe(strip.Color(MAX_VAL, 0, MAX_VAL), DELAY_TIME); // Purple
//  colorWipe(strip.Color(0, MAX_VAL, MAX_VAL), DELAY_TIME); // Cyan
//  colorWipe(strip.Color(MAX_VAL, MAX_VAL, MAX_VAL), DELAY_TIME); // White
//  rainbow(DELAY_TIME2);
//  rainbowCycle(DELAY_TIME2);

  if (0<Serial.available()) {
    String str = Serial.readStringUntil('\n');
    int r = str.substring(0,3).toInt();
    int g = str.substring(4,7).toInt();
    int b = str.substring(8,11).toInt();
    Serial.print("R:");
    Serial.println(r);
    Serial.print("G:");
    Serial.println(g);
    Serial.print("B:");
    Serial.println(b);
    color(strip.Color(r, g, b));
  }
}

String getValue(String data, char separator, int index)
{
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

void color(uint32_t c) {
  for(uint16_t i=0; i<strip.numPixels(); i++) {
    strip.setPixelColor(i, c);
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
void rainbowCycle(uint8_t wait) {
  uint16_t i, j;
 
  for(j=0; j<256*5; j++) { // 5 cycles of all colors on wheel
    for(i=0; i< strip.numPixels(); i++) {
      strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
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
