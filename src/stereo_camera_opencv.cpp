#include "stereo_camera_opencv.h"

#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>
#include <unistd.h>

StereoCameraOpenCV::StereoCameraOpenCV(const std::string & left_camera_path,
                                       const std::string & right_camera_path)
  : is_connected(false)
{
  camera_paths[0] = left_camera_path;
  camera_paths[1] = right_camera_path;
}

void StereoCameraOpenCV::connect()
{
  if (is_connected) {
    throw std::logic_error("StereoCameraOpenCV::connect: cameras are already connected");
  }
  for (unsigned int id = 0; id < 2; id++) {
    std::cout << "trying to connect to camera with id: " << id << std::endl;
    cameras[id].open(camera_paths[id]);
    if (!cameras[id].isOpened()) {
      throw std::runtime_error("Failed to open video stream from '" + camera_paths[id] + "'");
		}
  }
  is_connected = true;
}

void StereoCameraOpenCV::disconnect()
{
  if (!is_connected) {
    throw std::logic_error("StereoCameraOpenCV::disconnect: camera are not connected");
  }
  for (unsigned int id = 0; id < 2; id++) {
    std::cout << "trying to disconnect from camera with id: " << id << std::endl;
		cameras[id].release();
  }
  is_connected = false;
}

void StereoCameraOpenCV::startCapture() {
  if (!is_connected) connect();
  updateProperties();
  is_capture_active = true;
  bg_thread = std::thread([this](){this->captureLoop();});
}

void StereoCameraOpenCV::endCapture() {
  is_capture_active = false;
  bg_thread.join();
}

void StereoCameraOpenCV::updateProperties() {
  if (!is_connected) connect();
  for (unsigned int camera_id = 0; camera_id < 2; camera_id++) {
//    double fps = 2.0;
//    bool success = cameras[camera_id].set(cv::CAP_PROP_FPS, fps);
//    if (!success) {
//      throw std::runtime_error("Failed to set FPS on camera '" + camera_paths[camera_id] + "'");
//    }
  }
}

void StereoCameraOpenCV::getImages(cv::Mat * left_img, cv::Mat * right_img)
{
  // Ensure we are capturing
  if (!is_capture_active) startCapture();
  while (imgs[0].data == nullptr || imgs[1].data == nullptr) {
    usleep(5000);// Sleep 5 ms while both images are not ready
  }
  std::lock_guard<std::mutex> lock(mutex);
  *left_img = imgs[0].clone();
  *right_img = imgs[1].clone();
}

void StereoCameraOpenCV::captureLoop()
{
  while(is_capture_active) {
    cv::Mat tmp_imgs[2];
    for (int idx = 0; idx < 2; idx++) {
      cameras[idx] >> tmp_imgs[idx];
    }
    std::lock_guard<std::mutex> lock(mutex);
    for (int idx = 0; idx < 2; idx++) {
      imgs[idx] = tmp_imgs[idx];
    }
  }
}
