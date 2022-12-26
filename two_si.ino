// Import library for SERVOMOTOR
#include <Servo.h>

// Set ULTRASONIC SENSOR variables
long _duration;
int _distance;
int distance;


// Set ULTRASONIC SENSOR pins
const int trigPin = 12;
const int echoPin = 13;

// Set DC MOTOR pins
const int motorPin1 = 5; // Left motor forward
const int motorPin2 = 6; // Left motor backward
const int motorPin3 = 9; // Right motor forward
const int motorPin4 = 10; // Right motor backward

// Set FLAME SENSOR pin
const int flameSensorPin = A1;

// Set GAS SENSOR pin
const int gasSensorPin = A2;

// Set BUZZER pins
const int buzzerPin1 = A3;
const int buzzerPin2 = A4;

// Initialize SERVOMOTOR pin and attributes with Servo constructor
Servo servo;

// Initialize variables for left and right distances.
int distanceRight, distanceLeft;

// Initialize variable for flame and gas sensor values
int isFlame;
int isGas;

// Initialize functions to avoid undefined function errors
int getDistance(int delay_1, int delay_2);
int lookRight();
int lookLeft();
void turnLeft(int delay_ms);
void turnRight(int delay_ms);
void Forward(int delay_ms);
void Reverse(int delay_ms);
void Stop(int delay_ms);

void setup() {

    // Initialize ULTRASONIC SENSOR pins
    pinMode(trigPin, OUTPUT);
    pinMode(echoPin, INPUT);
  
    // Initialize DC MOTOR pins as outputs
    pinMode(motorPin1, OUTPUT);
    pinMode(motorPin2, OUTPUT);
    pinMode(motorPin3, OUTPUT);
    pinMode(motorPin4, OUTPUT);
  
    // Initialize FLAME SENSOR pin as input
    pinMode(flameSensorPin, INPUT);

    // Initialize GAS SENSOR pin as input
    pinMode(gasSensorPin, INPUT);

    // Initialize BUZZER pins as output
    pinMode(buzzerPin1, OUTPUT);
    pinMode(buzzerPin2, OUTPUT);
    
    // Set baud rate (per second)
    Serial.begin(9600);

    // Attach a servomotor to pin
    servo.attach(3);
}

void loop() 
{
    // Calculate distance in front and print in serial monitor
    distance = getDistance(20, 50);
    Serial.println(distance);

    // Set default movement to forward motion
    Forward(0);

    //Flame Sensor Operation
    isFlame = analogRead(flameSensorPin);  // Read the presence of flame
    isFlame = map(isFlame, 0, 1023, 255, 0); // Map input value to output of 0-255 in case of analog optimization

    // Turn buzzer in presence of flame and off if otherwise.
    if (isFlame >= 250) {
        digitalWrite(buzzerPin1, isFlame);
        Stop(500);
        Reverse(300);
    }
    else {
        digitalWrite(buzzerPin1, 0);
    }


    //Gas Sensor Operation
    isGas = analogRead(gasSensorPin); // Read the presence of smoke or flammable gas
    isGas = map(isGas, 0, 1023, 0, 255); // Map input value to output of 0-255 in case of analog optimization
    
    // Turn the buzzer on and off in regular intervals in presence of flame or flammable gas and off if otherwise.
    if (isGas >= 70) {
      digitalWrite(buzzerPin2, isGas);
      delay(100);
      digitalWrite(buzzerPin2, 0);
      delay(100);
    }
    else {
      digitalWrite(buzzerPin2, 0);
    }

    /* 
     * Ultrasonic Sensor and DC Motor Integration
     * 
     * Stop, reverse, stop, then determine left and right distances when front distance is less than 20cm.
     * Reverse if distances in both direction is less than 20cm.
     * Turn left if left distance is greater than right distance.
     * Turn right if right distance is greater than left distance.
     * 
     * Amount of turn depends on motion_duration parameter.
     */

    int motion_duration = 200;

    if (distance < 20) {
        Stop(300);
        Reverse(200);
        Stop(300);
        
        distanceRight = lookRight(); 
        distanceLeft = lookLeft();

        if (distanceLeft < 20 && distanceRight < 20) {
            for (int Start = millis(); (millis() - Start) < motion_duration; ) {
            Reverse(100);
            }
        }
        
        else if (distanceLeft > distanceRight) {
            for (int Start = millis(); (millis() - Start) < motion_duration; ) {
            turnLeft(100);
            }
        }
    
        else if (distanceRight > distanceLeft) {
            for (int Start = millis(); (millis() - Start) < motion_duration; ) {
            turnRight(100);
            }
        }
    } 
}

/*
 * Motor movements logic
 * 
 * STOP -     turn off motors.
 * FORWARD -  turn on forward direction outputs (IN1)
 * REVERSE -  turn on backward direction outputs (IN2)
 * TURNLEFT - turn on forward direction for right motor (IN1)
 *            while left motor is off
 * TURNRIGHT -turn on forward direction for left motor (IN1)
 *            while right motor is off
 */

void Stop(int delay_ms){
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    delay(delay_ms);
}

void Forward(int delay_ms){
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, HIGH);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, HIGH);
    delay(delay_ms);
}

void Reverse(int delay_ms){
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    Serial.println("REVERSE");
    delay(delay_ms);
}

void turnLeft(int delay_ms){
    digitalWrite(motorPin1, HIGH);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, LOW);
    digitalWrite(motorPin4, LOW);
    Serial.println("TURN LEFT");
    delay(delay_ms);
}

void turnRight(int delay_ms){
    digitalWrite(motorPin1, LOW);
    digitalWrite(motorPin2, LOW);
    digitalWrite(motorPin3, HIGH);
    digitalWrite(motorPin4, LOW);
    Serial.println("TURN RIGHT"); 
    delay(delay_ms);
}

int lookLeft() {
    /*
    * Calculate the distance of the left horizon.
    * 
    * Rotate servomotor to face left side.
    * Apply a delay and get its distance with getDistance function.
    * Apply a delay and return the servomotor back to equilibrium position.
    * Return the distance.
    */
    servo.write(150);
    delay(500);
    distance = getDistance(2, 5);
    delay(250);
    servo.write(90);
    return distance;
}

int lookRight() {
   /*
    * Calculate the distance of the right horizon.
    * 
    * Rotate servomotor to face right side.
    * Apply a delay and get its distance with getDistance function.
    * Apply a delay and return the servomotor back to equilibrium position.
    * Return the distance.
    */
    servo.write(10);
    delay(500);
    distance = getDistance(2, 5);
    delay(250);
    servo.write(90);
    return distance;
}


int getDistance(int delay_1, int delay_2) {  
   /*
   * ULTRASONIC SENSOR logic
   * 
   * delay_1 (int) - delay in ms before trigerring a pulse
   * delay_2 (int) - delay in ms before stop sending a pulse
   * 
   * trigPin -> Send signals at speed of sound (343 m/s)
   * echoPin -> Receive signals as a duration of time elapsed
   * 
   * Initialize trigPin to NOT send a signal.
   * Apply a delay and SEND a signal.
   * Apply a delay and TURN OFF the trigPin.
   * Measure the time elapsed and calculate the distance using d = vt.
   * Return the distance.
   */
    digitalWrite(trigPin, LOW);
    delay(delay_1);
    digitalWrite(trigPin, HIGH);
    delay(delay_2);
    digitalWrite(trigPin, LOW);
    _duration = pulseIn(echoPin, HIGH);
    _distance = _duration*0.034/2;
    return _distance;
}