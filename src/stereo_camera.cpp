#include "stereo_camera.h"

#include "opencv2/imgproc/imgproc.hpp"

#include <iostream>

StereoCamera::StereoCamera(FlyCapture2::PGRGuid left_cam_guid,
                           FlyCapture2::PGRGuid right_cam_guid)
  : is_connected(false), is_capturing(false)
{
  guids[0] = left_cam_guid;
  guids[1] = right_cam_guid;
}

void StereoCamera::connect()
{
  if (is_connected) {
    throw std::logic_error("StereoCamera::connect: camera are already connected");
  }
  for (unsigned int id = 0; id < 2; id++) {
    FlyCapture2::Error error;
		error = cameras[id].Connect(&(guids[id]));
		if (error != FlyCapture2::PGRERROR_OK)
		{
      //TODO: disconnect all cameras which have been connected on failure
      error.PrintErrorTrace();
      exit(EXIT_FAILURE);
		}
  }
  is_connected = true;
}

void StereoCamera::disconnect()
{
  if (!is_connected) {
    throw std::logic_error("StereoCamera::disconnect: camera are not connected");
  }
  for (unsigned int id = 0; id < 2; id++) {
		cameras[id].Disconnect();
  }
  is_connected = false;
}

void StereoCamera::startCapture()
{
  if (is_capturing) {
    throw std::logic_error("StereoCamera::startCapture: cameras are already capturing");
  }
  if (!is_connected) {
    connect();
  }
  updatePacketProperties();
  updateImageSettings();
  updateProperties();
  // StartSyncCapture with GigE Cameras is not support (Cf GigECamera.h):
  // - Another solution needs to be found
  //FlyCapture2::Error error;
	//error = FlyCapture2::Camera::StartSyncCapture(2, (const FlyCapture2::Camera**)cameras);
	//if (error != FlyCapture2::PGRERROR_OK) {
  //  error.PrintErrorTrace();
  //  exit(EXIT_FAILURE);
  //
  for (unsigned int id = 0 ; id < 2; id++) {
    FlyCapture2::Error error;
    error = cameras[id].StartCapture();
    if (error != FlyCapture2::PGRERROR_OK) {
      error.PrintErrorTrace();
      exit(EXIT_FAILURE);
    }
  }
  is_capturing = true;
}

void StereoCamera::stopCapture()
{
  if (!is_capturing) {
    throw std::logic_error("StereoCamera::stopCapture: cameras are not capturing");
  }
  for (unsigned int id = 0; id < 2; id++) {
		cameras[id].StopCapture();
  }
  is_capturing = false;
}

void StereoCamera::updateImageSettings() {
  if (!is_connected) connect();
  // Error variable
  FlyCapture2::Error error;
  // Setting Imaging Mode
  for (unsigned int camera_id = 0; camera_id < 2; camera_id++) {
    error = cameras[camera_id].SetGigEImagingMode(FlyCapture2::Mode::MODE_1);
    if (error != FlyCapture2::ErrorType::PGRERROR_OK) {
      throw std::runtime_error(
        "Camera::updateImageSettings: Error setting imaging mode");
    }
  }
  // Preparing binning
  unsigned int h_binning = 2;
  unsigned int v_binning = 2;
  // Setting binning
  for (unsigned int camera_id = 0; camera_id < 2; camera_id++) {
    error = cameras[camera_id].SetGigEImageBinningSettings(h_binning, v_binning);
    if (error != FlyCapture2::ErrorType::PGRERROR_OK) {
      throw std::runtime_error(
        "Camera::updateImageSettings: Error setting binning settings");
    }
  }
  // Preparing properties
  struct FlyCapture2::GigEImageSettings image_settings;
  image_settings.offsetX = 0;
  image_settings.offsetY = 0;
  image_settings.width = 640;
  image_settings.height = 480;
  image_settings.pixelFormat = FlyCapture2::PixelFormat::PIXEL_FORMAT_RGB8;
  // Setting properties
  for (unsigned int camera_id = 0; camera_id < 2; camera_id++) {
    error = cameras[camera_id].SetGigEImageSettings(&image_settings);
    if (error != FlyCapture2::ErrorType::PGRERROR_OK) {
      error.PrintErrorTrace();
      throw std::runtime_error(
        "Camera::updateImageSettings: Error setting image settings");
    }
  }
}

