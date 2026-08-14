#pragma once
#include "../pager.hpp"
#include "../video.hpp"
#include "../video_serializer.hpp"
#include "node.hpp"
#include "leaf_node.hpp"
#include "meta_page.hpp"
#include <cstdint>

class BTree {
public:
    BTree(Pager& pager);

    void insert(const Video& video);
    Video find(uint32_t id);

private:
    Pager& pager;
    uint64_t rootPage;
};