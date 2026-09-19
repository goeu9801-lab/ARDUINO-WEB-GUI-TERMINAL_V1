/*
 * Web Serial GUI Dynamic Protocol Example
 * 
 * Protocol Specification:
 * Init: {"type":"init", "id":"ID", "label":"LABEL", "kind":"toggle|slider|chart", "min":0, "max":1023}
 * Data: {"type":"data", "id":"ID", "val":VALUE}
 * Inbound: {"id":"ID", "val":VALUE}
 */

const int LED_PIN = 13;
const int POT_PIN = A0;

unsigned long lastUpdate = 0;
const int updateInterval = 100; // Update every 100ms

void setup() {
  Serial.begin(9600);
  pinMode(LED_PIN, OUTPUT);

  // 1. Initialize the GUI components upon connection
  // These lines tell the Web GUI what to render
  
  // Create a Toggle Switch for the LED
  Serial.println(F("{\"type\":\"init\", \"id\":\"led1\", \"label\":\"Onboard LED\", \"kind\":\"toggle\"}"));
  
  // Create a Real-time Chart for the Potentiometer
  Serial.println(F("{\"type\":\"init\", \"id\":\"pot1\", \"label\":\"Sensor Voltage\", \"kind\":\"chart\", \"min\":0, \"max\":1023}"));
  
  delay(100);
}

void loop() {
  // --- Handle Incoming Data (Web GUI -> Arduino) ---
  if (Serial.available() > 0) {
    String jsonIn = Serial.readStringUntil('\n');
    
    // Simple parsing for the LED toggle
    // Expected: {"id":"led1", "val":1}
    if (jsonIn.indexOf("\"id\":\"led1\"") != -1) {
      if (jsonIn.indexOf("\"val\":1") != -1) {
        digitalWrite(LED_PIN, HIGH);
      } else {
        digitalWrite(LED_PIN, LOW);
      }
    }
  }

  // --- Handle Outgoing Data (Arduino -> Web GUI) ---
  if (millis() - lastUpdate > updateInterval) {
    int sensorVal = analogRead(POT_PIN);
    
    // Send data update for the chart
    Serial.print(F("{\"type\":\"data\", \"id\":\"pot1\", \"val\":"));
    Serial.print(sensorVal);
    Serial.println(F("}"));
    
    lastUpdate = millis();
  }
}