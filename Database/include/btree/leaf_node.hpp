#pragma once
#include "../pager.hpp"
#include "node.hpp"
#include <cstdint>
#include <cstddef>
#include <vector>

unsigned int readLeafKeyAt(Page* page, uint32_t index);
void writeLeafKeyAt(Page* page, uint32_t index, uint32_t key);

uint64_t readLeafOffsetAt(Page* page, uint32_t index);
void writeLeafOffsetAt(Page* page, uint32_t index, uint64_t offset);

uint32_t readLeafLengthAt(Page* page, uint32_t index);
void writeLeafLengthAt(Page* page, uint32_t index, uint32_t length);

bool leafInsert(Page* page, uint32_t key, const std::vector<uint8_t>& record);
bool leafFind(Page* page, uint32_t key, uint64_t& outOffset, uint32_t& outLength);

std::vector<uint8_t> readRecordAt(Page* page, uint64_t offset, uint32_t length);