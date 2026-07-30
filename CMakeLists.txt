cmake_minimum_required(VERSION 3.10)
project(VideoUploading)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(Database/include)

add_executable(main
    Database/src/main.cpp
    Database/src/video_serializer.cpp
    Database/src/pager.cpp
)

add_executable(serializer_tests
    Database/tests/test_serializer.cpp
    Database/src/video_serializer.cpp
)

add_executable(pager_tests
    Database/tests/test_pager.cpp
    Database/src/pager.cpp
)