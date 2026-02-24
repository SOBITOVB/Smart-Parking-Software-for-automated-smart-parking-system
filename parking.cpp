#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Servo.h>
#include <SPI.h>
#include <MFRC522.h>
#include <EEPROM.h>

#define SS_PIN 10
#define RST_PIN 9
#define IR_ENTRY 2
#define IR_EXIT 3
#define SERVO_ENTRY 5
#define SERVO_EXIT 6

#define MAX_UIDS 4  
#define UID_SIZE 4  
#define EEPROM_START_ADDR 0  

LiquidCrystal_I2C lcd(0x27, 16, 2);
MFRC522 rfid(SS_PIN, RST_PIN);
Servo servoEntry;
Servo servoExit;

const int totalSlots = 6; 
int availableSlots = totalSlots;
bool accessGranted = false; 

void setup() {
    Serial.begin(9600);
    lcd.init();
    lcd.backlight();
    
    pinMode(IR_ENTRY, INPUT_PULLUP);
    pinMode(IR_EXIT, INPUT_PULLUP);
    servoEntry.attach(SERVO_ENTRY);
    servoExit.attach(SERVO_EXIT);
    
    servoEntry.write(90);
    servoExit.write(90);
    
    SPI.begin();
    rfid.PCD_Init();
    
    lcd.setCursor(0, 0);
    lcd.print("  PARKING READY ");
    delay(2000);
    lcd.clear();
    updateLCD();
    
    storeAllowedUIDs();
}

void loop() {
    if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
        byte readUID[UID_SIZE];
        for (byte i = 0; i < UID_SIZE; i++) {
            readUID[i] = rfid.uid.uidByte[i];
        }
        
        if (isAuthorized(readUID)) {
            accessGranted = true; 
            Serial.println("✅ Jeton ruxsat berildi!");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" Jeton OK ");
            lcd.setCursor(0, 1);
            lcd.print("Mashina olib keling");
        } else {
            Serial.println("❌ Jeton ruxsat etilmagan!");
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print("  Ruxsat yo'q! ");
            delay(2000);
            updateLCD();
        }

        rfid.PICC_HaltA();
        rfid.PCD_StopCrypto1();
    }

    if (accessGranted && digitalRead(IR_ENTRY) == LOW) {
        Serial.println("🔓 Mashina aniqlandi, shlakbaum ochilmoqda...");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("   Ochilmoqda! ");

        if (availableSlots > 0) {
            openGate(servoEntry);
            delay(3000);
            closeGate(servoEntry);
            availableSlots--;
            updateLCD();
        } else {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(" BO'SH JOY YO'Q ");
            delay(2000);
            updateLCD();
        }
        accessGranted = false; 
    }

    if (digitalRead(IR_EXIT) == LOW) {
        Serial.println("Mashina chiqmoqda...");
        if (availableSlots < totalSlots) {
            availableSlots++;
            updateLCD();
        }
        openGate(servoExit);
        delay(3000);
        closeGate(servoExit);
    }
}

void storeAllowedUIDs() {
    const byte allowedUIDs[MAX_UIDS][UID_SIZE] = {
        {0xA3, 0x00, 0x74, 0xFC},  
        {0x93, 0x03, 0x59, 0x19},  
        {0xF3, 0xFC, 0xEF, 0x0D},  
        {0x63, 0x5E, 0x29, 0xF7}     
    };

    for (int i = 0; i < MAX_UIDS; i++) {
        for (int j = 0; j < UID_SIZE; j++) {
            EEPROM.write(EEPROM_START_ADDR + (i * UID_SIZE) + j, allowedUIDs[i][j]);
        }
    }
}

bool isAuthorized(byte uid[]) {
    for (int i = 0; i < MAX_UIDS; i++) {
        bool match = true;
        for (byte j = 0; j < UID_SIZE; j++) {
            if (EEPROM.read(EEPROM_START_ADDR + (i * UID_SIZE) + j) != uid[j]) {
                match = false;
                break;
            }
        }
        if (match) return true;
    }
    return false;
}

void openGate(Servo &servo) {
    for (int pos = 90; pos >= 0; pos -= 2) {
        servo.write(pos);
        delay(30);
    }
}

void closeGate(Servo &servo) {
    for (int pos = 0; pos <= 90; pos += 2) {
        servo.write(pos);
        delay(30);
    }
}

void updateLCD() {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("  BO'SH JOYLAR: ");
    lcd.setCursor(7, 1);
    lcd.print(availableSlots);
}
