#include "VOneMqttClient.h"
#include <ESP32Servo.h>
#include "DHT.h"

/* ===================== CONFIGURATION ===================== */

// Moisture calibration
int MinMoistureValue = 4095;
int MaxMoistureValue = 2060;
int Moisture = 0;

// Automation control
bool automationEnabled = true;
unsigned long lastManualCommandTime = 0;
const unsigned long manualTimeout = 5000; // 5 seconds manual override

// Thresholds
const int MOISTURE_LOW_THRESHOLD = 30;
const int MOISTURE_HIGH_THRESHOLD = 60;

// Servo settings
const int SERVO_OPEN_ANGLE = 90;
const int SERVO_CLOSED_ANGLE = 0;

// Watering timing
const unsigned long MIN_WATERING_TIME = 5000;
const unsigned long MAX_WATERING_TIME = 30000;

// Watering state
bool isWatering = false;
unsigned long wateringStartTime = 0;

// Device IDs
const char* DHT11Sensor   = "7a1b4101-1139-4132-9ab0-a48fb49c5534";
const char* ServoMotor   = "548f1f27-0c7d-40e3-a0aa-37776c0973e0";
const char* MoistureSensor = "8e4dfefb-564b-4a1c-9c5e-134fe3d92a75";

// Pins
const int dht11Pin = 42;
const int servoPin = 21;
const int moisturePin = A2;
const int liquidSensorPin = 4;
const int buzzer = 12;

// Sensors
#define DHTTYPE DHT11
DHT dht(dht11Pin, DHTTYPE);

// Servo
Servo Myservo;

// V-One client
VOneMqttClient voneClient;

// Timing
unsigned long lastMsgTime = 0;

/* ===================== WIFI ===================== */

void setup_wifi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
  }
}

/* ===================== ACTUATOR CALLBACK ===================== */

void triggerActuator_callback(const char* actuatorDeviceId, const char* actuatorCommand) {

  JSONVar cmdObj = JSON.parse(actuatorCommand);
  JSONVar keys = cmdObj.keys();

  if (String(actuatorDeviceId) == ServoMotor) {

    String key = "";
    int cmd = 0;

    for (int i = 0; i < keys.length(); i++) {
      key = (const char*)keys[i];
      cmd = (int)cmdObj[keys[i]];
    }

    if (key == "Servo") {

      automationEnabled = false;
      lastManualCommandTime = millis();

      if (cmd == 1) {
        Myservo.write(SERVO_OPEN_ANGLE);
        Serial.println("MANUAL: Servo OPEN (90°)");
      } else {
        Myservo.write(SERVO_CLOSED_ANGLE);
        Serial.println("MANUAL: Servo CLOSED (0°)");
      }
    }

    voneClient.publishActuatorStatusEvent(
      actuatorDeviceId, actuatorCommand, "", true
    );
  }
}

/* ===================== SETUP ===================== */

void setup() {
  Serial.begin(115200);

  setup_wifi();

  voneClient.setup();
  voneClient.registerActuatorCallback(triggerActuator_callback);

  dht.begin();

  pinMode(liquidSensorPin, INPUT);
  pinMode(buzzer, OUTPUT);

  Myservo.attach(servoPin);
  Myservo.write(SERVO_CLOSED_ANGLE);
}

/* ===================== LOOP ===================== */

void loop() {

  if (!voneClient.connected()) {
    voneClient.reconnect();
    voneClient.publishDeviceStatusEvent(DHT11Sensor, true);
    voneClient.publishDeviceStatusEvent(MoistureSensor, true);
  }

  voneClient.loop();

  unsigned long now = millis();

  /* ===== Resume automation after manual timeout ===== */
  if (!automationEnabled && (now - lastManualCommandTime > manualTimeout)) {
    automationEnabled = true;
    Serial.println("AUTO: Automation resumed");
  }

  /* ===== Sensor publishing ===== */
  if (now - lastMsgTime > INTERVAL) {
    lastMsgTime = now;

    float h = dht.readHumidity();
    int t = dht.readTemperature();

    if (!isnan(h) && !isnan(t)) {
      JSONVar dhtPayload;
      dhtPayload["Humidity"] = h;
      dhtPayload["Temperature"] = t;
      voneClient.publishTelemetryData(DHT11Sensor, dhtPayload);
    }

    int rawMoisture = analogRead(moisturePin);
    Moisture = map(rawMoisture, MinMoistureValue, MaxMoistureValue, 0, 100);
    Moisture = constrain(Moisture, 0, 100);

    JSONVar moisturePayload;
    moisturePayload["Soil moisture"] = Moisture;
    voneClient.publishTelemetryData(MoistureSensor, moisturePayload);

    // Water tank alert
    if (digitalRead(liquidSensorPin) == 0) {
      tone(buzzer, 1000);
    } else {
      noTone(buzzer);
    }
  }

  /* ===================== AUTOMATION ===================== */
  if (automationEnabled) {

    // Start watering
    if (!isWatering && Moisture < MOISTURE_LOW_THRESHOLD) {

      if (digitalRead(liquidSensorPin) == 1) {
        isWatering = true;
        wateringStartTime = now;
        Myservo.write(SERVO_OPEN_ANGLE);
        Serial.println("AUTO: Watering started");
      }
    }

    // Stop watering
    if (isWatering) {

      if (Moisture > MOISTURE_HIGH_THRESHOLD ||
          (now - wateringStartTime > MAX_WATERING_TIME) ||
          ((now - wateringStartTime > MIN_WATERING_TIME) &&
           Moisture > MOISTURE_LOW_THRESHOLD)) {

        isWatering = false;
        Myservo.write(SERVO_CLOSED_ANGLE);
        Serial.println("AUTO: Watering stopped");
      }
    }
  }
}
