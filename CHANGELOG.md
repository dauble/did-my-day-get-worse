# Changelog

All notable changes to this project are documented here.
The format follows [Keep a Changelog](https://keepachangelog.com/) and the project uses [Semantic Versioning](https://semver.org/).

## [1.0.0] - 2026-10-17

### Added
- ESP32 firmware (PlatformIO, Arduino framework) that polls user-defined service status pages.
- Atlassian Statuspage API support (`/api/v2/status.json`) for each monitored service.
- FastLED pulse animation: green (all healthy), yellow (1-2 services in outage), red (3 or more), blue (waiting for first result).
- `include/config.h` for LED hardware, thresholds, polling interval and the service list.
- `include/secrets.example.h` template for WiFi credentials (`secrets.h` is git-ignored).
- README with VSCode + PlatformIO setup, wiring and configuration instructions.
