#include "custom_controller.h"

#include <iostream>
#include <stdlib.h>

int main(int argc, char ** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <left_img_path> <right_img_path>" << std::endl;
    exit(EXIT_FAILURE);
  }
  // Store paths in explicit names
  std::string left_img_path(argv[1]);
  std::string left_img_path(argv[2]);
  // Read the two provided images
  cv::Mat left_img, right_img;
  cv::imread(left_img_path,left_img);
  cv::imread(right_img_path,right_img);
  // Initialize controller
  Controller * c =  new CustomController();
  // Load config file
  c.loadFromFile("config");
  // Process two provided images
  double vx, vy,vTheta;
  c.process(left_img,right_img, &vx, &vx, &vTheta);
  // Show orders
  std::cout << "(vx, vy, vTheta): (" << vx << ", " << vy << ", " vTheta << ")" << std::endl;
}
