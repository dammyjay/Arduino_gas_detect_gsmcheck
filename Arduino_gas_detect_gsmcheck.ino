#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <SoftwareSerial.h>

#define MQ2_PIN A1  // Gas sensor analog pin
#define BUZZER_PIN 6
#define RED_LED 7
#define GREEN_LED 8

SoftwareSerial gsm(9, 10);  // GSM TX to Arduino RX (Pin 9), GSM RX to Arduino TX (Pin 10)
LiquidCrystal_I2C lcd(0x27, 16, 2);  // LCD I2C address (0x27 or 0x3F, check yours)

const int gasThreshold = 150;  // Adjust threshold based on environment

void setup() {
    Serial.begin(9600);
    gsm.begin(9600);
    lcd.init();
    lcd.backlight();
    
    pinMode(MQ2_PIN, INPUT);
    pinMode(BUZZER_PIN, OUTPUT);
    pinMode(RED_LED, OUTPUT);
    pinMode(GREEN_LED, OUTPUT);

    digitalWrite(GREEN_LED, HIGH);  // Normal state

    lcd.setCursor(0, 0);
    lcd.print("Gas leak Detector"); // lcd printing name of project
    lcd.setCursor(0, 1);
    lcd.print("Initializing...");
    delay(2000);

    if (checkGSM()) {
        Serial.println("GSM Module Ready!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GSM Ready!");
        delay(2000);
    } else {
        Serial.println("GSM Module Not Detected!");
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("GSM Error!");
        lcd.setCursor(0, 1);
        lcd.print("Check SIM/Power");
        while (true);  // Stop further execution
    }

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gas Detect Ready");
    delay(2000);
}

void loop() {
    int gasValue = analogRead(MQ2_PIN);
    Serial.print("Gas Level: ");
    Serial.println(gasValue);

    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Gas Level: ");
    lcd.print(gasValue);

    if (gasValue > gasThreshold) {
        Serial.println("Gas Detected! Sending SMS...");
        lcd.setCursor(0, 1);
        lcd.print("Warning:Gas Leak!");

        digitalWrite(BUZZER_PIN, HIGH);
        digitalWrite(RED_LED, HIGH);
        digitalWrite(GREEN_LED, LOW);
        
        sendSMS("Warning! Gas leak detected! Take immediate action.");

        delay(5000);  // Avoid continuous SMS spam
    } else {
        digitalWrite(BUZZER_PIN, LOW);
        digitalWrite(RED_LED, LOW);
        digitalWrite(GREEN_LED, HIGH);
        lcd.setCursor(0, 1);
        lcd.print("Status: Normal   ");
    }
    
    delay(1000);
}

bool checkGSM() {
    gsm.println("AT");  // Send AT command
    delay(1000);
    if (gsm.available()) {
        String response = gsm.readString();
        return response.indexOf("OK") >= 0;
    }
    return false;
}

void sendSMS(String message) {
    gsm.println("AT+CMGF=1");  // Set SMS mode
    delay(1000);
    gsm.println("AT+CMGS=\"+2349167667242\"");  // Replace with actual phone number
    delay(1000);
    gsm.print(message);
    delay(1000);
    gsm.write(26);  // End SMS with Ctrl+Z
    delay(3000);
}
