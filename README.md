# Electrical Safety Box ⚡

**ESP32-based electrical connection health monitor.**

Detects failing electrical connections — loose screws, corroded joints, bad contacts — **weeks before they become dangerous.** Not by waiting for 80°C. By detecting the problem at 29.8°C, when it still looks completely normal.

> "Anyone can build a device that reacts to 80°C. This one speaks at 29.8°C."

---

## The Core Idea — ΔT ÷ I²

A loose electrical connection has higher resistance than a tight one. Higher resistance means more heat for the same current — but the temperature rise also depends on *how much current is flowing*, which changes every time you plug in a different appliance.

So instead of measuring raw temperature, we measure:

```
Health Index = ΔT ÷ I²

where:
  ΔT = connection temperature − room temperature
  I  = current through the connection (measured by INA219)
```

This number stays **constant** for a healthy connection regardless of load:

| Load | Current | ΔT | Health Index |
|------|---------|-----|--------------|
| 21W bulb | 1.75A | 0.6°C | **0.20** |
| 21W + 10W | 2.58A | 1.3°C | **0.20** |

The number is identical. That is the baseline — what "healthy" looks like.

---

## What Happens as a Connection Loosens

| Screw tightness | NTC-1 temp | Looks dangerous? | Health Index | Box says |
|----------------|-----------|-----------------|--------------|----------|
| Tight | 28.6°C | ❌ No | 0.20 | ✅ Normal |
| ¼ turn loose | 29.8°C | ❌ No | 0.59 | ⚠️ Early warning |
| ½ turn loose | 34.0°C | ❌ No | 1.96 | ⚠️ Warning |
| 1 turn loose | 80.0°C | ✅ Yes | 7.80 | 🔴 TRIPPED |

**Read rows 2 and 3.** The connection is failing. A thermometer sees nothing. This box detects it.

---

## System Overview

```
12V Adapter → INA219 (current sensor) → Relay → Screw Block → Bulb → back to adapter
                                                      ↑
                                               NTC-1 taped here (connection temp)

NTC-2 — room temperature reference
NTC-3 — spare / second connection point
ESP32 — reads all sensors, decides, alerts, logs
```

### What it does
- Reads current (INA219 via I2C) every second
- Reads connection temp (NTC-1) and room temp (NTC-2) every second
- Computes Health Index = ΔT ÷ I²
- Learns baseline in first 10 minutes of operation
- Raises early warning when index is 2× baseline
- Trips relay and sounds buzzer when index is 5× baseline
- Refuses reset until temperature drops to safe level
- Saves last 60 seconds of data before trip
- Sends alert to phone (via Wi-Fi when available)

### Phone dashboard
```
Connection Health          ⚠ DEGRADING

Health index      0.59   (baseline 0.20)
Change            3.0×  worse
Trend             rising for 6 days

Current           1.75 A
Socket            29.8 °C
Room              28.0 °C

Advice: This connection is deteriorating.
        It is safe now. Have it checked.

[ TURN OFF ]   [ SELF-TEST ]   [ HISTORY ]
```

---

## Hardware

| # | Component | Qty |
|---|-----------|-----|
| 1 | ESP32 DevKit V1 (headers pre-soldered) | 1 |
| 2 | USB data cable (match board type) | 1 |
| 3 | INA219 current sensor module (I2C, pre-soldered) | 1 |
| 4 | Relay module, 1-ch, 5V, opto-isolated | 1 |
| 5 | NTC thermistor 10k B3950 | 3 |
| 6 | Resistor 10k 1% metal film | 10 |
| 7 | Resistor 220Ω ¼W | 10 |
| 8 | Buzzer 5V active | 1 |
| 9 | LED 5mm red | 3 |
| 10 | LED 5mm green | 3 |
| 11 | Tactile push button 6×6mm | 5 |
| 12 | DC adapter 12V 3A (5.5×2.1mm) | 1 |
| 13 | Barrel jack female to screw terminal | 2 |
| 14 | Automotive bulb 12V 21W BA15s | 2 |
| 15 | Automotive bulb 12V 10W BA15s | 1 |
| 16 | BA15s bulb holder with leads | 3 |
| 17 | Screw terminal strip 10A 12-way | 2 |
| 18 | Hookup wire 0.75mm² stranded, red + black | 5m |
| 19 | Breadboard 830-point | 1 |
| 20 | Jumper wires M-M 40pc | 1 set |
| 21 | Jumper wires M-F 40pc | 1 set |
| 22 | Heat-shrink tube assorted | 1 kit |
| 23 | Aluminium foil tape 25mm | 1 roll |
| 24 | Cable ties 100mm | 1 pack |
| 25 | Digital multimeter DT830D | 1 |
| 26 | Wire stripper/cutter | 1 |
| 27 | Screwdriver set (precision, small flat blade) | 1 |

**Total ≈ ₹2,890**

> ⚠️ Critical buying rules:
> - All modules must say **"headers pre-soldered"** — no soldering iron available
> - USB cable must be a **data cable**, not a charging cable
> - Load current (2.6A) must **never touch the breadboard** — screw terminals only
> - NTC must be **B3950**, not B3435 — code maths depends on this
> - Buy **3 bulbs** (two current levels) — one bulb proves the idea, three prove the physics

---

## Why 12V, not mains?

Building and testing at 12V DC is safe — no electrocution risk, no RCCB required, no licensed electrician needed. The physics (resistance → heat → ΔT/I²) is identical to mains voltage. The demonstration is fully valid. Scale to mains with appropriate safety hardware when productising.

---

## Build Checklist (before parts arrive)

- [ ] Download Arduino IDE
- [ ] Install ESP32 board package in Arduino IDE
- [ ] Install libraries: `Adafruit INA219`, `Adafruit SSD1306` (if OLED), `Wire`, `WiFi`
- [ ] Set up Arduino IDE → Tools → Board → ESP32 Dev Module

---

## Project Status
💡 Parts ordered — build starting soon

## By
Maruthi R M — ECE Student, Malnad College of Engineering, Hassan, Karnataka
[RuralSense Labs](https://github.com/maruthirm333-prog/ruralsense-labs)
