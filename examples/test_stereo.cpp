#include "stereo_camera.h"

#include <opencv2/highgui/highgui.hpp>

#include <iostream>
#include <sstream>

void saveImage(const cv::Mat & img, const std::string & side, int no)
{
  std::ostringstream oss;
  oss << side << "_" << no << ".png";
  cv::imwrite(oss.str(), img);
}

int main()
{
  FlyCapture2::PGRGuid left_guid, right_guid;
  // Left camera GUID
  left_guid.value[0] = 2022556694;
  left_guid.value[1] = 506125062;
  left_guid.value[2] = 2627711216;
  left_guid.value[3] = 2463930288;
  // Right camera GUID
  right_guid.value[0] = 932990310;
  right_guid.value[1] = 323408622;
  right_guid.value[2] = 1096954096;
  right_guid.value[3] = 2825689008;

  StereoCamera sc(left_guid, right_guid);

  for (int image_id = 0; image_id < 10; image_id++) {
    cv::Mat left_img, right_img;
    sc.getImages(&left_img, &right_img);
    saveImage(left_img, "left", image_id);
    saveImage(right_img, "right", image_id);
  }
}
