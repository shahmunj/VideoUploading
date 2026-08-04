#pragma once
#include "../pager.hpp"
#include "node.hpp"
#include <cstdint>
#include <cstddef>

size_t keyOffset(uint32_t index);
size_t pointerOffset(uint32_t index);

unsigned int readKeyAt(Page* page, uint32_t index);
void writeKeyAt(Page* page, uint32_t index, unsigned int key);

size_t readChildPointerAt(Page* page, uint32_t index);
void writeChildPointerAt(Page* page, uint32_t index, size_t childPageNumber);

size_t findChildPage(Page* page, unsigned int targetKey);

