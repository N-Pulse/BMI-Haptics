#include <Servo.h>

#define fsrpin1 A1
#define fsrpin2 A2
#define servopin 9

Servo servo;

int neutral = 90;     // a ajuster !
int speed = 30;       // vitesse de base
int timeDelay = 300;

// PID parameters
float Kp = 0.01;       // Proportional gain (a ajuster !)
float Ki = 0.0;      // Integral gain (a ajuster !)
float Kd = 0.0;      // Derivative gain (a ajuster !)

float integral = 0;
float prevError = 0;
unsigned long prevTime = 0;

// Speed limits for safety
int maxSpeed = 40;

bool closedLoopMode = false;

void setup() {
  servo.attach(servopin);
  Serial.begin(9600);
  Serial.println("Tape 1 pour serrer, 2 pour desserrer, 3 pour closed-loop");
  servo.write(neutral); // stop
  prevTime = millis();
}

void loop() {
  if (Serial.available()) {
    char c = Serial.read();

    if (c == '1') { // sens 1
      closedLoopMode = false;
      servo.write(neutral + speed);
      Serial.println("serre");
      delay(timeDelay);
      servo.write(neutral); // stop
    }

    if (c == '2') { // sens 2
      closedLoopMode = false;
      servo.write(neutral - speed);
      Serial.println("dessere");
      delay(timeDelay);
      servo.write(neutral); // stop
    }

    if (c == '3') { // closed-loop mode
      closedLoopMode = true;
      integral = 0; // Reset integral
      prevError = 0; // Reset error
      Serial.println("Mode closed-loop active");
    }

    if (c == '0') { // stop
      closedLoopMode = false;
      servo.write(neutral);
      Serial.println("Arret");
    }
  }

  // Read the FSR pins
  int fsr1 = analogRead(fsrpin1);
  int fsr2 = analogRead(fsrpin2);
  
  Serial.print("FSR1 = ");
  Serial.print(fsr1);
  Serial.print(" | FSR2 = ");
  Serial.println(fsr2);

  if (closedLoopMode) {
    // Calculate FSR difference
    float fsrDiff = fsr1 - fsr2;
    
    // Calculate time delta
    unsigned long currentTime = millis();
    float dt = (currentTime - prevTime) / 1000.0; // Convert to seconds
    prevTime = currentTime;
    
    // PID calculation
    float error = fsrDiff; // Target is 0 difference
    integral += error * dt;
    float derivative = (error - prevError) / dt;
    
    float pidOutput = Kp * error + Ki * integral + Kd * derivative;
    
    prevError = error;
    
    // Convert PID output to motor speed and clamp (keep sign!)
    int motorSpeed = constrain(pidOutput, -maxSpeed, maxSpeed);
    
    int speedCmd = 0;
    // Determine direction based on error sign
    if (abs(error) < 5) { // Dead zone to prevent oscillation
      speedCmd = neutral;
      motorSpeed = 0;
    } else {
      speedCmd = neutral - motorSpeed; // Invert sign if needed
    }

    servo.write(speedCmd); 

    
    Serial.print("Error = ");
    Serial.print(error);
    Serial.print(" | PID = ");
    Serial.print(pidOutput);
    Serial.print(" | Speed = ");
    Serial.print(motorSpeed);
    Serial.print(" | Speed command = ");
    Serial.println(speedCmd);
  }

  delay(500);
}
