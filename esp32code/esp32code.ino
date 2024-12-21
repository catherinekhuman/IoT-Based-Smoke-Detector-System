#define BLYNK_PRINT Serial
#define BLYNK_TEMPLATE_ID "   "
#define BLYNK_TEMPLATE_NAME "   "
#define BLYNK_AUTH_TOKEN "   "

#include <Wire.h>
#include <WiFiClient.h>
#include <LiquidCrystal_I2C.h>
#include <OneWire.h>
#include <DallasTemperature.h>
#include <WiFi.h>
#include <BlynkSimpleEsp32.h>

BlynkTimer timer;

//LCD setup
LiquidCrystal_I2C lcd(0x27, 16, 2);

//DS18B20 setup
#define ONE_WIRE_BUS 13
OneWire oneWire(ONE_WIRE_BUS);
DallasTemperature sensors(&oneWire);

//MQ2 setup
#define MQ2_PIN 34

// Buzzer setup
#define BUZZER_PIN 27

// Thresholds
const int SMOKE_THRESHOLD = 800;
const float TEMP_THRESHOLD = 30.0;

//Blynk setup
char auth[] = BLYNK_AUTH_TOKEN;
char ssid[] = "   ";
char pass[] = "   ";
String Message = "Smoke Alert";

//Timing variables
unsigned long previousMillis = 0;
const long interval = 2000;

void sendSensorData() {
  //Read temperature
  sensors.requestTemperatures();
  float temperature = sensors.getTempCByIndex(0);
  Serial.print("Temperature: ");
  Serial.println(temperature);

  //Read smoke level
  int smokeLevel = analogRead(MQ2_PIN);
  Serial.print("Smoke Level:");
  Serial.println(smokeLevel);

  //Display on LCD
  lcd.setCursor(0, 0);
  lcd.print("Smoke: ");
  lcd.print(smokeLevel);
  lcd.print(" PPM     ");
  lcd.setCursor(0, 1);
  lcd.print("Temp: ");
  lcd.print(temperature, 1);
  lcd.print(" C       ");

  //Send data to Blynk
  Blynk.virtualWrite(V0, smokeLevel);
  Blynk.virtualWrite(V1, temperature);

  //Check thresholds
  if (smokeLevel > SMOKE_THRESHOLD || temperature > TEMP_THRESHOLD){
    digitalWrite(BUZZER_PIN, HIGH);
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Smoke Alert!");
    lcd.setCursor(0, 1);
    lcd.print("Check Area!");
    Blynk.logEvent("smoke_alert");
    Blynk.logEvent("temperature _alert");
  } else {
    digitalWrite(BUZZER_PIN, LOW);
    lcd.setCursor(0, 0);
    lcd.setCursor(0, 1);
  }
}


void setup() {
  Serial.begin(115200);

  lcd.init();
  lcd.backlight();
  lcd.print("Initializing...");

  sensors.begin();

  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);

  Blynk.begin(auth, ssid, pass);
  timer.setInterval(2000L, sendSensorData);

  lcd.setCursor(0,1);
  lcd.print("Ready");
  delay(2000);
  lcd.clear();

  digitalWrite(BUZZER_PIN, HIGH);
  delay(2000);
  digitalWrite(BUZZER_PIN, LOW);

}

void loop() {
  Blynk.run();
  timer.run();

}