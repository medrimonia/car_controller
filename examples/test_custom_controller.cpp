#include "custom_controller.h"

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <stdlib.h>

int main(int argc, char ** argv) {
  if (argc < 3) {
    std::cerr << "Usage: " << argv[0] << " <left_img_path> <right_img_path>" << std::endl;
    exit(EXIT_FAILURE);
  }
  // Store paths in explicit names
  std::string left_img_path(argv[1]);
  std::string right_img_path(argv[2]);
  // Read the two provided images
  cv::Mat left_img, right_img;
  left_img = cv::imread(left_img_path);
  right_img = cv::imread(right_img_path);
  // Initialize controller
  Controller * c =  new group_name::CustomController();
  // Load config file
  c->load();
  // Process two provided images
  float vx, vy,vTheta;
  c->process(left_img,right_img, &vx, &vy, &vTheta);
  // Show orders
  std::cout << "(vx, vy, vTheta): ("
            << vx << ", " << vy << ", " << vTheta << ")" << std::endl;

  delete(c);
}
