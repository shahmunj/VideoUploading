#pragma once
#include "../pager.hpp"

enum class NodeType : uint8_t { LEAF, INTERNAL };

constexpr size_t HEADER_SIZE = 5;

NodeType getNodeType(Page* page);
void setNodeType(Page* page, NodeType type);

uint32_t getNumKeys(Page* page);
void setNumKeys(Page* page, uint32_t count);