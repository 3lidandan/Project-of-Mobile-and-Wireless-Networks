#define BLYNK_TEMPLATE_ID "TMPL6NPRa0l6p"
#define BLYNK_TEMPLATE_NAME "Self watering plant"
#define BLYNK_AUTH_TOKEN "NcCWFk4vz46el5Hpt90hJ6CB4ZAO2abV"  // Replace with your Blynk Auth Token

#define BLYNK_PRINT Serial 

#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

// Wi-Fi credentials
char ssid[] = "AA_4G";  
char pass[] = "0501902736";     
char auth[] = BLYNK_AUTH_TOKEN; 

// Pin definitions
#define MOISTURE_SENSOR_PIN D0     // soil moisture sensor (connected to D0)
#define WATER_LEVEL_PIN A0         // Water level sensor (connected to A0)
#define RELAY_PIN D3               // Relay control for the pump (connected to D3)

// Thresholds
const int moistureThreshold = 45; // Soil moisture threshold (below or equal to this value, water the plant)
const int waterLevelThreshold = HIGH; // Water level threshold (assumes HIGH is sufficient)

// State variables
bool isPumpOn = false; 


BlynkTimer timer;

void setup() {

  Serial.begin(9600);
  Serial.println("Initializing ESP8266...");

  
  pinMode(MOISTURE_SENSOR_PIN, INPUT);
  pinMode(WATER_LEVEL_PIN, INPUT);
  pinMode(RELAY_PIN, OUTPUT);

 
  digitalWrite(RELAY_PIN, LOW);

  
  Blynk.begin(auth, ssid, pass);
  Serial.println("Connected to Wi-Fi and Blynk!");


  timer.setInterval(2000L, checkSensors); 
}


void checkSensors() {
  // Read water level (from Water level sensor now connected to D0)
  int waterLevel = digitalRead(MOISTURE_SENSOR_PIN); // This is checking water level now
  Serial.print("Water Level: ");
  Serial.println(waterLevel == HIGH ? "OK" : "Low");
  Blynk.virtualWrite(V2, waterLevel == HIGH ? 1 : 0); // Send water level status to Blynk

  // Read soil moisture (from the soil moisture sensor now connected to A0)
  int soilMoisture = analogRead(WATER_LEVEL_PIN); // This is checking soil moisture now
  soilMoisture = map(soilMoisture, 1023, 0, 0, 100); // Convert to percentage (0-100)
  int soilMoistureBinary = soilMoisture >= 50 ? 1 : 0; // Convert to 0 or 1 based on threshold
  
  Serial.print("Soil Moisture: ");
  Serial.print(soilMoisture);
  Serial.print("% (Binary: ");
  Serial.print(soilMoistureBinary);
  Serial.println(")");

  Blynk.virtualWrite(V1, soilMoistureBinary); // Send binary value (0 or 1) to Blynk

  // Automatic watering logic (only if the pump is not manually turned on)
  if (!isPumpOn) { // Check if manual mode is not active
    if (soilMoisture <= moistureThreshold && waterLevel == HIGH) {
      Serial.println("Soil is dry and water level is OK. Turning on pump.");
      digitalWrite(RELAY_PIN, HIGH); // Turn on pump
      Blynk.virtualWrite(V3, HIGH);  // Display pump ON in Blynk app
    } else {
      Serial.println("Soil moisture sufficient or water level low. Turning off pump.");
      digitalWrite(RELAY_PIN, LOW); // Turn off pump
      Blynk.virtualWrite(V3, LOW);  // Display pump OFF in Blynk app
    }
  }
}


BLYNK_WRITE(V3) {
  int pumpState = param.asInt(); 

  if (pumpState == 1) {
    Serial.println("Manual Mode: Pump ON (via Blynk)");
    digitalWrite(RELAY_PIN, HIGH); // Turn on pump
    isPumpOn = true;
  } else {
    Serial.println("Manual Mode: Pump OFF (via Blynk)");
    digitalWrite(RELAY_PIN, LOW); // Turn off pump
    isPumpOn = false;
  }
}

void loop() {
  Blynk.run();
  timer.run();
}


