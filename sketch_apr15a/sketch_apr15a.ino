#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiUdp.h>
#include <OSCMessage.h>
#include <Wire.h>
#include "Adafruit_TCS34725.h"

const char* ssid = "MI6-WLAN";
const char* password = "MI028WLAN";

const char* host = "172.27.28.51";
const int port = 8000;

WiFiUDP Udp;

// カラーセンサ
Adafruit_TCS34725 tcs = Adafruit_TCS34725(
  TCS34725_INTEGRATIONTIME_50MS,
  TCS34725_GAIN_4X
);

int sendCount = 0;

void setup() {
  M5.begin();
  Wire.begin();

  M5.Lcd.setRotation(1);
  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setTextSize(2);

  // センサチェック
  if (!tcs.begin()) {
    M5.Lcd.println("No sensor");
    while (1);
  }

  // WiFi接続
  M5.Lcd.setCursor(10, 0);
  M5.Lcd.println("Connecting...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    M5.Lcd.print(".");
    delay(500);
  }

  M5.Lcd.fillScreen(BLACK);
  M5.Lcd.setCursor(10, 0);
  M5.Lcd.println("WiFi OK");

  M5.Lcd.setCursor(10, 20);
  M5.Lcd.println(WiFi.localIP());
}

void loop() {
  uint16_t r, g, b, c;
  tcs.getRawData(&r, &g, &b, &c);

  // --- 色の正規化（ここが重要🔥） ---
  float sum = r + g + b;
  if (sum == 0) sum = 1;

  float rf = (r / sum) * 255;
  float gf = (g / sum) * 255;
  float bf = (b / sum) * 255;

  // --- 彩度強調 ---
  rf *= 1.5;
  gf *= 1.5;
  bf *= 1.5;

  // --- 制限 ---
  int red = constrain((int)rf, 0, 255);
  int green = constrain((int)gf, 0, 255);
  int blue = constrain((int)bf, 0, 255);

  // OSC送信
  OSCMessage msg("/color");
  msg.add(red);
  msg.add(green);
  msg.add(blue);

  Udp.beginPacket(host, port);
  msg.send(Udp);
  Udp.endPacket();
  msg.empty();

  sendCount++;

  // -------- 表示 --------
  M5.Lcd.fillRect(0, 50, 240, 100, BLACK);

  M5.Lcd.setCursor(10, 50);
  M5.Lcd.printf("R:%d G:%d B:%d", red, green, blue);

  M5.Lcd.setCursor(10, 80);
  M5.Lcd.printf("Send:%d", sendCount);

  // 色プレビュー
  uint16_t color = M5.Lcd.color565(red, green, blue);
  M5.Lcd.fillRect(10, 110, 200, 20, color);

  // 送信点滅
  M5.Lcd.fillCircle(220, 20, 5, GREEN);
  delay(10);
  M5.Lcd.fillCircle(220, 20, 5, BLACK);

  delay(50);
}