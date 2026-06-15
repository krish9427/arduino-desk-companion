# Arduino Desk Companion Box 

An interactive, gesture-controlled desktop dashboard built using an Arduino Uno, an ADXL345 accelerometer, and a 16x2 I2C LCD screen. This device features a physical master toggle switch to preserve battery life and uses physical shaking motions to flip between three productivity modes.

---

## 🛠️ Features

The companion box operates on three dedicated software pages, completely managed by physical gestures:

1. **Focus Timer Mode:** A multi-hour deep work countdown timer. Add hours, pause/play your session, or reset it completely using dedicated desk buttons.
2. **Water Tracker Log:** An interactive water intake counter that helps you stay hydrated throughout the day by logging your target daily goal.
3. **Desk Temperature Monitor:** Tracks your ambient workspace temperature using the internal hardware registers of the Arduino microcontroller chip.

---

## 🔌 Hardware Circuit Diagram & Setup

The device is completely portable, powered by a 3.8V Lithium-Ion battery wired to bypass the internal voltage regulator for maximum energy efficiency.

### Wiring Reference:

| Component | Arduino Pin | Notes |
| :--- | :--- | :--- |
| **ADXL345 SDA** | A4 (SDA) | I2C Data Line |
| **ADXL345 SCL** | A5 (SCL) | I2C Clock Line |
| **16x2 LCD SDA** | A4 (SDA) | Shared I2C Bus |
| **16x2 LCD SCL** | A5 (SCL) | Shared I2C Bus |
| **Button 1** | Digital Pin 7 | Action / Add / Log Button |
| **Button 2** | Digital Pin 8 | Play / Pause Toggle Button |
| **Button 3** | Digital Pin 9 | Reset / Clear Button |
| **Buzzer (+)** | Digital Pin 10 | Audio Alert System |

### Power & Switch Configuration:
* **Battery GND (Black Wire):** Connects to **Arduino GND**.
* **Battery VCC (Red Wire):** Connects to **Switch Terminal 1**.
* **Switch Center Terminal:** Connects to **Arduino 5V Pin**.

> ⚠️ **Safety Notice:** Always flip the physical battery toggle switch to the **OFF** position before connecting a USB cable to update the microcontroller's firmware.

---

## 🎮 How To Use

| Intended Action | Physical Gesture / Input |
| :--- | :--- |
| **Change Active Mode** | Pick up the box and shake it horizontally side-to-side. |
| **Interact with Menu** | Use the 3 physical tactile buttons mounted on the box. |
| **Power On / Off** | Flip the mechanical switch mounted inline with the battery line. |

---

## 📦 Required Libraries

To compile the provided source code, ensure you have installed the following libraries inside your Arduino IDE:
* `Wire.h` (Built-in)
* `LiquidCrystal_I2C` by Frank de Brabander
* `Adafruit_Sensor` by Adafruit
* `Adafruit_ADXL345_U` by Adafruit

---

## 📝 License
This project is open-source and free to use for hobbyists and educational makers!
