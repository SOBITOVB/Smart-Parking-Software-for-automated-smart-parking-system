# Smart-Parking-Software-for-automated-smart-parking-system
Smart Parking – Software for automated smart parking system


# Smart Parking Gate System (Arduino + RFID + IR + LCD)

A small smart parking prototype that controls entry/exit barriers using **RFID authorization**, **IR sensors** for vehicle detection, and a **16x2 I2C LCD** to display the number of available parking slots.

## Features
- ✅ RFID-based access control (authorized tokens only)
- ✅ Entry gate opens only after valid RFID + vehicle detection (IR sensor)
- ✅ Exit gate opens when vehicle is detected at exit
- ✅ Parking availability counter (decreases on entry, increases on exit)
- ✅ LCD shows available slots in real time
- ✅ Authorized RFID UIDs are stored in **EEPROM** (persistent after restart)

## Hardware Components
- Arduino Uno/Nano (compatible)
- MFRC522 RFID reader (SPI)
- 2x IR sensors (Entry / Exit)
- 2x Servo motors (Entry barrier / Exit barrier)
- 16x2 I2C LCD (address typically `0x27`)
- Jumper wires, breadboard, power supply

## Wiring / Pin Mapping
| Module | Pin | Arduino |
|------|-----|---------|
| MFRC522 SDA(SS) | SS_PIN | D10 |
| MFRC522 RST | RST_PIN | D9 |
| Entry IR sensor | IR_ENTRY | D2 |
| Exit IR sensor | IR_EXIT | D3 |
| Entry Servo | SERVO_ENTRY | D5 |
| Exit Servo | SERVO_EXIT | D6 |
| LCD I2C | SDA/SCL | A4 / A5 (Uno) |

> Note: If your LCD does not work, scan I2C address (0x27 / 0x3F are common).

## How It Works (Logic)
1. **RFID card/token is scanned**
2. System checks UID in EEPROM:
   - If authorized → shows *Jeton OK* and waits for entry IR sensor
   - If not authorized → shows *Ruxsat yo'q!*
3. When entry IR sensor detects a car:
   - If slots available → opens entry barrier, decreases available slots
   - If no slots → shows *BO'SH JOY YO'Q*
4. When exit IR sensor detects a car:
   - opens exit barrier and increases available slots (up to totalSlots)

## Configuration
- Total parking capacity:
  ```cpp
  const int totalSlots = 6;
