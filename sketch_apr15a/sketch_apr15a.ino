#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

//////////////////////////////////////////////////
// WiFi
//////////////////////////////////////////////////

// const char* ssid = "TP-Link_3BE4";
// const char* password = "ninomiya317";

const char* ssid = "MI6-WLAN";
const char* password = "MI028WLAN";

const char* host = "172.27.56.24";

const int port = 8000;

WiFiUDP Udp;

//////////////////////////////////////////////////
// カラーセンサ
//////////////////////////////////////////////////

Adafruit_TCS34725 tcs =
Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

//////////////////////////////////////////////////

float smoothR = 0;
float smoothG = 0;
float smoothB = 0;

//////////////////////////////////////////////////

void setup() {

  M5.begin();

  Wire.begin();

  M5.Lcd.setRotation(1);

  M5.Lcd.setTextSize(2);

  if (!tcs.begin()) {

    M5.Lcd.println("Sensor NG");

    while (1);
  }

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    M5.Lcd.print(".");
  }

  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.println("COLOR M5");
  M5.Lcd.println(WiFi.localIP());
}

//////////////////////////////////////////////////

void loop() {

  uint16_t r_raw, g_raw, b_raw, c;

  tcs.getRawData(
    &r_raw,
    &g_raw,
    &b_raw,
    &c
  );

  if (c < 80) return;

  float sum =
    r_raw + g_raw + b_raw;

  if (sum == 0) sum = 1;

  int r = (r_raw / sum) * 255;
  int g = (g_raw / sum) * 255;
  int b = (b_raw / sum) * 255;

  smoothR = smoothR * 0.8 + r * 0.2;
  smoothG = smoothG * 0.8 + g * 0.2;
  smoothB = smoothB * 0.8 + b * 0.2;

  int finalR = smoothR;
  int finalG = smoothG;
  int finalB = smoothB;

  //////////////////////////////////////////////////
  // 送信
  //////////////////////////////////////////////////

  byte data[3] = {
    finalR,
    finalG,
    finalB
  };

  Udp.beginPacket(host, port);

  Udp.write(data, 3);

  Udp.endPacket();

  //////////////////////////////////////////////////

  M5.Lcd.fillRect(0,40,240,80,BLACK);

  M5.Lcd.setCursor(10,40);

  M5.Lcd.printf(
    "R:%d G:%d B:%d",
    finalR,
    finalG,
    finalB
  );

  delay(40);
}