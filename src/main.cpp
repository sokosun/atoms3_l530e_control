#include <M5AtomS3.h>
#include "L530EControl.h"
#include "ArduinoJson.h"
#define GPIO_BUTTON 41

// Please modify login information
L530EControl l530e(IPAddress(192,168,1,49), "username@mail.com", "password");

volatile bool g_irq0 = false;
volatile unsigned long g_time = 0;
void setIRQ0() {
  g_irq0 = true;
  g_time = millis();
}

void setup() {
  M5.begin(false, true, false, true); // Enable Serial and RGB LED
  M5.dis.clear();
  M5.dis.show();

  WiFi.begin("SSID", "PASSWORD");
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
  USBSerial.print("WiFi connected\r\n");
 
  pinMode(GPIO_BUTTON, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(GPIO_BUTTON), setIRQ0, FALLING);
}

void loop() {
  static int brightness = 10;
  static int hue = 0;

  if(!g_irq0){
    return;
  }
  M5.dis.drawpix(CRGB::Aqua);
  M5.dis.show();

  bool press_and_hold = true;
  while(millis() - g_time < 500){
    if(digitalRead(GPIO_BUTTON)){
      press_and_hold = false;
      break;
    }
    delay(100);
  }

  if(!l530e.HasActiveSession()){
    l530e.Handshake();
    l530e.Login();
  }

  if(press_and_hold){
    const int bstep = 10;
    while(1){
      l530e.SetDeviceInfo(true, L530EControl::HSB( -1, -1, brightness));
      brightness = brightness + bstep > 100 ? brightness + bstep - 100 : brightness + bstep;

      if(digitalRead(GPIO_BUTTON)){
        break;
      }
      delay(500);
    }
  }else{
    const int hstep = 60;
    l530e.SetDeviceInfo(true, L530EControl::HSB(hue, 100));
    hue = hue + hstep >= 360 ? hue + hstep - 360 : hue + hstep;
  }

  delay(100);
  M5.dis.clear();
  M5.dis.show();
  g_irq0 = false;
}