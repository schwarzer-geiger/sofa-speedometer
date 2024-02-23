#include "SwitecX25.h"

// 315 degrees of range = 315x3 steps = 945 steps
// declare motor1 with 945 steps on pins 4, 5, 6, 9
SwitecX25 motor1(315*3, 4,5,6,9);
int sensorPin = 10;

int high_threshold = 800;
int low_threshold = 200;

// 1 if signal was active at last reading, 0 if signal was not active yet at last reading
int high_reset = 0;
int low_reset = 0;
int ms_current;
int ms_last = millis();
int ms_passed;
int speed_current;
int position_next;

// number of degrees between mileage indication on dial
float deg_per_mile = 2;
// number of steps to pass one degree
int steps_per_deg = 3;
// how many steps the stepper has to move to increase the dial reading by one mile
int steps_per_mile = deg_per_mile * steps_per_deg;
// target position of needle in steps
int targetPos;

// diameter of the tire in cm
float dia_tire = 38;
// number of magnets attached equispaced to the wheel
int num_of_magnets = 2;
// circumference of the tire
float circ_tire;
// circumference passed between two magnets
float circ_mag;
// unrounded current speed in mph
float speed_mph;
// rounded up current speed in mph
int speed_mph_rounded_up;

void setup(void) {
  Serial.begin(9600);

  circ_tire = 3.14159 * dia_tire;
  circ_mag = circ_tire / num_of_magnets;

  // run motor against stops to re-zero
  motor1.zero();
  //  motor1.setPosition(425);
}

// returns speed in mph, requires ms between last two sensor pulses
int getSpeed(int pulseInterval) {
  speed_mph = 0.044704 * (circ_mag / pulseInterval);
  speed_mph_rounded_up = (int(speed_mph)) + 1;
  return speed_mph_rounded_up;
}

// returns needle position corresponding to current speed in mph (rounded up), requires current speed in mph (rounded up)
int getPosition(int speed) {
  targetPos = speed * steps_per_mile;
  return targetPos;
}

void loop() {
  motor1.update();
  if (analogRead(sensorPin) < low_threshold && !low_reset) {
    low_reset = 1;
    high_reset = 0;
  } else if (analogRead(sensorPin) > low_threshold && analogRead(sensorPin) < high_threshold && low_reset) {
    low_reset = 0;
  } else if (analogRead(sensorPin) >= high_threshold && !high_reset) {
    high_reset = 1;
    ms_current = millis();
    ms_passed = ms_current - ms_last;
    speed_current = getSpeed(ms_passed);
    position_next = getPosition(speed_current);
    motor1.setPosition(position_next);
    ms_last = ms_current;
  }
}
