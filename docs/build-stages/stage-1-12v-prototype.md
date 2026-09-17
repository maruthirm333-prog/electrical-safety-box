# Stage 1 — 12V Low-Voltage Prototype

**Goal:** Prove current sensing, relay control, and basic software logic before touching 230V mains.

> Build the 12V version completely first. Prove it works. Then scale to mains.

---

## What to Build in Stage 1

**Components needed (only these):**
- ESP32 DevKit V1
- INA219 module
- Relay module (1-channel, 5V opto-isolated)
- 12V 3A adapter
- 12V 10W bulb (BA15s) — start with this, NOT the 21W
- BA15s bulb holder
- Breadboard + jumper wires

**Do NOT connect yet:**
- ❌ NTC thermistors
- ❌ 230V mains plug/socket
- ❌ Mains fuse
- ❌ Hairdryer, iron, or any 230V appliance
- ❌ 21W bulbs (too high current for first test)

---

## Circuit — 12V Load Loop

```
12V Adapter (+)
      │
      ▼
 INA219 VIN+
 INA219 VIN−
      │
      ▼
 Relay COM
 Relay NO
      │
      ▼
 Bulb (+)
 Bulb (−)
      │
      ▼
12V Adapter (−)
```

All 12V current flows through INA219 (so it can measure it) then through the relay (so ESP32 can cut it), then through the bulb.

---

## Wiring — INA219 → ESP32

| INA219 Pin | ESP32 Pin | Note |
|-----------|-----------|------|
| VCC | 3.3V | Power |
| GND | GND | Ground |
| SDA | GPIO 21 | I2C data |
| SCL | GPIO 22 | I2C clock |

INA219 is in series with the 12V load. All current passes through it.

---

## Wiring — Relay → ESP32

| Relay Pin | ESP32 Pin | Note |
|-----------|-----------|------|
| VCC | 5V (VIN) | Relay coil power |
| GND | GND | Ground |
| IN | GPIO 26 | Control signal |

> ⚠️ **Test relay logic first.** Relay modules differ:
> - Some: `IN = HIGH` → relay closes (load ON)
> - Some: `IN = LOW` → relay closes (load ON) — inverted logic
>
> Upload a simple sketch that toggles GPIO 26 HIGH/LOW every 2 seconds.
> Watch the relay click and the bulb ON/OFF.
> Note which state (HIGH or LOW) turns the bulb ON — use that in all future code.

---

## Stage 1 Expected Output

When relay is ON:
```
12V → INA219 → relay → bulb → LIGHT ON 💡
Serial Monitor: "Current: ~0.83A  Load: ON"
```

When relay is OFF:
```
Bulb OFF
Serial Monitor: "Current: 0.00A  Load: OFF"
```

10W bulb at 12V = 10 ÷ 12 ≈ **0.83A** — this is the expected INA219 reading.

---

## Stage 1 Test Code

```cpp
#include <Wire.h>
#include <Adafruit_INA219.h>

Adafruit_INA219 ina219;

#define RELAY_PIN 26

void setup() {
  Serial.begin(115200);
  pinMode(RELAY_PIN, OUTPUT);
  digitalWrite(RELAY_PIN, LOW); // start with relay OFF

  if (!ina219.begin()) {
    Serial.println("INA219 not found — check wiring");
    while (1);
  }
  Serial.println("INA219 ready.");
}

void loop() {
  // Turn relay ON
  digitalWrite(RELAY_PIN, HIGH); // change to LOW if your relay is inverted
  Serial.println("Relay ON — bulb should light");
  delay(3000);

  float mA = ina219.getCurrent_mA();
  float V  = ina219.getBusVoltage_V();
  Serial.print("Voltage: "); Serial.print(V);  Serial.print(" V  |  ");
  Serial.print("Current: "); Serial.print(mA / 1000.0, 3); Serial.println(" A");

  // Turn relay OFF
  digitalWrite(RELAY_PIN, LOW); // change to HIGH if your relay is inverted
  Serial.println("Relay OFF — bulb should be dark");
  delay(3000);

  Serial.println("---");
}
```

**Libraries needed:**
- `Adafruit INA219` (search in Arduino Library Manager)
- `Adafruit BusIO` (installs automatically with INA219)

---

## Stage 1 Pass Criteria

| Check | Expected |
|-------|---------|
| Relay clicks when GPIO 26 changes | ✅ audible click |
| Bulb lights when relay closes | ✅ 12V bulb ON |
| INA219 reads ~0.83A with 10W bulb | ✅ ±0.1A acceptable |
| INA219 reads ~0.00A when relay open | ✅ |
| Serial Monitor shows clean values | ✅ |

All five pass → Stage 1 complete → move to Stage 2 (add NTC thermistors).

---

## What Stage 2 Adds

- NTC-1 taped to screw terminal block (connection temperature)
- NTC-2 hanging free (room temperature reference)
- Compute ΔT = NTC1 − NTC2
- Compute Health Index = ΔT ÷ I²
- Establish baseline with screw tight
- Detect degradation when screw loosened

→ See [algorithm.md](../algorithm.md) for full formula and worked numbers.
