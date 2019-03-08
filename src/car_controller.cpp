#include "car_controller.h"

#include <algorithm>

// Note: Communication protocol is extrapolated from what is available at api/fivebot.py
//       from repository http://www.github.com/RemiFabre/FiveBot


// Protocol values
char STX = 2;
char ETX = 3;
char ESC = 27;

// Since some 'byte' have a specific meaning, it is forbidden to use them, thus,
// if we want to write them, we replace them by two characters:
// 1: 'ESC'
// 2: byte ^ 'ESC'
// Return the number of bytes written
static int writeEscapedByte(char * dst, char byte) {
  if (byte == ESC || byte == STX || byte == ETX) {
    dst[0] = ESC;
    dst[1] = byte ^ ESC;
    return 2;
  }
  dst[0] = byte;
  return 1;
}

// Return the number of bytes written
static int writeEscapedBytes(char * dst, char * bytes, int nb_bytes) {
  int bytes_written = 0;
  for (int byte_no = 0; byte_no < nb_bytes; byte_no++) {
    bytes_written += writeEscapedByte(dst + bytes_written, bytes[byte_no]);
  }
  return bytes_written;
}

// Write the data and return number of bytes written
static int writeFloat(char * dst, float val) {
  return writeEscapedBytes(dst, reinterpret_cast<char*>(&val), sizeof(float));
}

static int writeBool(char * dst, bool val) {
  return writeEscapedBytes(dst, reinterpret_cast<char*>(&val), sizeof(bool));
}

CarController::CarController(const std::string & port, int baudrate)
  : serial::Serial(port, baudrate, serial::Timeout::simpleTimeout(1000))
{
}


void CarController::setSpeed(float vx, float vy, float omega)
{
  // Bounding speeds (Currently done in a brutal way)
  float max_speed = 0.3;
  vx = std::min(max_speed, std::max(-max_speed, vx));
  vy = std::min(max_speed, std::max(-max_speed, vy));
  omega = std::min(max_speed, std::max(-max_speed, omega));
  
  // Ratios used for conversion between robot speed and values used for robot
  // control
  // TODO: those values are experimental, they definitely need to be corrected
  double vx_ratio = -20;
  double vy_ratio = -20;
  double omega_ratio = 20;
  // Changing signs for speed
  vx = vx_ratio * vx;
  vy = vy_ratio * vy;
  omega = omega_ratio * omega;
  // Message writing
  char msg[100];// Size is large enough
  int byte_count = 0;
  // Writing header
  msg[byte_count] = STX;// prefix
  byte_count++;
  // Prefix for set_speed
  msg[byte_count] = 's';
  byte_count++;
  // Writing vx
  byte_count += writeFloat(msg+byte_count, vx);
  // Writing vy
  byte_count += writeFloat(msg+byte_count, vy);
  // Writing omega
  byte_count += writeFloat(msg+byte_count, omega);
  // Writing bypassPID
  bool bypassPID = false;
  byte_count += writeBool(msg+byte_count, bypassPID);
  // Write message ending
  msg[byte_count] = ETX;
  byte_count++;

  // Now that message is formatted, write it on serial
  size_t written = byte_count;
  write(reinterpret_cast<uint8_t *>(msg), written);
}
