# Connection Health Algorithm

## The Formula

```
Health Index = ΔT ÷ I²

ΔT = NTC_connection_temp − NTC_room_temp   (°C)
I  = INA219 current reading                (Amperes)
```

## Why This Works

A loose or corroded connection has increased contact resistance (Rc).

Power dissipated at the joint:
```
P = I² × Rc
```

That power becomes heat. So temperature rise at the joint:
```
ΔT ∝ I² × Rc
```

Therefore:
```
Rc ∝ ΔT ÷ I²
```

Dividing by I² removes the load dependency. The result is proportional to contact resistance only.

## Baseline Learning

On first run (or after reset), the device runs for 10 minutes with the screw tight to establish baseline:
```
baseline = average(ΔT ÷ I²) over 10 minutes
```

Stored in ESP32 non-volatile memory (Preferences library).

## Alert Thresholds

| Multiplier vs baseline | State |
|-----------------------|-------|
| < 2× | Normal |
| 2–5× | Early Warning |
| 5–10× | Warning |
| > 10× | Critical → trip relay |

## Worked Numbers

### Tight screw, one 21W bulb (I = 1.75A)
```
NTC_connection = 28.6°C
NTC_room       = 28.0°C
ΔT             = 0.6°C
I²             = 3.06
Health Index   = 0.6 ÷ 3.06 = 0.20  ← baseline
```

### Tight screw, both bulbs (I = 2.58A) — proves load independence
```
ΔT           = 1.3°C
I²           = 6.66
Health Index = 1.3 ÷ 6.66 = 0.20  ← same number, different load ✅
```

### Quarter-turn loose (I = 1.75A)
```
ΔT           = 1.8°C
Health Index = 1.8 ÷ 3.06 = 0.59
Ratio        = 0.59 ÷ 0.20 = 3.0×  → EARLY WARNING
```

### Half-turn loose
```
ΔT           = 6.0°C
Health Index = 6.0 ÷ 3.06 = 1.96
Ratio        = 9.8×  → WARNING
```

### Full turn loose, both bulbs (I = 2.58A)
```
ΔT           = 52°C
Health Index = 52 ÷ 6.66 = 7.8
Ratio        = 39×  → TRIP + CUT POWER
```

## NTC Temperature Calculation

Using Steinhart-Hart simplified (Beta equation):
```
1/T = 1/T0 + (1/B) × ln(R/R0)

T0 = 298.15 K  (25°C reference)
R0 = 10,000 Ω  (NTC nominal at 25°C)
B  = 3950       (B3950 thermistor)
R  = R_fixed × (Vcc − Vadc) / Vadc
```

R_fixed = 10kΩ 1% resistor in voltage divider with NTC.
