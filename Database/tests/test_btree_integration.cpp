#include "../include/pager.hpp"
#include "../include/video.hpp"
#include "../include/btree/btree.hpp"
#include <iostream>
#include <cstdio>

int checksRun = 0;
int checksPassed = 0;

void check(bool condition, const std::string& label) {
    checksRun++;
    if (condition) {
        checksPassed++;
        std::cout << "[PASS] " << label << "\n";
    } else {
        std::cout << "[FAIL] " << label << "\n";
    }
}

Video makeTestVideo(uint32_t id, const std::string& title, const std::string& description) {
    Video v;
    v.id = id;
    v.title = title;
    v.description = description;
    v.slug = title + "-slug";
    v.properties.duration_seconds = 120;
    v.properties.file_size_bytes = 1024 * 1024;
    v.properties.resolution_width = 1920;
    v.properties.resolution_height = 1080;
    v.properties.format = Media_properties::Format::MP4;
    v.properties.file_url = "https://example.com/" + title + ".mp4";
    v.properties.thumbnail_url = "https://example.com/" + title + "_thumb.jpg";
    v.timestamps.created_at = 1000;
    v.timestamps.uploaded_at = 1001;
    v.timestamps.updated_at = 1002;
    v.status = Video::Status::PUBLISHED;
    return v;
}

bool videosMatch(const Video& a, const Video& b) {
    return a.id == b.id
        && a.title == b.title
        && a.description == b.description
        && a.slug == b.slug
        && a.properties.duration_seconds == b.properties.duration_seconds
        && a.properties.file_size_bytes == b.properties.file_size_bytes
        && a.properties.resolution_width == b.properties.resolution_width
        && a.properties.resolution_height == b.properties.resolution_height
        && a.properties.format == b.properties.format
        && a.properties.file_url == b.properties.file_url
        && a.properties.thumbnail_url == b.properties.thumbnail_url
        && a.timestamps.created_at == b.timestamps.created_at
        && a.timestamps.uploaded_at == b.timestamps.uploaded_at
        && a.timestamps.updated_at == b.timestamps.updated_at
        && a.status == b.status;
}

int main() {
    const std::string filename = "btree_integration_test.dat";
    std::remove(filename.c_str());

    // -----------------------------------------------------------------
    // PHASE 1: Brand new database. Confirm the BTree constructor
    // correctly sets up a fresh meta page + root leaf, then insert
    // and find several real Video objects.
    // -----------------------------------------------------------------
    {
        Pager pager(filename);
        BTree tree(pager);

        Video v1 = makeTestVideo(1, "First Video", "The very first video ever inserted.");
        Video v2 = makeTestVideo(2, "Second Video", "A different description entirely.");
        Video v3 = makeTestVideo(3, "Third Video", "Short one.");

        tree.insert(v1);
        tree.insert(v2);
        tree.insert(v3);

        Video found1 = tree.find(1);
        Video found2 = tree.find(2);
        Video found3 = tree.find(3);

        check(videosMatch(found1, v1), "BTree: video 1 round-trips correctly (all fields match)");
        check(videosMatch(found2, v2), "BTree: video 2 round-trips correctly (all fields match)");
        check(videosMatch(found3, v3), "BTree: video 3 round-trips correctly (all fields match)");

        // Confirm finding a non-existent id throws, rather than returning garbage
        bool threw = false;
        try {
            tree.find(999);
        } catch (const std::runtime_error&) {
            threw = true;
        }
        check(threw, "BTree: find() throws for a non-existent id");

        // Pager destructor runs here -- flushes everything to disk
    }

    // -----------------------------------------------------------------
    // PHASE 2: Fresh Pager + BTree, same file. Confirm the constructor
    // correctly detects an EXISTING database (reads root from meta page,
    // does NOT recreate it), and all three videos survived the restart.
    // -----------------------------------------------------------------
    {
        Pager pager(filename);
        BTree tree(pager);   // should read existing meta page, NOT create a new one

        Video found1 = tree.find(1);
        Video found2 = tree.find(2);
        Video found3 = tree.find(3);

        check(found1.title == "First Video", "persistence: video 1 survives restart");
        check(found2.title == "Second Video", "persistence: video 2 survives restart");
        check(found3.title == "Third Video", "persistence: video 3 survives restart");
        check(found1.description == "The very first video ever inserted.",
              "persistence: video 1's description survives restart, byte-for-byte");

        // Insert a 4th video on this SECOND session, to confirm the tree
        // is genuinely still writable after reconnecting to an existing file
        Video v4 = makeTestVideo(4, "Fourth Video", "Inserted after a restart.");
        tree.insert(v4);
        Video found4 = tree.find(4);
        check(videosMatch(found4, v4), "BTree: can still insert new videos after reconnecting to an existing database");
    }

    // -----------------------------------------------------------------
    // PHASE 3: One more restart, confirming ALL FOUR videos (including
    // the one inserted during Phase 2) persisted correctly.
    // -----------------------------------------------------------------
    {
        Pager pager(filename);
        BTree tree(pager);

        Video found1 = tree.find(1);
        Video found2 = tree.find(2);
        Video found3 = tree.find(3);
        Video found4 = tree.find(4);

        check(found1.id == 1 && found2.id == 2 && found3.id == 3 && found4.id == 4,
              "persistence: all four videos (across two insert sessions) survive a second restart");
    }

    std::remove(filename.c_str());

    std::cout << "\n" << checksPassed << " / " << checksRun << " integration checks passed\n";
    return (checksPassed == checksRun) ? 0 : 1;
}