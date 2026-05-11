#include <WiFi.h>
#include <WiFiUdp.h>
#include <FastLED.h>

//////////////////////////////////////////////////

#define LED_PIN 5
#define NUM_LEDS 60

CRGB leds[NUM_LEDS];

//////////////////////////////////////////////////
// WiFi
//////////////////////////////////////////////////

// const char* ssid = "TP-Link_3BE4";
// const char* password = "ninomiya317";

const char* ssid = "MI6-WLAN";
const char* password = "MI028WLAN";

WiFiUDP Udp;

const int port = 8000;

//////////////////////////////////////////////////
// RGB
//////////////////////////////////////////////////

int baseR = 100;
int baseG = 100;
int baseB = 255;

//////////////////////////////////////////////////

int currentMode = 0;

//////////////////////////////////////////////////

void setup() {

  Serial.begin(115200);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(
    leds,
    NUM_LEDS
  );

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {

    delay(500);
    Serial.print(".");
  }

  Serial.println(WiFi.localIP());

  Udp.begin(port);
}

//////////////////////////////////////////////////

void normalGlow() {

  for (int i = 0; i < NUM_LEDS; i++) {

    float wave =
      sin((millis() * 0.002) + (i * 0.15));

    int brightness =
      120 + (wave * 40);

    leds[i] = CRGB(
      (baseR * brightness) / 255,
      (baseG * brightness) / 255,
      (baseB * brightness) / 255
    );
  }
}

//////////////////////////////////////////////////

void riseEffect() {

  for (int i = 0; i < NUM_LEDS; i++) {

    leds[i] = CRGB(
      baseR,
      baseG,
      baseB
    );

    FastLED.show();

    delay(15);
  }
}

//////////////////////////////////////////////////

void sparkleEffect() {

  fadeToBlackBy(
    leds,
    NUM_LEDS,
    25
  );

  int pixel = random(NUM_LEDS);

  leds[pixel] = CRGB(
    baseR,
    baseG,
    baseB
  );
}

//////////////////////////////////////////////////

void loop() {

  int packetSize =
    Udp.parsePacket();

  if (packetSize >= 4) {

    byte data[4];

    Udp.read(data, 4);

    baseR = data[0];
    baseG = data[1];
    baseB = data[2];

    currentMode = data[3];
  }

  //////////////////////////////////////////////////

  if (currentMode == 0 ||
      currentMode == 1) {

    normalGlow();
  }

  //////////////////////////////////////////////////

  else if (currentMode == 2) {

    riseEffect();

    currentMode = 1;
  }

  //////////////////////////////////////////////////

  else if (currentMode == 3) {

    sparkleEffect();
  }

  //////////////////////////////////////////////////

  FastLED.show();

  delay(20);
}