#include "car_controller.h"

#include <unistd.h>

int main() {
  unsigned int sleep_time = 2;
  float speed = 3;
  
  CarController cc("/dev/ttyUSB0");

  // Start by static position during a given time
  cc.setSpeed(0,0,0);
  sleep(5);
  
  // Move forward during ... second
  cc.setSpeed(speed,0,0);
  sleep(sleep_time);

  // Move left during ... seconds
  cc.setSpeed(0,speed,0);
  sleep(sleep_time);

  // Rotate during ... seconds
  cc.setSpeed(0,0,speed);
  sleep(sleep_time);


  // Stop
  cc.setSpeed(0,0,0);
}
