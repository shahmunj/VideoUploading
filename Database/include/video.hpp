#include <cstdint>
#include <iostream>
#include <vector>
#include <map>
#include <memory>
#include <ctime>
#include <string>

struct Media_properties {
    int duration_seconds = 0;
    int file_size_bytes = 0;
    int resolution_width = 0;
    int resolution_height = 0;
    enum class Format : uint8_t { MP4, AVI, MKV } format = Format::MP4;
    std::string file_url = "";
    std::string thumbnail_url = "";
};

struct Timestamps {
    time_t created_at = 0;
    time_t uploaded_at = 0;
    time_t updated_at = 0;
};

class Video {
    public:
        unsigned int id = 0;
        std::string title = "";
        std::string description = "";
        std::string slug = "";

        Media_properties properties {};
        Timestamps timestamps {};
        enum class Status : uint8_t { DRAFT, PUBLISHED, ARCHIVED, REMOVED, PROCESSING } status = Status::DRAFT;
};


