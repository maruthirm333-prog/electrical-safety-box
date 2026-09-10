# Electrical Safety Box ⚡

**Smart electrical connection health monitor — ESP32 + NTC + INA219.**

Detects loose, corroded, or degraded electrical joints **before** they become dangerous.
Speaks at **29.8 °C** while a thermometer stays silent until **80 °C**.

---

## The Core Idea

A loose screw terminal generates extra heat because its resistance is higher. But the heat depends on the current flowing — a loose joint at low current looks fine even to an infrared thermometer.

**The fix:** divide the temperature rise by the square of the current.

```
Health Index = ΔT ÷ I²

where ΔT = (joint temperature) − (room temperature)
      I  = current through the joint (amps)
```

This single number stays **constant for a healthy connection** regardless of load — and rises the moment the joint starts degrading.

---

## What It Looks Like in Practice

| Screw | Joint Temp | Looks dangerous? | Health Index | Box says |
|-------|-----------|-----------------|--------------|----------|
| Tight | 28.6 °C | ❌ No | 0.20 | ✅ Normal |
| ¼ turn loose | 29.8 °C | ❌ No | 0.59 | ⚠️ Early warning |
| ½ turn loose | 34.0 °C | ❌ No | 1.96 | ⚠️ Warning |
| 1 turn loose | 80.0 °C | ✅ Yes | 7.80 | 🔴 CUT POWER |

**Read rows 2 and 3.** That is the entire project.
A thermometer sees nothing. This box sees the connection dying.

---

## How It Works

```
12V Adapter → INA219 (current) → Relay → Screw Terminal → Bulb load
                                              ↑
                                    NTC-1 taped here

NTC-2 reads room temperature        ESP32 runs the health algorithm
```

### Alert Levels
- **Normal** — health index within 2× baseline → green LED blinks
- **Early Warning** — 2–5× baseline → amber alert, phone notification
- **Warning** — 5–10× baseline → buzzer, red LED, phone alert
- **TRIP** — >10× baseline → relay opens, power cut, buzzer, screen shows `JOINT FAILURE`

### Smart Reset
After a trip, the reset button is **locked** until the joint cools below a safe temperature. If the fault is not fixed, it will warn again within minutes of reset.

---

## Hardware

| # | Component | Qty |
|---|-----------|-----|
| 1 | ESP32 DevKit V1, 30-pin, headers pre-soldered | 1 |
| 2 | USB data cable (match your board: micro-USB or USB-C) | 1 |
| 3 | INA219 current sensor module, I2C, headers pre-soldered | 1 |
| 4 | Relay module, 1-channel, 5V, opto-isolated | 1 |
| 5 | NTC thermistor 10k B3950, glass bead | 3 |
| 6 | Resistor 10k 1% metal film | 10 |
| 7 | Resistor 220Ω ¼W | 10 |
| 8 | Buzzer 5V active | 1 |
| 9 | LED 5mm red | 3 |
| 10 | LED 5mm green | 3 |
| 11 | Tactile push button 6×6mm | 5 |
| 12 | DC adapter 12V 3A, 5.5×2.1mm barrel plug | 1 |
| 13 | Barrel jack female to screw terminal, 5.5×2.1mm | 2 |
| 14 | Automotive bulb 12V 21W, BA15s | 2 |
| 15 | Automotive bulb 12V 10W, BA15s | 1 |
| 16 | BA15s bulb holder with leads | 3 |
| 17 | Screw terminal strip (choc block), 10A, 12-way | 2 |
| 18 | Hookup wire 0.75mm² stranded, red and black | 5m |
| 19 | Breadboard 830-point | 1 |
| 20 | Jumper wires M-M, 40pc | 1 set |
| 21 | Jumper wires M-F, 40pc | 1 set |
| 22 | Heat-shrink tube assorted kit | 1 |
| 23 | Aluminium foil tape 25mm | 1 |
| 24 | Cable ties 100mm | 1 pack |
| 25 | Digital multimeter DT830D or similar | 1 |
| 26 | Wire stripper / cutter | 1 |
| 27 | Screwdriver set, precision, with small flat blade | 1 |

**Total ≈ ₹2,890**

→ Full BOM with ordering notes: [hardware/bom.md](hardware/bom.md)

---

## Critical Build Rules

1. **Headers must be pre-soldered.** No soldering iron = a module with loose pins in a bag is scrap.
2. **USB cable must be a data cable.** A charge-only cable powers the board but never uploads code.
3. **Load current never touches the breadboard.** Breadboard strips carry ~1A. Your load is 2.6A. Screw terminals and thick wire only.
4. **NTCs must be B3950, not B3435.** The calibration maths assumes 3950.
5. **Three bulbs, not one.** You need two current levels to prove ΔT ÷ I² stays constant.

---

## Libraries Required
- `Adafruit INA219`
- `Adafruit SSD1306` (if using OLED)
- `Adafruit GFX`
- `OneWire` / `DallasTemperature` (for NTC reading)

Install all before the parts arrive — so day one is building, not downloading.

---

## Project Status
🛒 Parts ordered
🔜 Firmware — coming after hardware arrives

---

## By
Maruthi R M — ECE Student, Malnad College of Engineering, Hassan, Karnataka
[RuralSense Labs](https://github.com/maruthirm333-prog/ruralsense-labs)
