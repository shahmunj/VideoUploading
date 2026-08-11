#pragma once
#include "../pager.hpp"
#include "node.hpp"
#include <cstdint>

uint64_t getTreeRootPage(Page* metaPage);
void setTreeRootPage(Page* metaPage, uint64_t rootPage);

uint32_t getFormatVersion(Page* metaPage);
void setFormatVersion(Page* metaPage, uint32_t version);

