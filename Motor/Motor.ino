#define BLYNK_TEMPLATE_NAME "Luminergy"
#define BLYNK_AUTH_TOKEN "gKh6CpwkXRATQMiHj8LUnKjsbE8P_k-9"
#define BLYNK_TEMPLATE_ID "TMPL6UktazjN5"

#define BLYNK_PRINT Serial

#include <ESP8266WiFi.h> 
#include <BlynkSimpleEsp8266.h>
#include <DHT.h>
#include <Stepper.h>

char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "niqhkmm";     // Your WiFi SSID
char pass[] = "aniq0000";    // Your WiFi password

BlynkTimer timer;

#define DHTPIN 2        // Connect Out pin to D4 in NODE MCU
#define DHTTYPE DHT11   // DHT sensor type
DHT dht(DHTPIN, DHTTYPE);

// Stepper motor configuration
#define STEPS_PER_REVOLUTION 200
#define STEPPER_PIN_1 D1
#define STEPPER_PIN_2 D2
#define STEPPER_PIN_3 D3
#define STEPPER_PIN_4 D5
Stepper stepper(STEPS_PER_REVOLUTION, STEPPER_PIN_1, STEPPER_PIN_3, STEPPER_PIN_2, STEPPER_PIN_4);

// Voltage sensor pin
#define VOLTAGE_SENSOR_PIN A0

// Initialize WiFi client
WiFiClientSecure client;

// Variables for stepper motor control
int targetStep = 0;
bool isMoving = false;

void sendSensor() {
  float h = dht.readHumidity();
  float t = dht.readTemperature(false);
  int voltage = analogRead(VOLTAGE_SENSOR_PIN); // Read voltage

  if (isnan(h) || isnan(t)) {
    Serial.println("Failed to read from DHT sensor!");
    return;
  }

  // Send temperature, humidity, and voltage to Blynk app
  Blynk.virtualWrite(V0, t);
  Blynk.virtualWrite(V1, h);
  Blynk.virtualWrite(V2, voltage); // Sending voltage to virtual pin V2
  Serial.print("Temperature: ");
  Serial.print(t);
  Serial.print(" °C, Humidity: ");
  Serial.print(h);
  Serial.print(", Voltage: ");
  Serial.print(voltage);
  Serial.println("V");
}

BLYNK_WRITE(V3) {  // Slider widget in Blynk app to control stepper motor
  targetStep = param.asInt();
  isMoving = true; // Set moving flag when the slider changes
}

void controlStepper() {
  static int currentPosition = 0; // Variable to track the current position

  if (currentPosition < targetStep) {
    stepper.step(1); // Move one step clockwise
    currentPosition++;
  } else if (currentPosition > targetStep) {
    stepper.step(-1); // Move one step counterclockwise
    currentPosition--;
  }
}

void setup() {
  Serial.begin(115200);

  // Initialize Blynk
  Blynk.begin(auth, ssid, pass);
  dht.begin();
  timer.setInterval(5000L, sendSensor); // Check every 5 seconds
  timer.setInterval(100L, controlStepper); // Control stepper motor frequently
}

void loop() {
  Blynk.run();
  timer.run();
}
