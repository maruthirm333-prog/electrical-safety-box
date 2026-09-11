# Wiring Guide

## Golden Rule
**Load current (up to 2.6A) never touches the breadboard.**
Breadboard strips are rated ~1A. Your load is 2.6A. Use screw terminals + 0.75mm² wire for everything at 12V.

## Circuit Overview

```
12V Adapter
    │
    └─→ INA219 VIN+
             │
         INA219 VIN−
             │
         Relay COM
             │
         Relay NO ──→ Screw Terminal Block ──→ Bulb(s) ──→ 12V GND
```

Everything from ESP32 and sensors runs at 3.3V/5V on the breadboard. Only the 12V bulb circuit uses screw terminals.

## ESP32 Pin Assignments

| Pin | Connected to | Note |
|-----|-------------|------|
| GPIO 21 (SDA) | INA219 SDA | I2C |
| GPIO 22 (SCL) | INA219 SCL | I2C |
| GPIO 34 | NTC-1 voltage divider | Connection temp (ADC input) |
| GPIO 35 | NTC-2 voltage divider | Room temp (ADC input) |
| GPIO 32 | NTC-3 voltage divider | Spare (ADC input) |
| GPIO 25 | Relay IN | Relay control |
| GPIO 26 | Buzzer | Active buzzer |
| GPIO 27 | Red LED (via 220Ω) | Alert indicator |
| GPIO 14 | Green LED (via 220Ω) | Normal indicator |
| GPIO 12 | Reset button | INPUT_PULLUP |

## NTC Voltage Divider (per NTC)
```
3.3V
 │
10kΩ (1% metal film)
 │
 ├──→ GPIO 34/35/32 (ADC)
 │
NTC 10k B3950
 │
GND
```

## NTC Mounting
- NTC-1: tape flat against screw terminal block using aluminium foil tape — thermal contact is critical
- NTC-2: hang freely in air near the board — room reference, must NOT touch anything warm
- NTC-3: spare

## INA219 Wiring
```
ESP32 3.3V → INA219 VCC
ESP32 GND  → INA219 GND
ESP32 SDA  → INA219 SDA
ESP32 SCL  → INA219 SCL

12V line (from adapter) → INA219 VIN+
12V line (to relay)     → INA219 VIN−
```
INA219 is in series with the load — all current passes through it.

## Relay Wiring
```
ESP32 3.3V → Relay VCC
ESP32 GND  → Relay GND
ESP32 GPIO25 → Relay IN

Relay COM → INA219 VIN−
Relay NO  → Screw terminal block (load side)
```
