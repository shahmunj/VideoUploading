#include "../include/video_serializer.hpp"
#include <iostream>
#include <cassert>

int tests_run = 0;
int tests_passed = 0;

void check(bool condition, const std::string& testName) {
    tests_run++;
    if (condition) {
        tests_passed++;
        std::cout << "[PASS] " << testName << "\n";
    } else {
        std::cout << "[FAIL] " << testName << "\n";
    }
}

Video makeSampleVideo() {
    Video v;
    v.id = 47;
    v.title = "Cat";
    v.description = "A short video";
    v.slug = "cat-video";
    v.properties.duration_seconds = 120;
    v.properties.file_size_bytes = 5000000;
    v.properties.resolution_width = 1920;
    v.properties.resolution_height = 1080;
    v.properties.format = Media_properties::Format::MP4;
    v.properties.file_url = "/videos/47.mp4";
    v.properties.thumbnail_url = "/thumbs/47.jpg";
    v.timestamps.created_at = 1000;
    v.timestamps.uploaded_at = 2000;
    v.timestamps.updated_at = 3000;
    v.status = Video::Status::PUBLISHED;
    return v;
}

void test_basic_roundtrip() {
    Video original = makeSampleVideo();
    auto bytes = serialize_video(original);
    Video reconstructed = deserialize_video(bytes);

    check(original.id == reconstructed.id, "basic: id");
    check(original.title == reconstructed.title, "basic: title");
    check(original.description == reconstructed.description, "basic: description");
    check(original.slug == reconstructed.slug, "basic: slug");
    check(original.properties.duration_seconds == reconstructed.properties.duration_seconds, "basic: duration_seconds");
    check(original.properties.file_size_bytes == reconstructed.properties.file_size_bytes, "basic: file_size_bytes");
    check(original.properties.resolution_width == reconstructed.properties.resolution_width, "basic: resolution_width");
    check(original.properties.resolution_height == reconstructed.properties.resolution_height, "basic: resolution_height");
    check(original.properties.format == reconstructed.properties.format, "basic: format");
    check(original.properties.file_url == reconstructed.properties.file_url, "basic: file_url");
    check(original.properties.thumbnail_url == reconstructed.properties.thumbnail_url, "basic: thumbnail_url");
    check(original.timestamps.created_at == reconstructed.timestamps.created_at, "basic: created_at");
    check(original.timestamps.uploaded_at == reconstructed.timestamps.uploaded_at, "basic: uploaded_at");
    check(original.timestamps.updated_at == reconstructed.timestamps.updated_at, "basic: updated_at");
    check(original.status == reconstructed.status, "basic: status");
}

void test_empty_strings() {
    Video v;
    v.id = 1;
    v.title = "";
    v.description = "";
    v.slug = "";

    auto bytes = serialize_video(v);
    Video r = deserialize_video(bytes);

    check(r.title == "", "empty_strings: title");
    check(r.description == "", "empty_strings: description");
    check(r.slug == "", "empty_strings: slug");
}

void test_long_string() {
    Video v;
    v.id = 2;
    v.title = std::string(10000, 'a');

    auto bytes = serialize_video(v);
    Video r = deserialize_video(bytes);

    check(r.title == v.title, "long_string: title matches");
    check(r.title.size() == 10000, "long_string: title length correct");
}

void test_unicode() {
    Video v;
    v.id = 3;
    v.title = "Cat Video";  // ASCII-only, safe baseline check first
    v.description = "Test";

    auto bytes = serialize_video(v);
    Video r = deserialize_video(bytes);

    check(r.title == v.title, "unicode: ascii title baseline");
    // Add real emoji/unicode content here once basic ASCII round-trip is confirmed
}

void test_zero_id() {
    Video v;
    v.id = 0;

    auto bytes = serialize_video(v);
    Video r = deserialize_video(bytes);

    check(r.id == 0, "zero_id: id is zero");
}

void test_max_id() {
    Video v;
    v.id = UINT_MAX;

    auto bytes = serialize_video(v);
    Video r = deserialize_video(bytes);

    check(r.id == UINT_MAX, "max_id: id at max value");
}

void test_byte_count_matches_expected() {
    Video v;
    v.id = 1;
    v.title = "Hello";       // 5 chars
    v.description = "World"; // 5 chars
    v.slug = "hello-world";  // 11 chars
    // properties/timestamps/status left default

    auto bytes = serialize_video(v);

    // 4 (id) + [4+5] (title) + [4+5] (description) + [4+11] (slug)
    // + 4*4 (duration/filesize/width/height) + 1 (format)
    // + [4+0] (file_url, empty) + [4+0] (thumbnail_url, empty)
    // + 8*3 (timestamps, assuming 8-byte time_t) + 1 (status)
    size_t expected = 4 + (4+5) + (4+5) + (4+11) + (4*4) + 1 + (4+0) + (4+0) + (8*3) + 1;

    check(bytes.size() == expected, "byte_count: matches hand-calculated size");
}

int main() {
    test_basic_roundtrip();
    test_empty_strings();
    test_long_string();
    test_unicode();
    test_zero_id();
    test_max_id();
    test_byte_count_matches_expected();

    std::cout << "\n" << tests_passed << " / " << tests_run << " tests passed\n";

    return (tests_passed == tests_run) ? 0 : 1;
}