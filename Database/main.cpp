#include "video_serializer.hpp"
#include <iostream>

int main() {
    Video video;

    video.id = 1;
    video.title = "Hello";
    video.description = "World";
    video.slug = "hello-world";

    auto bytes = serialize_video(video);

    std::cout << bytes.size() << '\n';

    return 0;
}