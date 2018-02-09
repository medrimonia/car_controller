#pragma once

#include <opencv2/core/core.hpp>
#include <flycapture/FlyCapture2.h>

class StereoCamera {
public:

  StereoCamera(FlyCapture2::PGRGuid left_cam_guid,
               FlyCapture2::PGRGuid right_cam_guid);

  /// Connect the cameras
  void connect();

  /// Disconnect cameras
  void disconnect();

  /// Start synchronized capture on both cameras
  void startCapture();

  /// Stop capture on both cameras
  void stopCapture();

  /// Update the imaging mode, binning and PixelFormat of the camera
  void updateImageSettings();

  /// Update the packet size and the packet delay to reduce image
  /// inconsistencies
  /// NOTE: this has to be done while the camera is not capturing
  void updatePacketProperties();

  /// Update properties: currently, only fps is set
  void updateProperties();

  /// Retrieve the images from camers
  void getImages(cv::Mat * left_img, cv::Mat * right_img);

private:
  /// Are both cameras connected?
  bool is_connected;

  /// Are both cameras properly updating?
  bool is_capturing;

  /// Unique identifiers for cameras
  /// guids[0] = left_cam_guid
  /// guids[1] = right_cam_guid
  FlyCapture2::PGRGuid guids[2];

  /// The flycapture cameras
  FlyCapture2::GigECamera cameras[2];
};
