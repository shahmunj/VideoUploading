#pragma once
#include <cstdint>
#include <vector>
#include "video.hpp"

std::vector<uint8_t> serialize_video(const Video& video);
Video deserialize_video(const std::vector<uint8_t>& video_data);

void append_string(std::vector<uint8_t>& serialized_data, const std::string& s);

template <typename T>
void append_fixed(std::vector<uint8_t>& serialized_data, const T& value) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    for (size_t i = 0; i < sizeof(value); ++i) {
        serialized_data.push_back(ptr[i]);
    }
}