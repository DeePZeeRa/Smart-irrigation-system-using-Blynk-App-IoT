#define BLYNK_TEMPLATE_ID "TMPL3sTpibzQr"
#define BLYNK_TEMPLATE_NAME "AUTOMATIC WATERING SYSTEM WITH IOT"
#define BLYNK_AUTH_TOKEN "hrX6xb7QLiHvLqVm2CK9EGvS-mHZLhic"

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h> 

#define SOIL_MOISTURE_PIN A0   // Analog pin for soil moisture sensor
int THRESHOLD_MOISTURE = 30; // Adjust this value based on your soil moisture sensor readings
#define PUMP_PIN D4  // Digital pin for controlling the pump
#define PUMP_SWITCH V6  // Virtual pin for controlling the pump manually

char auth[] = BLYNK_AUTH_TOKEN;  // Replace with your Blynk auth token
char ssid[] = "Deepayan";   // Replace with your WiFi credentials
char pass[] = "ebzd8001";

BlynkTimer timer;

bool isPumpOn = false;  // Variable to track pump status

void sendSensorData()
{
  int soilMoisture = analogRead(SOIL_MOISTURE_PIN);
  Serial.print("Soil Moisture ");
  Serial.println(soilMoisture);
  // Map the analog sensor values to a percentage (0-100)
  int soilMoisturePercentage = map(soilMoisture, 400, 1023, 100, 0);
  Serial.print("Soil Moisture Percentage ");
  Serial.println(soilMoisturePercentage);
  // Send soil moisture data to Blynk
  Blynk.virtualWrite(V5, soilMoisturePercentage);

  // Check if the pump should be on based on manual switch or soil moisture level
  if (isPumpOn || soilMoisturePercentage < THRESHOLD_MOISTURE)
  {
    // Turn on the pump
    digitalWrite(PUMP_PIN, LOW);
    // Check if the pump is turned on automatically (not manually)
    if (!isPumpOn) {
      // Send alert notification to Blynk app if the pump is turned on automatically
      Blynk.logEvent("moisture_alert","Soil moisture is below the threshold!");
      Serial.println("Soil moisture is below the threshold!");
    }
  }
  else
  {
    // Turn off the pump only if it was not turned on manually
    if (!isPumpOn) {
      digitalWrite(PUMP_PIN, HIGH);
    }
  }
}

BLYNK_WRITE(PUMP_SWITCH)
{
  isPumpOn = param.asInt();
  if (isPumpOn) {
    digitalWrite(PUMP_PIN, HIGH);
    // Send alert notification to Blynk app if the pump is turned on automatically
    
    Serial.println("Pump manually turned ON");
  } else {
    digitalWrite(PUMP_PIN, LOW);
    Serial.println("Pump manually turned OFF");
  }
}

void setup()
{
  Serial.begin(9600);
  Blynk.begin(auth, ssid, pass);
  pinMode(PUMP_PIN, OUTPUT); // Set pump pin as an output

  timer.setInterval(3000L, sendSensorData); // Set the interval for checking soil moisture (every 3 seconds)

  // Setup switch widget
  Blynk.virtualWrite(PUMP_SWITCH, isPumpOn);
  Blynk.syncVirtual(PUMP_SWITCH);
}

void loop()
{
  Blynk.run();
  timer.run();
}