set(SOURCES
  car_controller.cpp
  fake_controller.cpp
  traj_controller.cpp
  custom_controller.cpp
  stereo_camera_opencv.cpp
)

if (USE_FLYCAP)
  set (SOURCES
    ${SOURCES}
    camera.cpp
    stereo_camera.cpp
    )
endif(USE_FLYCAP)
