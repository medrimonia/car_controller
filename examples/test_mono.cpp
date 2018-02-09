#include "camera.h"

#include <opencv2/highgui/highgui.hpp>

int main() {
  Camera c;

  int nb_images = 0;
  while (nb_images < 10) {
    cv::Mat image = c.getNextImg();
    std::ostringstream oss;
    oss << "image" << nb_images << ".png";
    cv::imwrite(oss.str(),image);
    nb_images++;
  }
}
