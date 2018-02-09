#pragma once

#include "controller.h"

namespace group_name
{

class CustomController : public Controller
{
public:
  virtual ~CustomController();

  virtual void process(const cv::Mat & left_img,
                       const cv::Mat & right_img,
                       float * vx, float * vy, float * omega) override;

  virtual void load() override;
};

}
