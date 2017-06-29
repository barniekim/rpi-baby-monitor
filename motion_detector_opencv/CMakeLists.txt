
find_package( OpenCV REQUIRED )

MESSAGE("OpenCV information:") 
MESSAGE("  OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}") 
MESSAGE("  OpenCV_LIBRARIES: ${OpenCV_LIBRARIES}") 
MESSAGE("  OpenCV_LIBRARY_DIRS: ${OpenCV_LIBRARY_DIRS}") 
include_directories( ${OpenCV_INCLUDE_DIRS} )

include_directories(./)
add_executable(motion_detector motion_detector.cpp)
target_link_libraries(motion_detector ${OpenCV_LIBS})

install (TARGETS motion_detector DESTINATION bin)
