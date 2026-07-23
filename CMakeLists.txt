cmake_minimum_required(VERSION 3.10)
project(VideoUploading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Main executable
add_executable(main
    Database/main.cpp
    Database/video_serializer.cpp
)

# Test executable
add_executable(tests
    Database/tests/test_serializer.cpp
    Database/video_serializer.cpp
)