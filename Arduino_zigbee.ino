#include <DHT.h>
#include <SoftwareSerial.h>
#include <Servo.h>
#define DHTPIN 4
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);
SoftwareSerial xbee(2, 3);   // RX, TX
#define MQ2 A0
#define GREEN   6
#define YELLOW  7
#define RED     8
#define BUZZER  9
#define FAN     10
#define SERVO_PIN 11 /* Servo for vent [additional feature] */
Servo ventServo;
int SERVO_ON_POS  = 90;   /* SG90 open position*/
int SERVO_OFF_POS = 0;    /* SG90 closed position*/

int GAS_TH  = 400; /*Threshold set for gas and temperature to trigger alarm*/
int TEMP_TH = 45;

void setup() {
  Serial.begin(9600);
  xbee.begin(9600);
  dht.begin();

  pinMode(GREEN, OUTPUT); /*Led for warning*/
  pinMode(YELLOW, OUTPUT);
  pinMode(RED, OUTPUT);
  pinMode(BUZZER, OUTPUT);
  pinMode(FAN, OUTPUT);

  ventServo.attach(SERVO_PIN);
  ventServo.write(SERVO_OFF_POS);   /* default closed*/

  Serial.println("ARDUINO READY");
}

void loop() {

  float t = dht.readTemperature();
  float h = dht.readHumidity();
  int gas = analogRead(MQ2);

  if (isnan(t) || isnan(h)) return;

  bool gasAlert  = gas > GAS_TH;
  bool tempAlert = t > TEMP_TH;

  digitalWrite(GREEN, LOW);
  digitalWrite(YELLOW, LOW);
  digitalWrite(RED, LOW);
  digitalWrite(BUZZER, LOW);

  /* -------------------- LOGIC -------------------- */
  if (!gasAlert) {
    digitalWrite(GREEN, HIGH);
    digitalWrite(FAN, LOW);
    ventServo.write(SERVO_OFF_POS);     // close vent
  }
  else if (gasAlert && !tempAlert) {
    digitalWrite(YELLOW, HIGH);
    beep3();
    digitalWrite(FAN, HIGH);
    ventServo.write(SERVO_ON_POS);      // open vent
  }
  else {
    digitalWrite(RED, HIGH);
    digitalWrite(BUZZER, HIGH);
    digitalWrite(FAN, HIGH);
    ventServo.write(SERVO_ON_POS);      // open vent
  }

  /* -------------------- ZIGBEE TX -------------------- */
  xbee.print(t, 1);
  xbee.print(",");
  xbee.print(h, 1);
  xbee.print(",");
  xbee.println(gas);

  Serial.print("TX → ");
  Serial.print(t, 1);
  Serial.print(",");
  Serial.print(h, 1);
  Serial.print(",");
  Serial.println(gas);

  delay(1000);
}

void beep3() {
  for (int i = 0; i < 3; i++) {
    digitalWrite(BUZZER, HIGH);
    delay(200);
    digitalWrite(BUZZER, LOW);
    delay(200);
  }
}
