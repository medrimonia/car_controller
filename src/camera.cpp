#include "camera.h"

#include <iostream>
#include <unistd.h>

Camera::Camera() : is_capturing(false) {}

cv::Mat Camera::getNextImg() {
  /// Issues have been experimented without sleep
  usleep(5 * 1000); // 5 ms sleep

  FlyCapture2::Image fc_image;
  // If capture is not activated yet, start camera
  if (!is_capturing) {
    startCamera();
  }
  // Grab frame from camera
  FlyCapture2::Error error = camera.RetrieveBuffer(&fc_image);
  if (error != FlyCapture2::PGRERROR_OK) {
    std::cerr
        << "SourcePtGrey::process: Failed to retrieve buffer: error type: "
        << error.GetType() << std::endl;
    std::cerr << "Error description: " << error.GetDescription() << std::endl;
    throw std::runtime_error("Camera::getImg: Failed to retrieveBuffer");
  }
  // Convert image to openCV
  unsigned int bytes_per_row = fc_image.GetReceivedDataSize() / fc_image.GetRows();
  cv::Mat tmp_img = cv::Mat(fc_image.GetRows(), fc_image.GetCols(), CV_8UC3,
                            fc_image.GetData(), bytes_per_row).clone();
  invertChannels(tmp_img);
  return tmp_img;
}

void Camera::startCamera() {
  FlyCapture2::Error error;
  // If connected, disconnect
  if (camera.IsConnected()) {
    camera.Disconnect();
  }
  // Connect to camera
  error = camera.Connect(0);//&camera_id);
  if (error != FlyCapture2::PGRERROR_OK) {
    throw std::runtime_error(
        "Camera::StartCamera: Failed to connect to camera");
  }
  // Properly set up size of packet and delay between packets
  updatePacketProperties();
  // Set appropriate mode and size
  updateImageSettings();
  // Set camera properties
  updateProperties();
  // Start capture
  while (true) {
    error = camera.StartCapture();
    if (error == FlyCapture2::PGRERROR_ISOCH_BANDWIDTH_EXCEEDED) {
      throw std::runtime_error("Camera::StartCamera: bandwidth exceeded");
    } else if (error == FlyCapture2::PGRERROR_ISOCH_ALREADY_STARTED) {
      std::cerr << "Camera::StartCamera: Isoch already started: stopping"
                << std::endl;
      camera.StopCapture();
    } else if (error != FlyCapture2::PGRERROR_OK) {
      std::cerr << "Camera::StartCamera: Failed to start image capture"
                << std::endl;
      std::cerr << "ErrorType: " << error.GetType() << std::endl;
      reconnectCamera();
    } else {
      break;
    }
  }
  is_capturing = true;
  // TODO: Update internal properties
  // updateProperties();
  // updatePropertiesInformation();
  // TODO: Apply wished properties
  // applyWishedProperties();
  // TODO: Debug, dump Properties and informations
  // dumpProperties(std::cout);
  // dumpPropertiesInformation(std::cout);
}

void Camera::endCamera() {
  FlyCapture2::Error error;
  if (is_capturing) {
    camera.StopCapture();
    if (error != FlyCapture2::PGRERROR_OK) {
      throw std::runtime_error(
          "Camera::endCamera: Failed to stop capture!");
    }
    is_capturing = false;
  }
  if (camera.IsConnected()) {
    camera.Disconnect();
  }
}

void Camera::reconnectCamera() {
  FlyCapture2::Error error;
  // If connected, disconnect
  if (camera.IsConnected()) {
    camera.Disconnect();
  }
  // Connect to camera
  error = camera.Connect(&camera_id);
  if (error != FlyCapture2::PGRERROR_OK) {
    throw std::runtime_error(
        "Camera::StartCamera: Failed to connect to camera");
  }
}

void Camera::updateImageSettings() {
  // Error variable
  FlyCapture2::Error error;
  // Setting Imaging Mode
  error = camera.SetGigEImagingMode(FlyCapture2::Mode::MODE_1);
  if (error != FlyCapture2::ErrorType::PGRERROR_OK) {
    throw std::runtime_error(
        "Camera::updateImageSettings: Error setting imaging mode");
  }
  // Preparing binning
  unsigned int h_binning = 2;
  unsigned int v_binning = 2;
  // Setting binning
  error = camera.SetGigEImageBinningSettings(h_binning, v_binning);
  if (error != FlyCapture2::ErrorType::PGRERROR_OK) {
    throw std::runtime_error(
        "Camera::updateImageSettings: Error setting binning settings");
  }
  // Preparing properties
  struct FlyCapture2::GigEImageSettings image_settings;
  image_settings.offsetX = 0;
  image_settings.offsetY = 0;
  image_settings.width = 640;
  image_settings.height = 480;
  image_settings.pixelFormat = FlyCapture2::PixelFormat::PIXEL_FORMAT_444YUV8;
  // Setting properties
  error = camera.SetGigEImageSettings(&image_settings);
  if (error != FlyCapture2::ErrorType::PGRERROR_OK) {
    throw std::runtime_error(
        "Camera::updateImageSettings: Error setting image settings");
  }
}

void Camera::updatePacketProperties() {
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
  packet_delay_prop.value = 40000;

  // Send packets
  FlyCapture2::Error error;
  error = camera.SetGigEProperty(&packet_size_prop);
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
  error = camera.SetGigEProperty(&packet_delay_prop);
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

void Camera::updateProperties() {
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
  fps_property.absValue = 2;  
  // Applying fps_property
  FlyCapture2::Error error;
  error = camera.SetProperty(&fps_property, false);
  if (error != FlyCapture2::PGRERROR_OK) {
    throw std::runtime_error(error.GetDescription());
  }
}

void Camera::invertChannels(cv::Mat & frame) {
  uint8_t *data = (uint8_t *)frame.data;

  for (int i = 0; i < frame.cols * frame.rows; i++) {
    // According to: https://www.ptgrey.com/KB/10092
    // Format of PtGrey is UYV, but definition of U and V might be swapped
    uint8_t V = data[3 * i + 0];
    uint8_t Y = data[3 * i + 1];
    uint8_t U = data[3 * i + 2];
    data[3 * i + 0] = Y;
    data[3 * i + 1] = U;
    data[3 * i + 2] = V;
  }
}
