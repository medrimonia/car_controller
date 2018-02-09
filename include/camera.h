#pragma once

#include <opencv2/core/core.hpp>
#include <flycapture/FlyCapture2.h>

/// How to configure the camera to have a static ip:
/// https://www.ptgrey.com/KB/10933

class Camera {
public:

  /// Default constructor
  Camera();

  /// Get the next image from the camera, if necessary, restart the camera
  cv::Mat getNextImg();

  /// Connect to camera and start capture
  void startCamera();

  /// End capture and close connection to camera
  void endCamera();

  /// Cut current connection if active, then start a new connection
  void reconnectCamera();

  /// Update the imaging mode, binning and PixelFormat of the camera
  void updateImageSettings();

  /// Update the packet size and the packet delay to reduce image
  /// inconsistencies
  /// NOTE: this has to be done while the camera is not capturing
  void updatePacketProperties();

  /// Update properties: currently, only fps is set
  void updateProperties();

  // Invert channels to get a proper image with openCV definitions
  void invertChannels(cv::Mat & image);

private:
  /// The ethernet properties
  FlyCapture2::GigECamera camera;

  /// The specific ID of the camera, defined by 4 unsigned ints
  FlyCapture2::PGRGuid camera_id;

  /// Has the camera properly strted to capture images
  bool is_capturing;
};
