# WS2812B controller for Arduino Nano

Arduino firmware for controlling a WS2812B strip with a KY-040-style rotary
encoder.

## Controls

- Rotate: change hue.
- Hold the encoder button and rotate: change brightness.
- Double-click: turn the strip on or off.

## Wiring

| Device | Pin | Arduino Nano |
| --- | --- | --- |
| WS2812B | DIN | D6 through a 330-470 ohm resistor |
| Encoder | CLK | D2 |
| Encoder | DT | D3 |
| Encoder | SW | D4 |
| Encoder | VCC | 5V |
| Encoder and strip | GND | Common GND |

Power the LED strip from a suitable external 5 V supply and connect its ground
to the Arduino ground. Set `LED_COUNT` in `firmware/firmware.ino` to the actual number of
LEDs before uploading.

## Build and upload

The project keeps Arduino CLI, board packages and libraries in local ignored
directories. Compile from the repository root with:

```powershell
.\.tools\arduino-cli\arduino-cli.exe compile --config-file .arduino\arduino-cli.yaml --fqbn arduino:avr:nano firmware
```

Upload after replacing `COM3` with the Nano port:

```powershell
.\.tools\arduino-cli\arduino-cli.exe upload --config-file .arduino\arduino-cli.yaml --fqbn arduino:avr:nano --port COM3 firmware
```
