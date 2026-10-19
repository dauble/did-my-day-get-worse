# Services Status LED

An ESP32 + WS2812B LED strip that shows the overall health of the online services you depend on,
inspired by the network-status light in [I Like To Make Stuff's video](https://www.youtube.com/watch?v=WWTxgRnQeJ4) (from 10:47).
When everything is healthy a sky-blue breathing wave chases along the strip; otherwise the whole strip pulses ("breathes") in one colour:

| Services in outage | Colour |
| --- | --- |
| 0 | Sky blue (chasing breath) |
| 1–2 | Yellow (amber) |
| 3 or more | Red |
| No result yet (booting / first poll) | Blue |

Every 60 seconds the ESP32 first checks that the internet is reachable (your ISP, `ISP_NAME` in
`include/config.h`, e.g. Metronet), then fetches each service's status page. A service counts as in outage when
its status indicator is anything other than `none` or when the page can't be fetched.

If the internet check fails (WiFi down, or no probe URL answers HTTP 204), the strip goes red without polling
the services. Red is also shown whenever `RED_THRESHOLD` or more services are in outage.

> The request said "one or three services" for yellow; this implements **1–2 = yellow, 3+ = red**,
> matching the red rule. Change `RED_THRESHOLD` in `include/config.h` if you want something else.

## Hardware

- ESP32 dev board, e.g. the [ELEGOO ESP-WROOM-32 3-pack](https://www.amazon.com/dp/B0D8T53CQ5?tag=dauble-20) (any `esp32dev`-compatible board works)
- WS2812B / NeoPixel LED strip (5 V)
- 5 V power supply (for long strips, power the strip directly and share ground with the ESP32)

| Strip | ESP32 |
| --- | --- |
| DIN | GPIO 18 (`LED_PIN`) |
| GND | GND |
| 5V | 5V / VIN (or external supply) |

See [docs/PINOUT.md](docs/PINOUT.md) for the full pin-out and pin-selection notes.
A 300–500 Ω resistor on the data line and a 1000 µF capacitor across the strip's power are recommended.

_As an Amazon Associate I earn from qualifying purchases._

## Setup (VSCode + PlatformIO)

1. Install [VSCode](https://code.visualstudio.com/) and the **PlatformIO IDE** extension (`platformio.platformio-ide`).
   Restart VSCode when prompted.
2. Open this folder in VSCode (**File → Open Folder**). PlatformIO detects `platformio.ini` and downloads the
   ESP32 platform, FastLED and ArduinoJson on first load.
3. Copy `include/secrets.example.h` to `include/secrets.h` and enter your WiFi name and password.
4. Edit `include/config.h`:
   - `LED_PIN`, `LED_COUNT` for your strip.
   - `SERVICES` — your list of services (see below).
5. Plug in the ESP32 over USB. On Windows you may need the
   [CP210x](https://www.silabs.com/developers/usb-to-uart-bridge-vcp-drivers) or CH340 driver.
6. In the PlatformIO toolbar at the bottom of VSCode:
   - **✓ Build** to compile
   - **→ Upload** to flash (hold the board's `BOOT` button during "Connecting…" if it fails)
   - **🔌 Serial Monitor** to see per-service results (115200 baud)

CLI equivalent: `pio run -t upload && pio device monitor`.

## Adding services

Add a line to `SERVICES` in `include/config.h`:

```cpp
{"GitHub", "https://www.githubstatus.com/api/v2/status.json"},
```

The URL must be the service's status page API endpoint. Most status pages are hosted on
[Atlassian Statuspage](https://www.atlassian.com/software/statuspage); for those, append
`/api/v2/status.json` to the status page address and open it in a browser to confirm you get JSON like
`{"status":{"indicator":"none", ...}}`. Services on other platforms (e.g. AWS, Google Cloud) are not supported.

## ISP reachability

ISPs like Metronet don't publish a machine-readable status page, so the firmware probes the internet directly.
`ISP_PROBES` in `include/config.h` lists plain-HTTP "generate_204" URLs; the connection is considered up when any
one returns HTTP 204. Set `ISP_NAME` to label it in the serial log.

## Notes

- HTTPS certificates are not verified (`setInsecure()`); the data is public and read-only.
- The strip is capped at `LED_MAX_MILLIAMPS` (500 mA) so a USB port can power it; raise it for larger strips with a proper supply.
- Pulse speed and brightness range are `PULSE_BPM`, `PULSE_MIN_BRIGHTNESS`, `PULSE_MAX_BRIGHTNESS`.
- Chase speed, band count and brightness range are `CHASE_BREATH_MS`, `CHASE_WAVES`, `CHASE_MIN_BRIGHTNESS`, `CHASE_MAX_BRIGHTNESS`.
- Each service is polled one after another; keep the list modest (each fetch may take a second or two).
