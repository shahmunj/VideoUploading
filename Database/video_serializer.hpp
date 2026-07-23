#pragma once
#include <cstdint>
#include <cstring> 
#include <vector>
#include <string>    
#include "video.hpp"

std::vector<uint8_t> serialize_video(const Video& video);
Video deserialize_video(const std::vector<uint8_t>& video_data);


//deserializer helper functions
std::string read_string(const std::vector<uint8_t>& video_data, size_t& offset);

template <typename T>
void read_fixed(const std::vector<uint8_t>& video_data, size_t& offset, T& value) {
    memcpy(&value, video_data.data() + offset, sizeof(T));
    offset += sizeof(T);
}

//serializer helper functions
void append(std::vector<uint8_t>& serialized_data, const std::string& s);

template <typename T>
void append(std::vector<uint8_t>& serialized_data, const T& value) {
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(&value);
    for (size_t i = 0; i < sizeof(value); ++i) {
        serialized_data.push_back(ptr[i]);
    }
}