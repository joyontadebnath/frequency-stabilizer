# ⚡ Frequency Stabilizer — Variable AC to Fixed Frequency AC

<div align="center">

![Course](https://img.shields.io/badge/Course-EEE_316-blue)
![Simulation](https://img.shields.io/badge/Simulation-Proteus-orange)
![Firmware](https://img.shields.io/badge/Firmware-Arduino-teal)
![Hardware](https://img.shields.io/badge/Implementation-Breadboard-yellow)

**A power electronics system that converts variable-frequency AC input into a stable fixed-frequency AC output using a rectifier, SG3524-controlled half-bridge inverter, RC filter, and Arduino-based real-time frequency monitor.**

</div>

---


## 📖 About

Variable-frequency AC is common in renewable energy systems (wind, solar), unstable grids, and generator outputs. This project builds a hardware frequency stabilizer that accepts any variable-frequency AC input and delivers a clean, stable 50 Hz (or adjustable) sinusoidal AC output — with no digital signal processing or microcontroller in the power path.

The system follows a four-stage pipeline entirely in analog hardware, with an Arduino added only as a passive monitoring unit for the LCD display.

---

## 🏗️ System Architecture

```
Variable         ┌─────────────┐    ┌─────────────────────┐    ┌──────────────┐    Stabilized
Frequency AC ───▶│  Rectifier  │───▶│  Bridge            │───▶│  3rd-Order   │───▶ 50 Hz AC
                 │ Full-Bridge │    │  Inverter           │    │  RC Filter   │
                 │  4× Diodes  │    │  2× IRF250 MOSFET   │    │  R=5kΩ       │
                 │  + Cap      │    │  + Optocoupler      │    │  C=0.47µF    │
                 └─────────────┘    └──────────┬──────────┘    └──────────────┘
                      9V DC                    │
                                    ┌──────────▼──────────┐
                                    │  SG3524 Controller  │
                                    │  PWM Pulse Generator│
                                    │  Rt=55.3kΩ Ct=0.47µF│
                                    │  2× Potentiometers  │
                                    └─────────────────────┘
                                               │
                                    ┌──────────▼──────────┐
                                    │  Arduino Uno + LCD  │
                                    │  Real-time Monitor  │
                                    │  Pins 7 & 8 input   │
                                    └─────────────────────┘
```

---

## ⚙️ How It Works

### Stage 1 — Rectification

A **full-bridge diode rectifier** with a smoothing capacitor converts the variable-frequency AC input into steady DC. The output frequency becomes 0 Hz — all frequency information from the input is discarded at this stage.

| Parameter | Value |
|---|---|
| Input | 20V p-p sinusoidal, variable frequency |
| Output | ~9V DC (steady) |
| Diode forward drop | 0.7V × 2 = 1.4V |
| Theoretical output | (10 − 1.4) V ≈ 8.6V ≈ 9V DC |

### Stage 2 — SG3524 PWM Controller

The **SG3524 regulating pulse-width modulator IC** generates two alternating 180°-phase-shifted PWM signals to drive the half-bridge inverter. The output frequency is set by an external RC timing network and adjustable via potentiometer.

**Frequency formula:**

$$f_c = \frac{1.3}{R_t \times C_t} = \frac{1.3}{55.3\,\text{k} \times 0.47\,\mu\text{F}} = 50\,\text{Hz}$$

Two potentiometers provide independent control of:
- **Frequency** (potentiometer 1 — adjusts Rt)
- **Duty cycle** (potentiometer 2 — set to 50% for symmetric output)

An **optocoupler** isolates the controller ground from the inverter ground, protecting the low-voltage control circuit from the high-voltage power stage.

### Stage 3 — Bridge Inverter

Two **IRF250 MOSFETs** switch alternately under the SG3524 control pulses to convert the 9V DC back into AC at the desired frequency.

| Parameter | Value |
|---|---|
| Input | 9V DC |
| Output | 8V p-p square wave |
| Output frequency | 50 Hz (set by controller) |
| Note | Half-bridge loses half the voltage vs full-bridge |

### Stage 4 — 3rd-Order RC Low-Pass Filter

A three-stage RC ladder filter attenuates harmonics above the fundamental frequency, converting the square wave into an approximated sinusoid.

**Cutoff frequency formula:**

$$f_c = \frac{1}{2\pi R C} = \frac{1}{2\pi \times 5\,\text{k}\Omega \times 0.47\,\mu\text{F}} = 67.72\,\text{Hz}$$

The cutoff is intentionally set above 60 Hz to support both 50 Hz and 60 Hz output configurations.

**Transfer function at 50 Hz:**

$$|H(\omega)| = \frac{1}{\sqrt{1 + \left(\frac{w}{w_c}\right)^6}} = \frac{1}{\sqrt{1 + \left(\frac{314.15}{420.97}\right)^6}} = 0.92$$

| Filter stage | Output |
|---|---|
| After 1st RC | Sawtooth-like wave |
| After 2nd RC | Near-sinusoidal (usable) |
| After 3rd RC | Clean sinusoid at 50 Hz, ~4V p-p |

### Stage 5 — Arduino Frequency Monitor (Firmware)

An **Arduino Uno** passively monitors the input and output frequencies and displays them on a **16×2 I2C LCD**. It does not participate in the power path — monitoring only.

- Pin 8: rectified input signal (before inverter)
- Pin 7: rectified output signal (after filter)
- Uses `pulseIn()` to measure high and low pulse widths and compute frequency

---

## 💡 Simulation

The full circuit is simulated in **Proteus Design Suite**.

### Opening the Simulation

1. Install **Proteus** (version 8.x or later)
2. Open `Freq_Stable.pdsprj`
3. Run the simulation
4. Adjust the virtual potentiometers to change frequency and duty cycle
5. Use the virtual oscilloscope probes to observe waveforms at each stage

---

## 🚀 How to Use the Hardware

**Step 1 — Power the controller:**
Connect a 9–10V DC source to the SG3524 IC supply. Connect a 5–6V DC source to the optocoupler.

**Step 2 — Set duty cycle:**
Monitor SG3524 pin 12 on an oscilloscope. Adjust potentiometer 1 until the duty cycle reads 50%.

**Step 3 — Set output frequency:**
Use potentiometer 2 to tune the output to your desired frequency (50 Hz or 60 Hz). Verify on the oscilloscope at the filter output or read from the LCD display.

**Step 4 — Apply AC input:**
Connect the variable-frequency AC source to the rectifier input. Observe that the output frequency remains stable regardless of the input frequency.

**Step 5 — Monitor:**
The LCD displays both input and output frequencies in real time. The output should remain near the set frequency while the input can vary freely.

---

## 🔌 Component List

| Component | Part Number | Qty | Function |
|---|---|---|---|
| MOSFET | IRF250 | 4 | Half-bridge inverter switches |
| Rectifier Diode | General purpose | 8 | Full-bridge rectifier |
| PWM Controller | SG3524 / IC3524 | 2 | Frequency and duty cycle control |
| Optocoupler | General purpose | 3 | Ground isolation |
| Potentiometer | 100kΩ | 2 | Frequency + duty cycle adjustment |
| LCD Display | 16×2 character | 1 | Real-time frequency readout |
| I2C Display Module | PCF8574 | 1 | LCD interface |
| Microcontroller | Arduino Uno | 1 | Frequency measurement and display |
| Breadboard | — | 4 | Circuit assembly |
| Capacitors | 0.47µF, 35µF | Various | Timing, smoothing, filtering |
| Resistors | 1kΩ, 5kΩ, 55.3kΩ | Various | Timing, current limiting, filter |
| Jumper Wires | — | Various | Connections |

**Total project cost: ~2,515 BDT (≈ USD 23)**

---

## ⚠️ Known Limitations

- **Voltage loss:** The half-bridge inverter outputs only half the DC bus voltage. Using a full-bridge inverter or a step-up transformer after the filter would recover this.
- **Waveform quality:** The 3rd-order RC filter produces a reasonable sinusoid but not a pure one — THD is measurable. An LC filter would achieve better waveform quality with less voltage loss.
- **Manual tuning:** Frequency adjustment requires manual potentiometer turning. There is no automatic frequency locking.
- **Arduino blocking:** `pulseIn()` is a blocking call with a 1-second timeout. At very low input frequencies, the display update can lag significantly.
- **Low output amplitude:** 4V p-p output from the filter. A step-up transformer is needed for mains-level voltage applications.
- **potentiometer wear:** Mechanical potentiometers are prone to degradation over time and imprecise manual setting.

---

## 👥 Team

| Name | Contribution |
|---|---|
| **Joyonta Debnath** | Hardware, testing, debugging, perfection |
| **Md. Maksudur Rahman Turzo** |  Theory, simulation, hardware, testing |
| **Ahamad Abtahi** | Hardware, components, testing, presentation |
| **Anas Rohan** | Simulation, hardware, theory, resources, testing |
| **Mahadi Zaman** | Theory, hardware, testing |
| **Shamiul Islam Anik** | Hardware, components, testing |
| **Md. Nazmus Shakib Tushar** | Hardware, simulation, components, testing |

---

## 🎓 Academic Context

| Field | Detail |
|---|---|
| Course | EEE 316 — Power Electronics Laboratory |
| Department | Electrical & Electronic Engineering |
| University | Bangladesh University of Engineering and Technology (BUET) |
| Section / Group | A2 / Group 04 |

---

## 📄 License

This project was submitted as an academic assignment at BUET. Free to use for educational purposes with attribution.

---

<div align="center">

Built with breadboards, MOSFETs, and a lot of oscilloscope debugging at BUET 🎓

</div>
