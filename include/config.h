// User-editable settings: LED hardware, thresholds, timing and the services to monitor.
#pragma once

#include <Arduino.h>

constexpr char FW_VERSION[] = "2.0.0";

// --- LED strip ---------------------------------------------------------------
#define LED_PIN 18                          // ESP32 GPIO wired to the strip's data line
constexpr uint16_t LED_COUNT = 17;          // Number of LEDs on the strip
constexpr uint16_t LED_MAX_MILLIAMPS = 500; // Power cap; keeps a USB supply safe
constexpr uint8_t PULSE_BPM = 20;           // Pulses per minute
constexpr uint8_t PULSE_MIN_BRIGHTNESS = 15;
constexpr uint8_t PULSE_MAX_BRIGHTNESS = 50;
// All-healthy (sky blue) shows a breathing wave chasing along the strip instead of pulsing.
constexpr uint16_t CHASE_BREATH_MS = 5000; // Time for each LED to breathe in and out; higher = slower
constexpr uint8_t CHASE_WAVES = 1;         // Number of bright bands along the strip at once
constexpr uint8_t CHASE_MIN_BRIGHTNESS = 10;
constexpr uint8_t CHASE_MAX_BRIGHTNESS = 50;

// --- Health thresholds -------------------------------------------------------
// Services in outage: 0 = sky blue, 1..(RED_THRESHOLD-1) = yellow, >= RED_THRESHOLD = red.
constexpr uint8_t RED_THRESHOLD = 3;

// --- Polling -----------------------------------------------------------------
constexpr uint32_t POLL_INTERVAL_MS = 60UL * 1000;
constexpr uint16_t HTTP_TIMEOUT_MS = 8000;

// --- Internet (ISP) reachability ---------------------------------------------
// The ISP is checked first. It is online when any probe URL answers HTTP 204
// (plain HTTP, so captive portals and error pages don't count). When it is offline
// every service is unreachable, so the strip goes red without polling them,
// regardless of RED_THRESHOLD or how many services are listed.
constexpr char ISP_NAME[] = "Metronet";
constexpr const char *ISP_PROBES[] = {
    "http://connectivitycheck.gstatic.com/generate_204",
    "http://cp.cloudflare.com/generate_204",
};

// --- Services ----------------------------------------------------------------
// Each entry needs a status page that exposes the Atlassian Statuspage API, i.e.
// "<status page>/api/v2/status.json". A service counts as in outage when its
// indicator is anything other than "none" or when the page cannot be fetched.
struct Service
{
    const char *name;
    const char *url;
};

constexpr Service SERVICES[] = {
    {"GitHub", "https://www.githubstatus.com/api/v2/status.json"},
    {"Claude", "https://status.claude.com/api/v2/status.json"},
    {"Reddit", "https://www.redditstatus.com/api/v2/status.json"},
    {"Zoom", "https://status.zoom.us/api/v2/status.json"},
    // {"Cloudflare", "https://www.cloudflarestatus.com/api/v2/status.json"},
    {"DigitalOcean", "https://status.digitalocean.com/api/v2/status.json"},
    {"Figma", "https://status.figma.com/api/v2/status.json"},
    {"Vercel", "https://www.vercel-status.com/api/v2/status.json"},
    {"Netlify", "https://www.netlifystatus.com/api/v2/status.json"},
    {"npm", "https://status.npmjs.org/api/v2/status.json"},
};
constexpr size_t SERVICE_COUNT = sizeof(SERVICES) / sizeof(SERVICES[0]);
