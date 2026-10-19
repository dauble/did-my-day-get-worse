# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/) and the project uses [Semantic Versioning](https://semver.org/).

## [2.0.0] - 2026-10-19

### Changed
- All-healthy now shows a slow breathing wave that chases along the strip instead of a comet. Tune it with `CHASE_BREATH_MS`, `CHASE_WAVES`, `CHASE_MIN_BRIGHTNESS` and `CHASE_MAX_BRIGHTNESS` in `include/config.h`.
- Default `PULSE_MAX_BRIGHTNESS` lowered from 100 to 50.
- Cloudflare removed from the default service list; its status page almost always has a minor incident open, which kept the strip on amber.

### Fixed
- Polls now start every `POLL_INTERVAL_MS` (60 seconds) instead of waiting that long after each round finishes, which made rounds about 70 seconds apart.

### Removed
- `COMET_STEP_MS`, `COMET_FADE` and `COMET_BRIGHTNESS`; existing configs that set them must switch to the `CHASE_*` options.

## [1.3.0] - 2026-10-18

### Added
- All-healthy now shows a comet chasing along the strip instead of a pulse, tunable with `COMET_STEP_MS`, `COMET_FADE` and `COMET_BRIGHTNESS` in `include/config.h`.
- More services in the default list: Reddit, Zoom, DigitalOcean, Figma, Vercel, Netlify and npm.

### Changed
- All-healthy color is now sky blue, and the outage amber is a warmer yellow.
- Default `LED_COUNT` lowered from 30 to 17 and `PULSE_MAX_BRIGHTNESS` from 200 to 100.
- Discord and Atlassian removed from the default service list.
- FastLED pinned to the 3.9.x series (`~3.9.20`).

### Fixed
- The first poll after boot waits up to 10 seconds for WiFi, so it no longer reports a false ISP outage.

## [1.2.1] - 2026-10-17

### Changed
- Board link now carries the Amazon Associates tag, with the required disclosure in the README and pin-out doc.

## [1.2.0] - 2026-10-17

### Added
- `docs/PINOUT.md` with wiring and pin-selection notes for the ELEGOO ESP-WROOM-32 board.
- Product link for the recommended board in the README.

### Changed
- Default `LED_PIN` moved from GPIO 5 (a boot strapping pin) to GPIO 18.

## [1.1.1] - 2026-10-17

### Fixed
- An offline ISP now always shows red, even when fewer than `RED_THRESHOLD` services are configured.

## [1.1.0] - 2026-10-17

### Added
- Internet reachability check (`ISP_NAME`, `ISP_PROBES` in `include/config.h`) to report ISP outages such as Metronet.
  When the check fails, the ISP and all services count as down and the strip goes red.

## [1.0.0] - 2026-10-17

### Added
- ESP32 firmware (PlatformIO, Arduino framework) that polls user-defined service status pages.
- Atlassian Statuspage API support (`/api/v2/status.json`) for each monitored service.
- FastLED pulse animation: green (all healthy), yellow (1-2 services in outage), red (3 or more), blue (waiting for first result).
- `include/config.h` for LED hardware, thresholds, polling interval and the service list.
- `include/secrets.example.h` template for WiFi credentials (`secrets.h` is git-ignored).
- README with VSCode + PlatformIO setup, wiring and configuration instructions.