void StereoCamera::updatePacketProperties() {
  if (!is_connected) connect();
  // Prepare packets
  FlyCapture2::GigEProperty packet_size_prop;
  packet_size_prop.propType = FlyCapture2::PACKET_SIZE;
  // Using larger packets reduces the number of CPU interruptions
  packet_size_prop.value = 9000;
  FlyCapture2::GigEProperty packet_delay_prop;
  packet_delay_prop.propType = FlyCapture2::PACKET_DELAY;
  // In 640*480, at 50 fps, with 3 bytes per pixel and a packet size of 9000:
  // - value should be around 2000
  // For in depth description of this parameters, refer to the Technical
  // reference guide of BFLY
  packet_delay_prop.value = 10000;

  // Send packets
  FlyCapture2::Error error;
  for (unsigned int camera_id = 0; camera_id < 2; camera_id++) {
    error = cameras[camera_id].SetGigEProperty(&packet_size_prop);
    if (error != FlyCapture2::PGRERROR_OK) {
      std::ostringstream oss;
      oss << "Camera::updatePacketProperties: failed to set packet size: "
          << error.GetType();
      {
        oss << ": Camera was capturing, stop capturing before changing packet "
          "size";
      }
      throw std::runtime_error(oss.str());
    }
    error = cameras[camera_id].SetGigEProperty(&packet_delay_prop);
    if (error != FlyCapture2::PGRERROR_OK) {
      std::ostringstream oss;
      oss << "Camera::updatePacketProperties: failed to set packet delay";
      if (is_capturing) {
        oss << ": Camera was capturing, stop capturing before changing packet "
          "size";
      }
      throw std::runtime_error(oss.str());
    }
  }
}

void StereoCamera::updateProperties() {
  if (!is_connected) connect();
  // Preparing fps_property structure
  FlyCapture2::Property fps_property;
  fps_property.type = FlyCapture2::FRAME_RATE;
  fps_property.present = true;
  fps_property.absControl = true;
  fps_property.onePush = false;
  fps_property.onOff = true;
  fps_property.autoManualMode = false;
  fps_property.valueA = 0;
  fps_property.valueB = 0;
  fps_property.absValue = 10;
  // Preparing shutter property
  FlyCapture2::Property shutter_property;
  shutter_property.type = FlyCapture2::SHUTTER;
  shutter_property.present = true;
  shutter_property.absControl = true;
  shutter_property.onePush = false;
  shutter_property.onOff = true;
  shutter_property.autoManualMode = false;
  shutter_property.valueA = 0;
  shutter_property.valueB = 0;
  shutter_property.absValue = 10;
  /// Preparing gain property
  FlyCapture2::Property gain_property;
  gain_property.type = FlyCapture2::GAIN;
  gain_property.present = true;
  gain_property.absControl = true;
  gain_property.onePush = false;
  gain_property.onOff = true;
  gain_property.autoManualMode = false;
  gain_property.valueA = 0;
  gain_property.valueB = 0;
  gain_property.absValue = -0.5;
  // Applying properties
  FlyCapture2::Error error;
  for (const FlyCapture2::Property & property : {fps_property, shutter_property, gain_property}) {
    for (unsigned int camera_id = 0; camera_id < 2; camera_id++) {
      error = cameras[camera_id].SetProperty(&property, false);
      if (error != FlyCapture2::PGRERROR_OK) {
        throw std::runtime_error(error.GetDescription());
      }
    }
  }
}

void StereoCamera::getImages(cv::Mat * left_img, cv::Mat * right_img)
{
  // Ensure we are capturing
  if (!is_capturing) startCapture();
  // Retrieving and converting images
  cv::Mat images[2];
  for (unsigned int id = 0; id < 2; id++) {
    FlyCapture2::Image fc_img;
    FlyCapture2::Error error;
    error = cameras[id].RetrieveBuffer(&fc_img);
    if (error != FlyCapture2::PGRERROR_OK) {
      error.PrintErrorTrace();
      exit(EXIT_FAILURE);
    }
    FlyCapture2::TimeStamp timestamp = fc_img.GetTimeStamp();
    // TODO: Synchronize cameras
//    // DEBUG: showing internal timestamps
//    std::cout << "Cam " << id
//              << ": Timestamp: [" << timestamp.cycleSeconds
//              << " " << timestamp.cycleCount << "]" << std::endl;
   
    unsigned int bytes_per_row = fc_img.GetReceivedDataSize() / fc_img.GetRows();
    cv::Mat rgb_img = cv::Mat(fc_img.GetRows(), fc_img.GetCols(), CV_8UC3,
			      fc_img.GetData(), bytes_per_row).clone();
    cv::cvtColor(rgb_img, images[id], CV_RGB2BGR);
  }
  *left_img = images[0];
  *right_img = images[1];
}
