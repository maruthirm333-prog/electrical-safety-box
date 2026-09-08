# Electrical Safety Box ⚡

**ESP32-based smart electrical connection health monitor.**

Detects loose, corroded, or degraded electrical joints **before** they become dangerous — at 29 °C instead of 80 °C.

---

## The Core Idea

A loose screw terminal generates extra heat because its resistance is higher. But the heat depends on the current flowing through it — a loose joint at low current looks fine even to an infrared thermometer.

The fix: divide the temperature rise by the square of the current.

```
Health Index = ΔT ÷ I²

ΔT = (joint temperature) − (room temperature)
I  = current through the joint (measured by INA219)
```

This number stays **constant for a healthy connection**, regardless of load.

| Screw | Joint Temp | Looks dangerous? | Health Index | Box says |
|-------|-----------|-----------------|-------------|---------|
| Tight | 28.6 °C | ❌ No | 0.20 | ✅ Normal |
| ¼ turn loose | 29.8 °C | ❌ No | 0.59 | ⚠️ Early warning |
| ½ turn loose | 34 °C | ❌ No | 1.96 | ⚠️ Warning |
| 1 turn loose | 80 °C | ✅ Yes | 7.8 | 🔴 TRIP — CUT POWER |

**Read rows 2 and 3.** A thermometer sees nothing. The box sees the connection dying — weeks before it becomes dangerous.

---

## What It Does

- 📊 Calculates Health Index (ΔT ÷ I²) continuously
- ⚠️ Early warning when index drifts from baseline — connection still safe but degrading
- 🔴 Trips relay and cuts power when index crosses danger threshold
- 🔊 Buzzer + red LED on fault
- 📱 Phone dashboard showing health trend, current, temperatures
- 🔁 Reset only allowed after joint cools down
- 💾 Last 60 seconds of data saved to memory before trip

---

## Hardware

| # | Component | Qty |
|---|-----------|-----|
| 1 | ESP32 DevKit V1, 30-pin, headers pre-soldered | 1 |
| 2 | USB data cable (match your board — micro or USB-C) | 1 |
| 3 | INA219 current sensor module, I2C, headers pre-soldered | 1 |
| 4 | Relay module, 1-channel, 5V, opto-isolated | 1 |
| 5 | NTC thermistor 10k, **B3950**, glass bead | 3 |
| 6 | Resistor 10kΩ, 1% metal film | 10 |
| 7 | Resistor 220Ω, ¼W | 10 |
| 8 | Buzzer, 5V, active | 1 |
| 9 | LED 5mm red | 3 |
| 10 | LED 5mm green | 3 |
| 11 | Tactile push button, 6×6 mm | 5 |
| 12 | DC adapter 12V 3A, 5.5×2.1 mm barrel plug | 1 |
| 13 | Barrel jack female to screw terminal, 5.5×2.1 mm | 2 |
| 14 | Automotive bulb 12V 21W, BA15s | 2 |
| 15 | Automotive bulb 12V 10W, BA15s | 1 |
| 16 | BA15s bulb holder with leads | 3 |
| 17 | Screw terminal strip (choc block), 10A, 12-way | 2 |
| 18 | Hookup wire 0.75mm² stranded, red + black | 5 m |
| 19 | Breadboard, 830-point | 1 |
| 20 | Jumper wires M-M, 40 pc | 1 set |
| 21 | Jumper wires M-F, 40 pc | 1 set |
| 22 | Heat-shrink tube, assorted kit | 1 |
| 23 | Aluminium foil tape, 25 mm | 1 |
| 24 | Cable ties, 100 mm | 1 pack |
| 25 | Digital multimeter, DT830D or similar | 1 |
| 26 | Wire stripper / cutter | 1 |
| 27 | Screwdriver set, precision, with small flat blade | 1 |

**Total ≈ ₹2,890**

---

## Critical Rules Before Ordering

1. **"Headers pre-soldered"** — every module must say this. No soldering iron = loose pins = dead module.
2. **USB data cable** — not a charging cable. Charging cables cannot upload code.
3. **Load current never touches the breadboard** — breadboard strips handle ~1A, your load is 2.6A. Use screw terminals + thick wire for the 12V circuit.
4. **NTC must be B3950** — not B3435. The health index formula uses B=3950.
5. **Three bulbs minimum** — two current levels prove the health index stays constant. One bulb only proves the idea.

---

## NTC Calibration (Free, No Thermometer Needed)

- Ice water = exactly **0 °C**
- Boiling water = exactly **100 °C**

Two perfect reference points at zero cost.

---

## System Architecture

```
12V Adapter
    ↓
INA219 (measures current)
    ↓
Relay (ESP32 controls this — trips on fault)
    ↓
Screw Terminal Block ← NTC-1 taped here (joint temperature)
    ↓
Bulb Load
    ↓
Back to adapter (GND)

NTC-2 — hanging free in air (room temperature)
NTC-3 — spare / second joint

ESP32 — reads INA219 via I2C, reads NTCs via ADC,
         controls relay, buzzer, LEDs, serves dashboard
```

---

## Phone Dashboard (Planned)

```
Connection Health          ⚠ DEGRADING

Health index      0.59   (baseline 0.20)
Change            3.0×  worse
Trend             rising

Current           1.75 A
Socket            29.8 °C
Room              28.0 °C

Advice: This connection is deteriorating.
        It is safe now. Have it checked.

[ TURN OFF ]   [ SELF-TEST ]   [ HISTORY ]
```

---

## Project Status

- ✅ Concept validated — health index formula proven
- ✅ Component list finalised (₹2,890)
- 🚧 Hardware build in progress
- 📋 Firmware — coming after parts arrive
- 📋 Phone dashboard — coming after firmware

---

## By

Maruthi R M — ECE Student, Malnad College of Engineering, Hassan, Karnataka
[RuralSense Labs](https://github.com/maruthirm333-prog/ruralsense-labs)
