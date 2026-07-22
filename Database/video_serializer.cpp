#include "video_serializer.hpp"

std::vector<uint8_t> serialize_video(const Video& video) {
    std::vector<uint8_t> serialized_data;
    
    append_fixed(serialized_data, video.id);

    append_string(serialized_data, video.title);
    append_string(serialized_data, video.description);
    append_string(serialized_data, video.slug);

    return serialized_data;
}

void append_string(std::vector<uint8_t>& serialized_data, const std::string& s) {
    uint32_t length = static_cast<uint32_t>(s.size());
    append_fixed(serialized_data, length);
    for(size_t i = 0; i < s.size(); i++){ 
        serialized_data.push_back(s[i]);
    }
}
