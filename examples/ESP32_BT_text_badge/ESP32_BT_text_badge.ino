// Load libraries
#include "BluetoothSerial.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_IS31FL3731.h>

// This is probably not done the best way it can but im not great at this, it does work though

// Type txt + message in a bluetooth serial monitor app to update the display
// Type br + a number between 0-255 to update the brightness

String Dname = "ESP32 - Badge"; // Bluetooth device name

// Check if Bluetooth configs are enabled
#if !defined(CONFIG_BT_ENABLED) || !defined(CONFIG_BLUEDROID_ENABLED)
#error Bluetooth is not enabled! Please run `make menuconfig` to and enable it
#endif

Adafruit_IS31FL3731_ScrPhHD matrix = Adafruit_IS31FL3731_ScrPhHD();

// Bluetooth Serial object
BluetoothSerial SerialBT;  

// Handle received and sent messages
int Brightness = 255;
String txt = "BOB";
String message = "";
String message2;
String message3;
String message4;
String SerialD;
bool swirl = false;
bool loopt = false;
// The lookup table to make the brightness changes be more visible
uint8_t sweep[] = {10, 20, 30, 40, 60, 80, 100, 150, 200, 255, 255, 200, 150, 100, 80, 60, 40, 30, 20, 10};

static const uint8_t PROGMEM
  smile_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10100101,
    B10011001,
    B01000010,
    B00111100 },
  frown_bmp[] =
  { B00111100,
    B01000010,
    B10100101,
    B10000001,
    B10011001,
    B10100101,
    B01000010,
    B00111100 },
  smile_love_bmp[] =
  {   0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
  0x00, 0x00, 0x00, 0x00, 0x00 };

void setup() {
  Serial.begin(115200);
  Wire.begin(13,12);
    if (! matrix.begin()) {
    Serial.println("IS31 not found");
    while (1); // loop
  }
  Serial.println("IS31 Found!");
  SerialBT.begin(Dname);
  Serial.print("The device started, now you can pair with bluetooth, Device name: ");
  Serial.println(Dname);
}

void loop() {
  // Read received messages
  unsigned long currentMillis = millis();
  if (SerialBT.available()){
    SerialD = SerialBT.readString();
    Serial.println(SerialD);
      message = SerialD;
      message3 = SerialD;
  }

    if(swirl) {
    for (uint8_t incr = 0; incr < 20; incr++)
      for (uint8_t x = 0; x < 17; x++)
        for (uint8_t y = 0; y < 9; y++)
          matrix.drawPixel(x, y, sweep[(x+y+incr)%20]);
    //delay(20);
    }

  // Check received message and control output accordingly
if (message3.length() > 1){
message3.remove(0,4);
  
if (message.indexOf("smile") >=0 ) {
  matrix.clear();
  matrix.drawBitmap(4, 0, smile_bmp, 8, 8, 255);
} else if (message.indexOf("br") >=0 ) {
 message4 = message;
 message4.remove(0,2);
 String b = message4;
 Brightness = b.toInt();
 message2 = "";
 Serial.println("Brightness set to: " + b);
 SerialBT.println("Brightness set to: " + b);
} else if (message.indexOf("txt") >=0 ){
    swirl = false;
    message4 = message;
    message4.remove(0,4);
    message4.trim();
    txt = message4;
    Serial.println("Display message set to: " + txt);
    SerialBT.println("Display message set to: " + txt);
} else if (message.indexOf("swirl") >=0 ){
    loopt = false;
    swirl = true;
    Serial.println("Display set to swirl");
    SerialBT.println("Display set to swirl");
}
}

  matrix.setRotation(0);
  matrix.setTextSize(0);
  matrix.setTextWrap(false);  // we dont want text to wrap so it scrolls nicely
  matrix.setTextColor(Brightness);
  
  if (message2 == txt) {} else {
  message2 = txt;
  matrix.clear();
  matrix.setCursor(0,0);
  if (message2.length() >= 4) {
    loopt = true;
  } else {
    loopt = false;
    matrix.print(message2);
  }
  }

if (loopt == true) {
  int leng = message2.length() * 7;
      for (int8_t x=18; x>=-leng; x--) {
      matrix.clear();
      matrix.setCursor(x,0);
      matrix.print(message2);
      delay(150);
      }
}

}
