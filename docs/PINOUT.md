# Pin-out and wiring

Target board: [ELEGOO ESP-WROOM-32 development board](https://www.amazon.com/dp/B0D8T53CQ5)
(ESP32-WROOM-32 module, USB-C, CP2102 USB-to-serial, 2.4 GHz WiFi). PlatformIO board: `esp32dev`.

> Pin names come from the ESP32-WROOM-32 module datasheet and the common 38-pin dev-board layout.
> ELEGOO's silkscreen labels may differ slightly, so match the labels printed on your board
> (`VIN`/`5V`, `GND`, `D18`/`GPIO18`) before powering up.

## LED strip wiring (WS2812B, 5 V)

| Strip wire | ESP32 board pin | Notes |
| --- | --- | --- |
| DIN (data) | **GPIO 18** (`LED_PIN`) | Through a 300–500 Ω resistor |
| GND | **GND** | Must be shared with the strip's power supply |
| 5V | **VIN** (5 V from USB) | Short strips only; for longer strips use an external 5 V supply |

```
USB-C ──► ESP32 ──GPIO18──[330 Ω]──► DIN ┐
           │ VIN (5 V) ───────────────► 5V ├─ WS2812B strip
           └ GND ─────────────────────► GND┘
                         1000 µF capacitor across 5V and GND at the strip
```

- The ESP32 outputs 3.3 V logic. WS2812B strips expect 5 V, but most work directly on short runs.
  If the first LEDs flicker, add a level shifter (e.g. 74AHCT125).
- The firmware caps current at `LED_MAX_MILLIAMPS` (500 mA) so USB power is safe. Use a separate 5 V supply
  for more or brighter LEDs, and tie its GND to the board's GND.
- Do not apply 5 V to the board's `3V3` pin.

## Choosing a different data pin

| Pins | Use for LEDs? | Reason |
| --- | --- | --- |
| 4, 13, 14, 16, 17, 18, 19, 21, 22, 23, 25, 26, 27, 32, 33 | Yes | General purpose |
| 0, 2, 5, 12, 15 | Avoid | Strapping pins, read at boot |
| 6–11 | Never | Connected to the module's flash |
| 34, 35, 36, 39 | Never | Input only |
| 1, 3 | Avoid | UART0 (USB serial / flashing) |

`LED_PIN` is set in `include/config.h`.
