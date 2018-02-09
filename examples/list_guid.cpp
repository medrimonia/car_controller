#include "camera.h"

#include <iostream>
#include <sstream>

// Highly inpired by "MultipleCameraEx"

int main()
{
  FlyCapture2::Error error;

  FlyCapture2::BusManager busMgr;
	unsigned int numCameras;
	error = busMgr.GetNumOfCameras(&numCameras);
	if (error != FlyCapture2::PGRERROR_OK)
	{
		error.PrintErrorTrace();
		return -1;
	}

	std::cout << "Number of cameras detected: " << numCameras << std::endl;

  for (unsigned int i = 0; i < numCameras; i++) {
    FlyCapture2::PGRGuid guid;
		error = busMgr.GetCameraFromIndex( i, &guid );
		if (error != FlyCapture2::PGRERROR_OK)
		{
      error.PrintErrorTrace();
			return -1;
		}
    std::cout << "Guid for camera " << i << ": ";
    for (int i = 0; i < 4; i++) {
      std::cout << guid.value[i] << ",";
    }
    std::cout << std::endl;
  }
}
