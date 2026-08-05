#include "../../include/btree/node.hpp"
#include <cstring>

constexpr size_t NODE_TYPE_OFFSET = 0;
constexpr size_t NODE_TYPE_SIZE = 1;
constexpr size_t NUM_KEYS_OFFSET = NODE_TYPE_OFFSET + NODE_TYPE_SIZE;
constexpr size_t NUM_KEYS_SIZE = 4;

NodeType getNodeType(Page* page){
    return static_cast<NodeType>(page->data[NODE_TYPE_OFFSET]);
}
void setNodeType(Page* page, NodeType type){
    page->data[NODE_TYPE_OFFSET] = static_cast<uint8_t>(type);
    page->is_dirty = true;
}

uint32_t getNumKeys(Page* page){
    uint32_t numKeys;
    memcpy(&numKeys, page->data + NUM_KEYS_OFFSET, sizeof(numKeys));
    return numKeys;
}
void setNumKeys(Page* page, uint32_t count){
    memcpy(page->data + NUM_KEYS_OFFSET, &count, sizeof(count));
    page->is_dirty = true;
}