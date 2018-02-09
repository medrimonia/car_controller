#include "custom_controller.h"

namespace group_name
{

CustomController::~CustomController() {
  // Do not forget to move any memory owned by your controller
}

void CustomController::process(const cv::Mat & left_img,
                               const cv::Mat & right_img,
                               float * vx, float * vy, float * omega) {
  // Fake does not use images
  (void) left_img; (void) right_img;
  // Just keep static
  *vx = 0;
  *vy = 0;
  *omega = 0;
}

void CustomController::load() {
  // This function can be used to load internal parameters of the controller
  // without requiring compilation
}

}
