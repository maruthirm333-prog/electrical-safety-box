# Bill of Materials — Electrical Safety Box

**Total ≈ ₹2,890** (all essential items)

---

## Core Electronics

| # | Item | Qty | Notes |
|---|------|-----|-------|
| 1 | ESP32 DevKit V1, 30-pin | 1 | **Headers pre-soldered** — critical |
| 2 | USB data cable | 1 | Match your board: micro-USB or USB-C. Must be data cable, not charge-only |
| 3 | INA219 module, I2C | 1 | **Headers pre-soldered** — critical |
| 4 | Relay module, 1-channel, 5V, opto-isolated | 1 | Opto-isolated = ESP32 GPIO safe |

## Sensors

| # | Item | Qty | Notes |
|---|------|-----|-------|
| 5 | NTC thermistor 10k, **B3950**, glass bead | 3 | Must be B3950 — code calibration assumes this |
| 6 | Resistor 10k, 1% metal film | 10 | 1% tolerance — not 5% |
| 7 | Resistor 220Ω, ¼W | 10 | For LEDs |

## Alerts

| # | Item | Qty | Notes |
|---|------|-----|-------|
| 8 | Buzzer, 5V, active | 1 | Active = buzzes on HIGH, no tone code needed |
| 9 | LED 5mm red | 3 | |
| 10 | LED 5mm green | 3 | |
| 11 | Tactile push button, 6×6mm | 5 | |

## 12V Power Circuit

| # | Item | Qty | Notes |
|---|------|-----|-------|
| 12 | DC adapter 12V 3A, 5.5×2.1mm barrel | 1 | 3A minimum — two 21W bulbs = 3.5A peak |
| 13 | Barrel jack female to screw terminal | 2 | Adapts barrel plug to wires |
| 14 | Automotive bulb 12V 21W, BA15s | 2 | Test loads — different wattages prove ΔT÷I² is constant |
| 15 | Automotive bulb 12V 10W, BA15s | 1 | |
| 16 | BA15s bulb holder with leads | 3 | One per bulb |
| 17 | Screw terminal strip (choc block), 10A, 12-way | 2 | **This is the experiment** — the joint you intentionally loosen |
| 18 | Hookup wire 0.75mm² stranded, red + black | 5m | Thick wire — load current never touches breadboard |

## Consumables

| # | Item | Qty | Notes |
|---|------|-----|-------|
| 19 | Breadboard, 830-point | 1 | For ESP32 + sensors only — not load circuit |
| 20 | Jumper wires M-M, 40pc | 1 set | |
| 21 | Jumper wires M-F, 40pc | 1 set | |
| 22 | Heat-shrink tube, assorted kit | 1 | |
| 23 | Aluminium foil tape, 25mm | 1 | Improves NTC thermal contact to terminals |
| 24 | Cable ties, 100mm | 1 pack | |

## Tools

| # | Item | Qty | Notes |
|---|------|-----|-------|
| 25 | Digital multimeter, DT830D or similar | 1 | Verify voltages before powering up |
| 26 | Wire stripper / cutter | 1 | |
| 27 | Screwdriver set, precision, with small flat blade | 1 | For screw terminals |

---

## What You Already Have (Buy Nothing)

| Item | Why it works |
|------|-------------|
| Laptop | Upload code, Serial Monitor |
| Phone | Dashboard / alerts |
| Ice water (0 °C) | NTC calibration reference — more accurate than a ₹600 IR gun |
| Boiling water (100 °C) | NTC calibration reference |
| Notebook | Log baseline readings |

---

## What Is Deliberately Not On This List

| Item | Why excluded |
|------|-------------|
| IR thermometer | Ice + boiling water are exact references — IR gun adds cost with less accuracy |
| Soldering iron | Not needed — everything runs at 12V, no fine SMD work |
| Enclosure | Prototype phase — enclose after firmware is finalised |
| OLED display | Optional — Serial Monitor is enough to prove the concept |
| RCCB | Not applicable at 12V |
