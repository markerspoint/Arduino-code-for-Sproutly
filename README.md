# 🌱 Sproutly Arduino Firmware

This repository contains the Arduino firmware for the **Sproutly smart plant care system**. It manages soil moisture monitoring, automatic watering, and Bluetooth communication with the mobile app.

---

## 🚀 Features

- 🌿 Real-time soil moisture monitoring  
- 💧 Automatic watering based on thresholds  
- ⏱️ Smart cooldown to prevent overwatering  
- 📱 Bluetooth (HC-05) manual control  
- ⚙️ App-controlled cooldown interval  
- 🔁 Pump safety timeout  

---

## 🧠 How It Works

1. Reads soil moisture continuously  
2. If dry → pump turns ON  
3. If wet → pump turns OFF and cooldown starts  
4. If still dry after timeout → retries watering  
5. App allows manual control and configuration  

---

## 🔌 Hardware

- Arduino (Uno/Nano)  
- Soil moisture sensor  
- Water pump  
- Motor driver / relay  
- HC-05 Bluetooth module  

---

## 📡 Commands

| Command | Action |
|--------|--------|
| `1` | Pump ON |
| `0` | Pump OFF |
| `C30` | Set cooldown to 30s |
| `C180` | Set cooldown to 180s |

---

## 🛠️ Setup

1. Wire components  
2. Open `.ino` in Arduino IDE  
3. Select board & port  
4. Upload  

---

## 📊 Output

```
M:45% D:22% C:30s
```

---

## 🌱 Goal

Simple, efficient, and automated plant care.
