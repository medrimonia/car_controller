cmake_minimum_required(VERSION 2.8)
project(car_controller)

find_package(OpenCV REQUIRED)

option(USE_FLYCAP OFF "Is flycapture available")

add_subdirectory(serial)

# OpenCV includes
include_directories(include ${OpenCV_DIR}/include)
link_directories(${OpenCV_DIR}/lib ${OpenCV_DIR}/3rdparty/lib)

# Serial includes (communication with the robot)
include_directories(serial/include)

#Enable C++11
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

#Enable real warnings
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra")

#Add Vision libraries
set (LINKED_LIBRARIES
  ${LINKED_LIBRARIES}
  ${OpenCV_LIBS}
  serial
  )

if (USE_FLYCAP)
  set (LINKED_LIBRARIES
    ${LINKED_LIBRARIES}
    flycapture)
endif(USE_FLYCAP)


#List Sources sub source directories
set(SOURCES_DIRECTORIES
  src
)

#Include Sources sub sources
foreach (DIRECTORY ${SOURCES_DIRECTORIES})
    message("-- Sources: Adding ${DIRECTORY}/")
    include (${DIRECTORY}/Sources.cmake)
    set (PREFIXED_SOURCES)
    foreach (SOURCE ${SOURCES})
        set (PREFIXED_SOURCES ${PREFIXED_SOURCES} ${DIRECTORY}/${SOURCE})
    endforeach (SOURCE)
    set (ALL_SOURCES ${ALL_SOURCES} ${PREFIXED_SOURCES})
endforeach (DIRECTORY)

#Build the RhobanServer main library
add_library (car_controller SHARED ${ALL_SOURCES})
target_link_libraries (car_controller ${LINKED_LIBRARIES})

#Build the test applications
if (USE_FLYCAP)
  add_executable (TestMono examples/test_mono.cpp)
  target_link_libraries (TestMono ${LINKED_LIBRARIES} car_controller)

  add_executable (TestStereo examples/test_stereo.cpp)
  target_link_libraries (TestStereo ${LINKED_LIBRARIES} car_controller)

  add_executable (ListGuid examples/list_guid.cpp)
  target_link_libraries (ListGuid ${LINKED_LIBRARIES} car_controller)

  add_executable (AutonomousCar examples/autonomous_car.cpp)
  target_link_libraries (AutonomousCar ${LINKED_LIBRARIES} car_controller)
endif(USE_FLYCAP)

add_executable (TestCar examples/test_car.cpp)
target_link_libraries (TestCar ${LINKED_LIBRARIES} car_controller)

add_executable (TestCustomController examples/test_custom_controller.cpp)
target_link_libraries (TestCustomController ${LINKED_LIBRARIES} car_controller)

add_executable (TestStereoOpenCV examples/test_stereo_opencv.cpp)
target_link_libraries (TestStereoOpenCV ${LINKED_LIBRARIES} car_controller)

add_executable (AutonomousCarOpenCV examples/autonomous_car_opencv.cpp)
target_link_libraries (AutonomousCarOpenCV ${LINKED_LIBRARIES} car_controller)
