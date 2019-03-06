#pragma once

#include <opencv2/core/core.hpp>
#include <opencv2/videoio.hpp>

#include <mutex>
#include <thread>

class StereoCameraOpenCV {
public:

  StereoCameraOpenCV(const std::string & left_camera_path,
                     const std::string & right_camera_path);

  /// Connect the cameras
  void connect();

  /// Disconnect cameras
  void disconnect();

  /// Start capture
  void startCapture();

  /// Lock capture
  void endCapture();

  /// Update properties: currently, only fps is set
  void updateProperties();

  /// Retrieve the images from camers
  void getImages(cv::Mat * left_img, cv::Mat * right_img);

  /// Capture images until
  void captureLoop();

private:
  /// Are both cameras connected?
  bool is_connected;

  /// Is capture loop running
  bool is_capture_active;

  /// Unique identifiers for cameras
  std::string camera_paths[2];

  /// The flycapture cameras
  cv::VideoCapture cameras[2];

  /// Images received
  cv::Mat imgs[2];

  /// Access to the main images
  std::mutex mutex;

  /// The thread retrieving the images
  std::thread bg_thread;
};
