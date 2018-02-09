#include "fake_controller.h"

void FakeController::process(const cv::Mat & left_img,
                            const cv::Mat & right_img,
                            float * vx, float * vy, float * omega) {
  // Fake does not use images
  (void) left_img; (void) right_img;
  // Just keep static
  *vx = 0;
  *vy = 0;
  *omega = 0;
}
