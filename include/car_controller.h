#pragma once

#include <serial/serial.h>

class CarController : private serial::Serial
{
public:
  // Build a serial link with the car
  CarController(const std::string & port, int baudrate = 115200);

  // Send a command to update speed to the robot
  // vx: speed forward [m/s]
  // vy: lateral speed [m/s] (+ is left)
  // omega: angular speed [rad/s] (+ is rotating to left)
  void setSpeed(float vx, float vy, float omega);
};
