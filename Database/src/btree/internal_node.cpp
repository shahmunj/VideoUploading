#include "../../include/btree/internal_node.hpp"
#include <cstring>

constexpr size_t ENTRY_SIZE = 12; 
constexpr size_t POINTER_SIZE = 8;
constexpr size_t POINTER_OFFSET = 0;

size_t pointerOffset(uint32_t index){
    return HEADER_SIZE + (index * ENTRY_SIZE);
}

size_t keyOffset(uint32_t index){
    return pointerOffset(index) + POINTER_SIZE;
}

unsigned int readKeyAt(Page *page, uint32_t index){
    unsigned int key;
    memcpy(&key, page->data + keyOffset(index), sizeof(key));
    return key;
}

void writeKeyAt(Page* page, uint32_t index, unsigned int key){
    memcpy(page->data + keyOffset(index), &key, sizeof(key));
    page->is_dirty = true;
}

size_t readChildPointerAt(Page* page, uint32_t index){
    size_t pointer;
    memcpy(&pointer, page->data + pointerOffset(index), sizeof(pointer));
    return pointer;
}

void writeChildPointerAt(Page* page, uint32_t index, size_t childPageNumber){
    memcpy(page->data + pointerOffset(index), &childPageNumber, sizeof(childPageNumber));
    page->is_dirty = true;
}

size_t findChildPage(Page* page, unsigned int targetKey){
    uint32_t numKeys = getNumKeys(page);

    int32_t min = 0;
    int32_t max = static_cast<int32_t>(numKeys) - 1;
    int32_t result = static_cast<int32_t>(numKeys);

    while (min <= max) {
        int32_t middle = min + (max - min) / 2;
        unsigned int middle_key = readKeyAt(page, middle);

        if (targetKey < middle_key) {
            result = middle;
            max = middle - 1;
        } else {
            min = middle + 1;
        }
    }

    return readChildPointerAt(page, result);
}