#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include <string>

struct Media_properties {
    int duration_seconds;
    int file_size_bytes;
    int resolution_width;
    int resolution_height;
    enum class Format : uint8_t { MP4, AVI, MKV } format;
    std::string file_url;
    std::string thumbnail_url;
};

struct Timestamps {
    time_t created_at;
    time_t uploaded_at;
    time_t updated_at;
};

class Video {
    public:
        unsigned int id;
        std::string title;
        std::string description;
        std::string slug;

        Media_properties properties;
        Timestamps timestamps;
        enum class Status : uint8_t { DRAFT, PUBLISHED, ARCHIVED, REMOVED, PROCESSING } status;

};


