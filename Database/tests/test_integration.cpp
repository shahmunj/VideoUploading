#include "../include/pager.hpp"
#include "../include/btree/node.hpp"
#include "../include/btree/internal_node.hpp"
#include "../include/btree/leaf_node.hpp"
#include <iostream>
#include <cstdio>
#include <cassert>

int checksRun = 0;
int checksPassed = 0;

void check(bool condition, const std::string& label) {
    checksRun++;
    if (condition) {
        checksPassed++;
        std::cout << "[PASS] " << label << "\n";
    } else {
        std::cout << "[FAIL] " << label << "\n";
    }
}

int main() {
    const std::string filename = "integration_test.dat";
    std::remove(filename.c_str());

    uint64_t internalPageNum;
    uint64_t leafPageNum;

    // -----------------------------------------------------------------
    // PHASE 1: Build a real internal node AND a real leaf node, through
    // the Pager, all in one session -- then let the Pager go out of scope
    // (destructor flushes everything dirty to disk).
    // -----------------------------------------------------------------
    {
        Pager pager(filename);

        // --- Internal node, exactly like the earlier worked example ---
        internalPageNum = pager.allocateNewPage();
        Page* internalPage = pager.getPage(internalPageNum);

        setNodeType(internalPage, NodeType::INTERNAL);
        setNumKeys(internalPage, 2);
        writeChildPointerAt(internalPage, 0, 3);
        writeKeyAt(internalPage, 0, 30);
        writeChildPointerAt(internalPage, 1, 7);
        writeKeyAt(internalPage, 1, 60);
        writeChildPointerAt(internalPage, 2, 12);

        uint64_t routed = findChildPage(internalPage, 45);
        check(routed == 7, "internal node: findChildPage(45) routes to page 7, through a real Pager page");

        // --- Leaf node, exercising leafInsert/leafFind/readRecordAt end-to-end ---
        leafPageNum = pager.allocateNewPage();
        Page* leafPage = pager.getPage(leafPageNum);

        setNodeType(leafPage, NodeType::LEAF);
        setNumKeys(leafPage, 0);

        std::vector<uint8_t> recordA = {10, 20, 30};
        std::vector<uint8_t> recordB = {40, 50, 60, 70};
        std::vector<uint8_t> recordC = {1, 2, 3, 4, 5, 6, 7};

        bool insertedA = leafInsert(leafPage, 12, recordA);
        bool insertedB = leafInsert(leafPage, 22, recordB);
        bool insertedC = leafInsert(leafPage, 47, recordC);

        check(insertedA && insertedB && insertedC, "leaf node: three records inserted successfully through a real Pager page");

        uint64_t outOffset;
        uint32_t outLength;
        bool found = leafFind(leafPage, 22, outOffset, outLength);
        std::vector<uint8_t> extracted = readRecordAt(leafPage, outOffset, outLength);

        check(found, "leaf node: leafFind locates a record on a real Pager page");
        check(extracted == recordB, "leaf node: readRecordAt extracts correct bytes on a real Pager page");

        // Pager destructor runs here -- flushes both dirty pages to disk
    }

    // -----------------------------------------------------------------
    // PHASE 2: Fresh Pager, same file. Re-fetch BOTH pages from disk and
    // confirm everything survived the restart, byte for byte.
    // -----------------------------------------------------------------
    {
        Pager pager(filename);

        // --- Internal node survives restart ---
        Page* internalPage = pager.getPage(internalPageNum);
        check(getNodeType(internalPage) == NodeType::INTERNAL,
              "persistence: internal node's type survives restart");
        check(findChildPage(internalPage, 45) == 7,
              "persistence: findChildPage(45) still routes correctly after restart");
        check(findChildPage(internalPage, 10) == 3,
              "persistence: findChildPage(10) still routes correctly after restart");
        check(findChildPage(internalPage, 75) == 12,
              "persistence: findChildPage(75) still routes correctly after restart");

        // --- Leaf node survives restart ---
        Page* leafPage = pager.getPage(leafPageNum);
        check(getNodeType(leafPage) == NodeType::LEAF,
              "persistence: leaf node's type survives restart");
        check(getNumKeys(leafPage) == 3,
              "persistence: leaf node's num_keys survives restart");

        uint64_t outOffset;
        uint32_t outLength;

        bool foundA = leafFind(leafPage, 12, outOffset, outLength);
        std::vector<uint8_t> extractedA = readRecordAt(leafPage, outOffset, outLength);
        check(foundA && extractedA == std::vector<uint8_t>({10, 20, 30}),
              "persistence: record with key=12 survives restart, bytes correct");

        bool foundB = leafFind(leafPage, 22, outOffset, outLength);
        std::vector<uint8_t> extractedB = readRecordAt(leafPage, outOffset, outLength);
        check(foundB && extractedB == std::vector<uint8_t>({40, 50, 60, 70}),
              "persistence: record with key=22 survives restart, bytes correct");

        bool foundC = leafFind(leafPage, 47, outOffset, outLength);
        std::vector<uint8_t> extractedC = readRecordAt(leafPage, outOffset, outLength);
        check(foundC && extractedC == std::vector<uint8_t>({1, 2, 3, 4, 5, 6, 7}),
              "persistence: record with key=47 survives restart, bytes correct");

        bool missing = leafFind(leafPage, 999, outOffset, outLength);
        check(!missing, "persistence: non-existent key still correctly reports not found after restart");
    }

    std::remove(filename.c_str());

    std::cout << "\n" << checksPassed << " / " << checksRun << " integration checks passed\n";
    return (checksPassed == checksRun) ? 0 : 1;
}