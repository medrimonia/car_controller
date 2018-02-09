#pragma once

#include "controller.h"

class FakeController : public Controller
{
public:
  virtual void process(const cv::Mat & left_img,
                       const cv::Mat & right_img,
                       float * vx, float * vy, float * omega) override;
};
