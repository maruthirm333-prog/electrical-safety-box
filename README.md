# Electrical Safety Box ⚡

**Detect failing electrical connections before they cause fires — at 29.8°C, not 80°C.**

Most electrical fires start weeks before any visible sign. A loose screw terminal heats up slowly, resists normally, then one day arcs. This device watches the connection health index 24/7 and warns you long before anything looks dangerous.

---

## The Core Idea

A loose or corroding connection has higher resistance. Higher resistance = more heat for the same current.

But temperature alone is useless — a connection carrying 3A *should* be warmer than one carrying 0.5A.

So we normalise:

```
Health Index = ΔT ÷ I²

ΔT = temperature at connection − room temperature
I  = current through the connection (measured by INA219)
```

This one number stays constant when the connection is healthy — regardless of load.
When it starts rising, the joint is degrading.

### Real Example

| Screw tightness | Temp at joint | Looks dangerous? | Health Index | Device says |
|----------------|--------------|-----------------|-------------|-------------|
| Tight (baseline) | 28.6°C | no | 0.20 | ✅ Normal |
| ¼ turn loose | 29.8°C | **no** | 0.59 | ⚠️ Early warning — 3× worse |
| ½ turn loose | 34.0°C | **no** | 1.96 | ⚠️ Warning — 10× worse |
| 1 turn loose | 80.0°C | yes | 7.80 | 🔴 TRIPPED — power cut |

**The value of this device is rows 2 and 3.** Anyone can build something that reacts to 80°C. This one speaks at 29.8°C.

---

## How It Works

```
12V Adapter → INA219 → Relay → Screw Terminal Block → Load → back to adapter
                                       ↑
                               NTC-1 taped here (connection temp)

NTC-2 = room temperature reference
NTC-3 = spare / second connection point

ESP32 monitors everything, computes Health Index every 10 seconds
```

### Alert Levels

| Health Index | Condition | Action |
|-------------|-----------|--------|
| < 2× baseline | Normal | Green LED blinks, relay closed |
| 2–5× baseline | Early warning | Amber state, buzzer beep, phone alert |
| 5–10× baseline | Warning | Red LED, repeated buzzer |
| > 10× baseline | Critical | Relay opens (power cut), buzzer continuous |

### Reset Logic
After a trip, the reset button is locked until NTC-1 cools below threshold. The device will re-warn if the screw is still loose. A human must physically fix the connection.

---

## Hardware

| # | Component | Qty | Note |
|---|-----------|-----|------|
| 1 | ESP32 DevKit V1, 30-pin | 1 | Headers pre-soldered |
| 2 | USB data cable | 1 | Match your board (micro-USB or USB-C) |
| 3 | INA219 module, I2C | 1 | Headers pre-soldered |
| 4 | Relay module, 1-channel, 5V opto-isolated | 1 | |
| 5 | NTC thermistor 10k B3950, glass bead | 3 | Must be B3950, not B3435 |
| 6 | Resistor 10k, 1% metal film | 10 | Voltage divider for NTC |
| 7 | Resistor 220Ω, ¼W | 10 | LED current limiting |
| 8 | Buzzer, 5V active | 1 | |
| 9 | LED 5mm red | 3 | |
| 10 | LED 5mm green | 3 | |
| 11 | Tactile push button 6×6mm | 5 | |
| 12 | DC adapter 12V 3A, 5.5×2.1mm | 1 | |
| 13 | Barrel jack female to screw terminal | 2 | |
| 14 | Automotive bulb 12V 21W, BA15s | 2 | Load for testing |
| 15 | Automotive bulb 12V 10W, BA15s | 1 | Second current level |
| 16 | BA15s bulb holder with leads | 3 | |
| 17 | Screw terminal strip (choc block) 10A 12-way | 2 | The test connection point |
| 18 | Hookup wire 0.75mm² stranded, red + black | 5m | Load circuit only — no breadboard |
| 19 | Breadboard, 830-point | 1 | ESP32/sensor circuit only |
| 20 | Jumper wires M-M, 40pc | 1 set | |
| 21 | Jumper wires M-F, 40pc | 1 set | |
| 22 | Heat-shrink tube, assorted | 1 kit | |
| 23 | Aluminium foil tape, 25mm | 1 roll | Mount NTC to screw block |
| 24 | Cable ties, 100mm | 1 pack | |
| 25 | Digital multimeter DT830D | 1 | |
| 26 | Wire stripper / cutter | 1 | |
| 27 | Screwdriver set (precision, small flat blade) | 1 | |

**Total: ₹2,890**

> ⚠️ **Critical rules before ordering:**
> - All modules must say **"headers pre-soldered"** — no soldering iron available
> - USB cable must be a **data cable**, not a charging cable
> - Load current (up to 2.6A) must go through **screw terminals + thick wire**, never the breadboard
> - NTC must be **B3950** — the code maths assumes this beta value

---

## Three Bulbs — Why

You need at least two current levels to prove ΔT ÷ I² stays constant:

- One 21W bulb alone → I = 1.75A
- Both 21W bulbs → I = 3.50A
- 21W + 10W → I = 2.58A

If Health Index is the same across all three with the screw tight → the formula works.
If Health Index changes only when you loosen the screw → the device is doing its job.

---

## Calibration (Day 1 — before anything else)

Use ice water (0°C exact) and boiling water (100°C exact) to calibrate all 3 NTCs. No IR thermometer needed — these two reference points are free and more accurate than any ₹600 gun.

---

## Phone Dashboard (planned)

```
Connection Health          ⚠ DEGRADING

Health index      0.59   (baseline 0.20)
Change            3.0×  worse
Trend             rising

Current           1.75 A
Socket            29.8 °C
Room              28.0 °C

Advice: Deteriorating — safe now. Have it checked.

[ TURN OFF ]   [ SELF-TEST ]   [ HISTORY ]
```

---

## Software Setup — Do This Before Parts Arrive

1. Download [Arduino IDE](https://www.arduino.cc/en/software)
2. Add ESP32 board package (Espressif)
3. Install libraries: `Adafruit INA219`, `Adafruit SSD1306` (if OLED), `WiFi`, `Preferences`
4. Clone this repo — firmware will be here once hardware is assembled

---

## Status

- ✅ Concept validated, BOM finalised
- ✅ Core algorithm designed (ΔT ÷ I²)
- 🚧 Hardware assembly — parts being ordered
- 📋 Firmware — after assembly
- 📋 Phone dashboard — after firmware

---

By Maruthi R M — ECE Student, Malnad College of Engineering, Hassan, Karnataka
[RuralSense Labs](https://github.com/maruthirm333-prog/ruralsense-labs)
