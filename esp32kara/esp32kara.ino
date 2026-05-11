#include <WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>

#define LED_PIN 5
#define NUM_LEDS 6

CRGB leds[NUM_LEDS];

const char* ssid = "MI6-WLAN";
const char* password = "MI028WLAN";

// const char* ssid = "TP-Link_3BE4";
// const char* password = "ninomiya317";

WiFiUDP Udp;
const int port = 8000;

void setup() {
  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, NUM_LEDS);

  Serial.println("Connecting...");
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("");
  Serial.println("WiFi Connected!");

  Serial.print("ESP32 IP: ");
  Serial.println(WiFi.localIP());

  Udp.begin(port);
  Serial.println("UDP ready");
}

void loop() {
  int packetSize = Udp.parsePacket();

  if (packetSize >= 3) {
    byte data[3];
    Udp.read(data, 3);

    int r = data[0];
    int g = data[1];
    int b = data[2];

    for (int i = 0; i < NUM_LEDS; i++) {
      leds[i] = CRGB(r, g, b);
    }

    FastLED.show();

    Serial.printf("R:%d G:%d B:%d\n", r, g, b);
  }
}