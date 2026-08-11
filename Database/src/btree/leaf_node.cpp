#include "../../include/btree/leaf_node.hpp"
#include <cstring>

constexpr uint64_t KEY_OFFSET = 0;
constexpr uint64_t KEY_SIZE = 4;
constexpr uint64_t LOCATION_OFFSET = KEY_OFFSET + KEY_SIZE;
constexpr uint64_t LOCATION_SIZE = 8;
constexpr uint64_t LENGTH_OFFSET = LOCATION_OFFSET + LOCATION_SIZE;
constexpr uint64_t LENGTH_SIZE = 4;
constexpr uint64_t ENTRY_SIZE = KEY_SIZE + LOCATION_SIZE + LENGTH_SIZE;

size_t leafFieldOffset(uint32_t index, size_t fieldOffset) {
    return HEADER_SIZE + (index * ENTRY_SIZE) + fieldOffset;
}

uint32_t readLeafKeyAt(Page* page, uint32_t index) {
    return readFieldAt<uint32_t>(page, leafFieldOffset(index, KEY_OFFSET));
}
void writeLeafKeyAt(Page* page, uint32_t index, uint32_t key) {
    writeFieldAt(page, leafFieldOffset(index, KEY_OFFSET), key);
}

uint64_t readLeafOffsetAt(Page* page, uint32_t index) {
    return readFieldAt<uint64_t>(page, leafFieldOffset(index, LOCATION_OFFSET));
}
void writeLeafOffsetAt(Page* page, uint32_t index, uint64_t offset) {
    writeFieldAt(page, leafFieldOffset(index, LOCATION_OFFSET), offset);
}

uint32_t readLeafLengthAt(Page* page, uint32_t index) {
    return readFieldAt<uint32_t>(page, leafFieldOffset(index, LENGTH_OFFSET));
}
void writeLeafLengthAt(Page* page, uint32_t index, uint32_t length) {
    writeFieldAt(page, leafFieldOffset(index, LENGTH_OFFSET), length);
}

bool leafFind(Page* page, uint32_t key, uint64_t& outOffset, uint32_t& outLength){
    uint32_t numKeys = getNumKeys(page);

    int32_t min = 0;
    int32_t max = static_cast<int32_t>(numKeys) - 1; 

    while(min <= max){
        int32_t mid = min + (max - min) / 2; 
        uint32_t middle_key = readLeafKeyAt(page, mid);

        if (key == middle_key){
            outOffset = readLeafOffsetAt(page, mid);
            outLength = readLeafLengthAt(page, mid);
            return true;
        } else if (key < middle_key){
            max = mid - 1;
        } else {
            min = mid + 1;
        }
    }
    return false;
}

uint64_t currentRecordsStart(Page* page) {
    uint32_t numKeys = getNumKeys(page);
    if (numKeys == 0) {
        return PAGE_SIZE;
    }
    return readLeafOffsetAt(page, numKeys - 1);
}

bool leafInsert(Page* page, uint32_t key, const std::vector<uint8_t>& record){
    uint64_t space_needed = ENTRY_SIZE + record.size();
    uint32_t numKeys = getNumKeys(page);
    uint64_t space_avail =  currentRecordsStart(page) - (HEADER_SIZE + (numKeys * ENTRY_SIZE));

    if(space_avail < space_needed){
        return false;
    }
    //calculate the new record's key, offset, and length
    uint32_t new_record_key = key;
    uint32_t new_record_length = static_cast<uint32_t>(record.size());
    uint64_t new_record_offset = currentRecordsStart(page) - new_record_length;
    
    //use the helper functions to write the record
    writeLeafKeyAt(page, numKeys, new_record_key);
    writeLeafLengthAt(page, numKeys, new_record_length);
    writeLeafOffsetAt(page, numKeys, new_record_offset);
    
    memcpy(page->data + new_record_offset, record.data(), new_record_length);

    numKeys++;
    setNumKeys(page, numKeys);

    page->is_dirty = true;
    return true;
}

std::vector<uint8_t> readRecordAt(Page* page, uint64_t offset, uint32_t length){
    std::vector<uint8_t> record(length);
    memcpy(record.data(), page->data + offset, length);
    return record;
}