#include <M5StickCPlus.h>
#include <WiFi.h>
#include <WiFiUdp.h>


// WiFi
// const char* ssid = "TP-Link_3BE4";
// const char* password = "ninomiya317";

const char* ssid = "MI6-WLAN";
const char* password = "MI028WLAN";

// ESP32 IP
const char* espHost = "172.27.56.15";

const int port = 8000;

WiFiUDP Udp;


// RGB
int baseR = 255;
int baseG = 100;
int baseB = 50;


// 加速度
float ax, ay, az;
float prevMagnitude = 0;


float smallThreshold = 0.08;
float bigThreshold = 0.35;


int mode = 0;


unsigned long lastMovementTime = 0;


void setup() {

  M5.begin();

  M5.Imu.Init();

  M5.Lcd.setRotation(1);

  M5.Lcd.setTextSize(2);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    M5.Lcd.print(".");
  }

  Udp.begin(port);

  M5.Lcd.fillScreen(BLACK);

  M5.Lcd.println("MOTION M5");

  M5.Lcd.println(WiFi.localIP());

  lastMovementTime = millis();
}


void loop() {

  // RGB受信
  int packetSize = Udp.parsePacket();

  if (packetSize >= 3) {

    byte colorData[3];

    Udp.read(colorData, 3);

    baseR = colorData[0];
    baseG = colorData[1];
    baseB = colorData[2];
  }


  // 加速度
  M5.Imu.getAccelData(&ax, &ay, &az);

  float magnitude =
    sqrt(ax * ax + ay * ay + az * az);

  float diff =
    abs(magnitude - prevMagnitude);

  prevMagnitude = magnitude;


  // mode判定
  mode = 0;

  if (diff > bigThreshold) {

    mode = 2;

    lastMovementTime = millis();
  }

  else if (diff > smallThreshold) {

    mode = 1;

    lastMovementTime = millis();
  }

  if (millis() - lastMovementTime > 3000) {

    mode = 3;
  }


  // ESP32へ送信
  byte sendData[4] = {
    baseR,
    baseG,
    baseB,
    mode
  };

  Udp.beginPacket(espHost, port);

  Udp.write(sendData, 4);

  Udp.endPacket();
  

  M5.Lcd.fillRect(0,40,240,100,BLACK);

  M5.Lcd.setCursor(10,40);

  M5.Lcd.printf(
    "R:%d G:%d B:%d",
    baseR,
    baseG,
    baseB
  );

  M5.Lcd.setCursor(10,80);

  M5.Lcd.printf(
    "Mode:%d",
    mode
  );

  delay(30);
}