#include <Wire.h>
#include <SoftwareSerial.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <TinyGPS++.h>

// Pin Definitions
#define PULSE_PIN A0               // Pulse Sensor connected to A0
#define ONE_WIRE_BUS 2             // DS18B20 Temperature Sensor connected to pin 2
#define GPS_RX 4                   // GPS RX Pin
#define GPS_TX 3                   // GPS TX Pin
#define GSM_TX 10                  // GSM TX Pin
#define GSM_RX 11                  // GSM RX Pin

// Initialize Components
SoftwareSerial gpsSerial(GPS_RX, GPS_TX);
SoftwareSerial gsmSerial(GSM_TX, GSM_RX);
TinyGPSPlus gps;
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature tempSensor(&oneWire);

// Variables for Sensor Readings
float temperature;
int heartRate;

void setup() {
    Serial.begin(9600);             // Monitor Serial
    gpsSerial.begin(9600);          // GPS Module Serial
    gsmSerial.begin(9600);          // GSM Module Serial
    tempSensor.begin();             // Temperature Sensor Initialization
    pinMode(PULSE_PIN, INPUT);      // Pulse Sensor Initialization

    // Initialize GSM Module
    gsmSerial.println("AT");        // Basic command to check connectivity
    delay(1000);
    gsmSerial.println("AT+CMGF=1"); // Set SMS to text mode
    delay(1000);
}

void loop() {
    // Step 2: GPS Position Tracking
    if (gpsSerial.available() > 0) {
        if (gps.encode(gpsSerial.read())) {
            float latitude = gps.location.lat();
            float longitude = gps.location.lng();
            Serial.print("Latitude: "); Serial.println(latitude, 6);
            Serial.print("Longitude: "); Serial.println(longitude, 6);
        }
    }

    // Step 3: Health Monitoring
    tempSensor.requestTemperatures();         // Request temperature from DS18B20
    temperature = tempSensor.getTempCByIndex(0); // Get temperature reading
    Serial.print("Temperature (°C): "); Serial.println(temperature);

    // Pulse Sensor Reading (simulated reading; replace with actual processing if available)
    heartRate = analogRead(PULSE_PIN);        // Simulated heart rate value
    Serial.print("Heart Rate: "); Serial.println(heartRate);

    // Step 4: Data Processing & Communication
    String message = "Lat: " + String(gps.location.lat(), 6) + ", Long: " + String(gps.location.lng(), 6);
    message += ", Temp: " + String(temperature) + "°C, Heart Rate: " + String(heartRate) + " BPM";
    sendSMS(message);

    delay(5000); // Delay to optimize power consumption
}

void sendSMS(String message) {
    gsmSerial.println("AT+CMGS=\"+91xxxxxxxxxxx\""); // Replace with base station or command center number
    delay(1000);
    gsmSerial.println(message);                  // Send message data
    delay(100);
    gsmSerial.write(26);                         // End SMS with Ctrl+Z
    delay(1000);
}