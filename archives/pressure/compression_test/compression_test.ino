#include <Servo.h>

#define fsrpin A1
#define servopin 9

Servo servo;

int neutral = 90;     // a ajuster !
int speed = 30;       // vitesse de base
int timeDelay = 300;

void setup() {
  servo.attach(servopin);
  Serial.begin(9600);
  Serial.println("Tape 1 pour serrer ou 2 pour desserrer");
  servo.write(neutral); // stop
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '1') { // sens 1
      servo.write(neutral + speed);
      Serial.println("serre");
      delay(timeDelay);
      servo.write(neutral); // stop
    }

    if (c == '2') { // sens 2
      servo.write(neutral - speed);
      Serial.println("dessere");
      delay(timeDelay);
      servo.write(neutral); // stop
    }
  }

  // Read the FSR pin and store the output as fsrreading:
  int fsrreading = analogRead(fsrpin);
  Serial.print("Analog reading = ");
  Serial.println(fsrreading);

  delay(50);
}
