#pragma once
#include "../pager.hpp"
#include "node.hpp"
#include <cstdint>
#include <cstddef>

uint64_t keyOffset(uint32_t index); 
uint64_t pointerOffset(uint32_t index);    

uint32_t readKeyAt(Page* page, uint32_t index);
void writeKeyAt(Page* page, uint32_t index, uint32_t key);

uint64_t readChildPointerAt(Page* page, uint32_t index);   
void writeChildPointerAt(Page* page, uint32_t index, uint64_t childPageNumber); 

uint64_t findChildPage(Page* page, uint32_t targetKey); 