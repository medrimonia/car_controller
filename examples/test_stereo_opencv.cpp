#include "stereo_camera_opencv.h"

#include <opencv2/highgui/highgui.hpp>

#include <chrono>
#include <iostream>
#include <sstream>
#include <unistd.h>

using namespace std::chrono;

void saveImage(const cv::Mat & img, const std::string & side, int no)
{
  std::ostringstream oss;
  oss << side << "_" << no << ".png";
  cv::imwrite(oss.str(), img);
}

int main(int argc, char ** argv)
{
  if (argc != 4) {
    std::cerr << "Usage: " << argv[0] << " <left_camera> <right_camera> <fps>" << std::endl;
    exit(EXIT_FAILURE);
  }

  StereoCameraOpenCV sc(argv[1], argv[2]);

  double fps = std::stod(argv[3]);
  double period = 1. / fps;

  for (int image_id = 0; image_id < 10; image_id++) {
    cv::Mat left_img, right_img;
    steady_clock::time_point ask_img = steady_clock::now();
    sc.getImages(&left_img, &right_img);
    saveImage(left_img, "left", image_id);
    saveImage(right_img, "right", image_id);
    steady_clock::time_point now = steady_clock::now();
    double time_sec = duration_cast<duration<double>>(now - ask_img).count();
    double wait_time = period - time_sec;
    if (wait_time > 0) {
      usleep(wait_time * 1000 * 1000);
    }
  }
}
