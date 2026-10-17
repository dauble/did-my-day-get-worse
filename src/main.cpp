// Services status LED strip.
//
// A background task polls each service's status page and counts the ones in
// outage. The main loop pulses the whole strip: green (all healthy), yellow
// (some in outage) or red (RED_THRESHOLD or more). Blue means "no result yet".

#include <Arduino.h>
#include <ArduinoJson.h>
#include <FastLED.h>
#include <HTTPClient.h>
#include <WiFi.h>
#include <WiFiClientSecure.h>

#include "config.h"
#include "secrets.h"

constexpr int NO_RESULT = -1;

static CRGB leds[LED_COUNT];
static volatile int outageCount = NO_RESULT; // written by pollTask, read by loop()

// Returns true when the status page reports indicator "none" (all operational).
static bool isHealthy(const Service &service) {
  WiFiClientSecure client;
  client.setInsecure(); // Public, non-sensitive data; skips certificate management.

  HTTPClient http;
  http.useHTTP10(true); // Avoids chunked encoding so the body can be streamed to the parser.
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (!http.begin(client, service.url)) return false;

  bool healthy = false;
  if (http.GET() == HTTP_CODE_OK) {
    JsonDocument filter;
    filter["status"]["indicator"] = true;
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter))) {
      healthy = strcmp(doc["status"]["indicator"] | "unknown", "none") == 0;
    }
  }
  http.end();
  return healthy;
}

// Polls every service forever; a lost WiFi connection counts every service as down.
static void pollTask(void *) {
  for (;;) {
    int down = 0;
    for (const Service &service : SERVICES) {
      const bool healthy = WiFi.status() == WL_CONNECTED && isHealthy(service);
      Serial.printf("%-12s %s\n", service.name, healthy ? "OK" : "OUTAGE");
      if (!healthy) down++;
    }
    outageCount = down;
    vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
  }
}

static CRGB colorFor(int outages) {
  if (outages == NO_RESULT) return CRGB::Blue;
  if (outages == 0) return CRGB::Green;
  if (outages < RED_THRESHOLD) return CRGB(255, 140, 0); // Amber; pure yellow looks green on WS2812.
  return CRGB::Red;
}

void setup() {
  Serial.begin(115200);
  Serial.printf("Services status LED v%s\n", FW_VERSION);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_MAX_MILLIAMPS);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  xTaskCreate(pollTask, "poll", 12288, nullptr, 1, nullptr);
}

void loop() {
  fill_solid(leds, LED_COUNT, colorFor(outageCount));
  FastLED.setBrightness(beatsin8(PULSE_BPM, PULSE_MIN_BRIGHTNESS, PULSE_MAX_BRIGHTNESS));
  FastLED.show();
  FastLED.delay(10);
}
