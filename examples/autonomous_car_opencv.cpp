#include "car_controller.h"
#include "stereo_camera_opencv.h"

#include "fake_controller.h"
#include "traj_controller.h"

#include <opencv2/highgui/highgui.hpp>

#include <chrono>
#include <functional>
#include <iostream>
#include <map>
#include <sstream>

#include <stdlib.h>
#include <unistd.h>

using namespace std::chrono;

std::map<std::string, std::function<Controller*()> > builders =
{
  {"fake", [](){return new FakeController();}},
  {"traj", [](){return new TrajController();}}
};

void usage(char ** argv)
{
  std::cerr << "Usage: " << argv[0] << " <analyzer_name> <nb_images> <frequency> <left_camera> <right_camera>" << std::endl;
  exit(EXIT_FAILURE);
}

int main(int argc, char ** argv)
{
  // Checking args
  if (argc < 6) {
    usage(argv);
  }

  // Reading config from args
  std::string analyzer_name = argv[1];
  int nb_images = atoi(argv[2]);
  float frequency = atof(argv[3]);
  std::string left_camera_path = argv[4];
  std::string right_camera_path = argv[5];

  Controller * controller;

  try {
    controller = builders.at(analyzer_name)();
  }
  catch (const std::out_of_range & exc) {
    std::cerr << "Cannot find '" << analyzer_name << "' among analyzers" << std::endl;
    exit(EXIT_FAILURE);
  }

  // Computing period
  float period_us = 1000 * 1000 / frequency;

  // Initialize car
  CarController cc("/dev/ttyUSB0");
  std::cout << "Connexion with car is established" << std::endl;

  StereoCameraOpenCV sc(left_camera_path, right_camera_path);
  std::cout << "Starting capture" << std::endl;
  sc.startCapture();
  std::cout << "Connexion with cameras established" << std::endl;

  // Initializing command
  cc.setSpeed(0,0,0);
  sleep(1);

  for (int image_id = 0; image_id < nb_images; image_id++) {
    time_point<steady_clock> tick_start, tick_end;
    tick_start = steady_clock::now();

    // Retrieving images from camera
    cv::Mat left_img, right_img;
    sc.getImages(&left_img, &right_img);

    // Analyzing images and sending command to car controller
    float vx, vy, omega;
    controller->process(left_img, right_img, &vx, &vy, &omega);
    cc.setSpeed(vx, vy, omega);

    // Computing elapsed time
    tick_end = steady_clock::now();
    double elapsed_ticks = (tick_end - tick_start).count();
    double elapsed_secs = elapsed_ticks * steady_clock::period::num / steady_clock::period::den;
    int elapsed_us = (int)(elapsed_secs * 1000*1000);

    // Computing sleeping time
    int sleeping_us = period_us - elapsed_us;

    // Sleeping
    if (sleeping_us > 0) {
      std::cout << "\tSleeping for " << sleeping_us << " microseconds" << std::endl;
      usleep(sleeping_us);
    }
  }

  // Stop
  cc.setSpeed(0,0,0);
}
