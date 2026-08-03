#pragma once
#include "../pager.hpp"

enum class NodeType : uint8_t { LEAF, INTERNAL };

NodeType getNodeType(Page* page);
void setNodeType(Page* page, NodeType type);

uint32_t getNumKeys(Page* page);
void setNumKeys(Page* page, uint32_t count);