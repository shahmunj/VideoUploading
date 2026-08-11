#pragma once
#include "../pager.hpp"
#include <cstring>

enum class NodeType : uint8_t { META, LEAF, INTERNAL };

constexpr uint64_t HEADER_OFFSET = 0;
constexpr uint64_t HEADER_SIZE = 5;

NodeType getNodeType(Page* page);
void setNodeType(Page* page, NodeType type);

uint32_t getNumKeys(Page* page);
void setNumKeys(Page* page, uint32_t count);

template <typename T>
T readFieldAt(Page* page, uint64_t offset) {
    T value;
    memcpy(&value, page->data + offset, sizeof(T));
    return value;
}
template <typename T>
void writeFieldAt(Page* page, uint64_t offset, const T& value) {
    memcpy(page->data + offset, &value, sizeof(T));
    page->is_dirty = true;
}
