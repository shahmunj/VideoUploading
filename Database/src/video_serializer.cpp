#include "../include/video_serializer.hpp"

std::vector<uint8_t> serialize_video(const Video& video) {
    std::vector<uint8_t> serialized_data;
    
    append(serialized_data, video.id);
    append(serialized_data, video.title);
    append(serialized_data, video.description);
    append(serialized_data, video.slug);

    // Media_properties (nested struct) — expand field by field, in declared order
    append(serialized_data, video.properties.duration_seconds);
    append(serialized_data, video.properties.file_size_bytes);
    append(serialized_data, video.properties.resolution_width);
    append(serialized_data, video.properties.resolution_height);
    append(serialized_data, video.properties.format);          // uint8_t-backed enum — template handles it
    append(serialized_data, video.properties.file_url);        // string — uses the string overload
    append(serialized_data, video.properties.thumbnail_url);   // string

    // Timestamps (nested struct)
    append(serialized_data, video.timestamps.created_at);
    append(serialized_data, video.timestamps.uploaded_at);
    append(serialized_data, video.timestamps.updated_at);

    // Video's status
    append(serialized_data, video.status);

    return serialized_data;
}

Video deserialize_video(const std::vector<uint8_t>& video_data) {
    Video video;
    size_t offset = 0;
    read_fixed(video_data, offset, video.id);
    video.title = read_string(video_data, offset);
    video.description = read_string(video_data, offset);
    video.slug = read_string(video_data, offset);

    // Media_properties (nested struct)
    read_fixed(video_data, offset, video.properties.duration_seconds);
    read_fixed(video_data, offset, video.properties.file_size_bytes);
    read_fixed(video_data, offset, video.properties.resolution_width);
    read_fixed(video_data, offset, video.properties.resolution_height);
    read_fixed(video_data, offset, video.properties.format);
    video.properties.file_url = read_string(video_data, offset);
    video.properties.thumbnail_url = read_string(video_data, offset);

    // Timestamps (nested struct)
    read_fixed(video_data, offset, video.timestamps.created_at);
    read_fixed(video_data, offset, video.timestamps.uploaded_at);
    read_fixed(video_data, offset, video.timestamps.updated_at);

    // Video's status
    read_fixed(video_data, offset, video.status);

    return video;
}

std::string read_string(const std::vector<uint8_t>& video_data, size_t& offset) {
    uint32_t length;
    read_fixed(video_data, offset, length);
    std::string result(video_data.begin() + offset, video_data.begin() + offset + length);
    offset += length;

    return result;
}

void append(std::vector<uint8_t>& serialized_data, const std::string& s) {
    uint32_t length = static_cast<uint32_t>(s.size());
    append(serialized_data, length);
    for(size_t i = 0; i < s.size(); i++){ 
        serialized_data.push_back(s[i]);
    }
}
