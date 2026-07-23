#include "video_serializer.hpp"
#include <iostream>

int main() {
    Video video {};

    auto bytes = serialize_video(video);

    std::cout << bytes.size() << '\n';

    return 0;
}