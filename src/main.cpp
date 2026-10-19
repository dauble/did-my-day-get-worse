// Services status LED strip.
//
// A background task checks internet reachability, then each service's status
// page, and counts the ones in outage. All healthy shows a green comet chasing along the strip;
// otherwise the whole strip pulses yellow (some in outage) or red (RED_THRESHOLD or more).
// Blue means "no result yet".

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
static bool isHealthy(const Service &service)
{
  WiFiClientSecure client;
  client.setInsecure(); // Public, non-sensitive data; skips certificate management.

  HTTPClient http;
  http.useHTTP10(true); // Avoids chunked encoding so the body can be streamed to the parser.
  http.setTimeout(HTTP_TIMEOUT_MS);
  http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
  if (!http.begin(client, service.url))
    return false;

  bool healthy = false;
  if (http.GET() == HTTP_CODE_OK)
  {
    JsonDocument filter;
    filter["status"]["indicator"] = true;
    JsonDocument doc;
    if (!deserializeJson(doc, http.getStream(), DeserializationOption::Filter(filter)))
    {
      healthy = strcmp(doc["status"]["indicator"] | "unknown", "none") == 0;
    }
  }
  http.end();
  return healthy;
}

// Returns true when WiFi is up and any probe URL answers HTTP 204.
static bool isOnline()
{
  if (WiFi.status() != WL_CONNECTED)
    return false;
  for (const char *url : ISP_PROBES)
  {
    HTTPClient http;
    http.setTimeout(HTTP_TIMEOUT_MS);
    if (!http.begin(url))
      continue;
    const int code = http.GET();
    http.end();
    if (code == HTTP_CODE_NO_CONTENT)
      return true;
  }
  return false;
}

// Polls forever. An offline ISP is always red, however few services are configured.
static void pollTask(void *)
{
  // Give WiFi a chance to connect so the first poll doesn't report a false ISP outage.
  for (int i = 0; i < 20 && WiFi.status() != WL_CONNECTED; i++)
    vTaskDelay(pdMS_TO_TICKS(500));

  for (;;)
  {
    int down = 0;
    const bool online = isOnline();
    Serial.printf("%-12s %s\n", ISP_NAME, online ? "OK" : "OUTAGE");
    if (!online)
    {
      down = RED_THRESHOLD;
    }
    else
    {
      for (const Service &service : SERVICES)
      {
        const bool healthy = isHealthy(service);
        Serial.printf("%-12s %s\n", service.name, healthy ? "OK" : "OUTAGE");
        if (!healthy)
          down++;
      }
    }
    outageCount = down;
    vTaskDelay(pdMS_TO_TICKS(POLL_INTERVAL_MS));
  }
}

static CRGB colorFor(int outages)
{
  Serial.printf("Num outages: %d\n", outages);

  if (outages == NO_RESULT)
    return CRGB(0, 0, 255);
  if (outages == 0)
    return CRGB(0, 201, 255);
  if (outages < RED_THRESHOLD)
    return CRGB(255, 187, 0); // Amber; pure yellow looks green on WS2812.

  return CRGB(255, 0, 0);
}

void setup()
{
  Serial.begin(115200);
  Serial.printf("Services status LED v%s\n", FW_VERSION);

  FastLED.addLeds<WS2812B, LED_PIN, GRB>(leds, LED_COUNT);
  FastLED.setMaxPowerInVoltsAndMilliamps(5, LED_MAX_MILLIAMPS);

  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  xTaskCreate(pollTask, "poll", 12288, nullptr, 1, nullptr);
}

// Moves a bright head one LED per step, leaving a fading tail behind it.
static void drawComet(const CRGB &color)
{
  static uint16_t head = 0;
  EVERY_N_MILLISECONDS(COMET_STEP_MS)
  {
    fadeToBlackBy(leds, LED_COUNT, COMET_FADE);
    leds[head] = color;
    head = (head + 1) % LED_COUNT;
  }
  FastLED.setBrightness(COMET_BRIGHTNESS);
}

static void drawPulse(const CRGB &color)
{
  fill_solid(leds, LED_COUNT, color);
  FastLED.setBrightness(beatsin8(PULSE_BPM, PULSE_MIN_BRIGHTNESS, PULSE_MAX_BRIGHTNESS));
}

void loop()
{
  const int outages = outageCount;
  if (outages == 0)
    drawComet(colorFor(outages));
  else
    drawPulse(colorFor(outages));
  FastLED.show();
  FastLED.delay(10);
}
