#pragma once
#include <fstream>
#include <cstdint>
#include <vector>
#include "video.hpp"

std::vector<uint8_t> serialize_video(const Video& video);
Video deserialize_video(const std::vector<uint8_t>& video_data);

