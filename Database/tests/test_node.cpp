#include "../include/btree/node.hpp"
#include "../include/btree/internal_node.hpp"
#include "../include/btree/leaf_node.hpp"
#include "../include/btree/meta_page.hpp"
#include <iostream>
#include <cstring>

int tests_run = 0;
int tests_passed = 0;

void check(bool condition, const std::string& testName) {
    tests_run++;
    if (condition) {
        tests_passed++;
        std::cout << "[PASS] " << testName << "\n";
    } else {
        std::cout << "[FAIL] " << testName << "\n";
    }
}

Page makeCleanPage() {
    Page page{};
    memset(page.data, 0, PAGE_SIZE);
    page.is_dirty = false;
    return page;
}

// ---------------------------------------------------------------------
// node.cpp tests -- shared header (node_type, num_keys)
// ---------------------------------------------------------------------

void test_setNodeType_then_getNodeType_leaf() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    check(getNodeType(&page) == NodeType::LEAF, "node: set/get NodeType::LEAF round-trips");
    check(page.is_dirty == true, "node: setNodeType marks page dirty");
}

void test_setNodeType_then_getNodeType_internal() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::INTERNAL);
    check(getNodeType(&page) == NodeType::INTERNAL, "node: set/get NodeType::INTERNAL round-trips");
}

void test_setNumKeys_then_getNumKeys() {
    Page page = makeCleanPage();
    setNumKeys(&page, 0);
    check(getNumKeys(&page) == 0, "node: num_keys = 0 round-trips");

    setNumKeys(&page, 42);
    check(getNumKeys(&page) == 42, "node: num_keys = 42 round-trips");

    setNumKeys(&page, 340);
    check(getNumKeys(&page) == 340, "node: num_keys = 340 (max realistic value) round-trips");
}

void test_nodeType_and_numKeys_dont_clobber_each_other() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::INTERNAL);
    setNumKeys(&page, 7);

    check(getNodeType(&page) == NodeType::INTERNAL, "node: node_type unaffected after setting num_keys");
    check(getNumKeys(&page) == 7, "node: num_keys unaffected after setting node_type");
}

// ---------------------------------------------------------------------
// internal_node.cpp tests -- readKeyAt / writeKeyAt
// ---------------------------------------------------------------------

void test_writeKeyAt_then_readKeyAt_single() {
    Page page = makeCleanPage();
    writeKeyAt(&page, 0, 30);
    check(readKeyAt(&page, 0) == 30, "internal_node: write/read key at index 0 round-trips");
    check(page.is_dirty == true, "internal_node: writeKeyAt marks page dirty");
}

void test_writeKeyAt_then_readKeyAt_multiple() {
    Page page = makeCleanPage();
    writeKeyAt(&page, 0, 30);
    writeKeyAt(&page, 1, 60);
    writeKeyAt(&page, 2, 95);

    check(readKeyAt(&page, 0) == 30, "internal_node: key at index 0 correct after multiple writes");
    check(readKeyAt(&page, 1) == 60, "internal_node: key at index 1 correct after multiple writes");
    check(readKeyAt(&page, 2) == 95, "internal_node: key at index 2 correct after multiple writes");
}

void test_writeKeyAt_does_not_clobber_adjacent_entries() {
    Page page = makeCleanPage();
    writeKeyAt(&page, 0, 111);
    writeKeyAt(&page, 1, 222);
    writeKeyAt(&page, 0, 999);   // overwrite index 0 only

    check(readKeyAt(&page, 0) == 999, "internal_node: overwriting key 0 updates only index 0");
    check(readKeyAt(&page, 1) == 222, "internal_node: overwriting key 0 leaves index 1 untouched");
}

// ---------------------------------------------------------------------
// internal_node.cpp tests -- readChildPointerAt / writeChildPointerAt
// ---------------------------------------------------------------------

void test_writeChildPointerAt_then_readChildPointerAt_single() {
    Page page = makeCleanPage();
    writeChildPointerAt(&page, 0, 3);
    check(readChildPointerAt(&page, 0) == 3, "internal_node: write/read child pointer at index 0 round-trips");
}

void test_writeChildPointerAt_then_readChildPointerAt_multiple() {
    Page page = makeCleanPage();
    writeChildPointerAt(&page, 0, 3);
    writeChildPointerAt(&page, 1, 7);
    writeChildPointerAt(&page, 2, 12);

    check(readChildPointerAt(&page, 0) == 3, "internal_node: child pointer 0 correct after multiple writes");
    check(readChildPointerAt(&page, 1) == 7, "internal_node: child pointer 1 correct after multiple writes");
    check(readChildPointerAt(&page, 2) == 12, "internal_node: child pointer 2 correct after multiple writes");
}

void test_key_and_pointer_dont_overlap() {
    Page page = makeCleanPage();
    writeChildPointerAt(&page, 0, 999999);
    writeKeyAt(&page, 0, 30);

    check(readChildPointerAt(&page, 0) == 999999, "internal_node: writing key does not corrupt same-entry child pointer");
    check(readKeyAt(&page, 0) == 30, "internal_node: writing child pointer does not corrupt same-entry key");
}

// ---------------------------------------------------------------------
// internal_node.cpp tests -- findChildPage (binary search)
// ---------------------------------------------------------------------

Page makeSampleInternalNode() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::INTERNAL);
    setNumKeys(&page, 2);

    writeChildPointerAt(&page, 0, 3);
    writeKeyAt(&page, 0, 30);
    writeChildPointerAt(&page, 1, 7);
    writeKeyAt(&page, 1, 60);
    writeChildPointerAt(&page, 2, 12);

    return page;
}

void test_findChildPage_targetLessThanAllKeys() {
    Page page = makeSampleInternalNode();
    check(findChildPage(&page, 10) == 3, "findChildPage: target < all keys -> first child (page 3)");
}

void test_findChildPage_targetGreaterThanAllKeys() {
    Page page = makeSampleInternalNode();
    check(findChildPage(&page, 75) == 12, "findChildPage: target > all keys -> last child (page 12)");
}

void test_findChildPage_targetBetweenKeys() {
    Page page = makeSampleInternalNode();
    check(findChildPage(&page, 45) == 7, "findChildPage: target between key[0] and key[1] -> middle child (page 7)");
}

void test_findChildPage_targetExactlyEqualsFirstKey() {
    Page page = makeSampleInternalNode();
    check(findChildPage(&page, 30) == 7, "findChildPage: target == key[0] -> child AFTER key[0] (page 7)");
}

void test_findChildPage_targetExactlySecondKey() {
    Page page = makeSampleInternalNode();
    check(findChildPage(&page, 60) == 12, "findChildPage: target == key[1] -> child AFTER key[1] (page 12)");
}

void test_findChildPage_singleKeyNode() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::INTERNAL);
    setNumKeys(&page, 1);
    writeChildPointerAt(&page, 0, 100);
    writeKeyAt(&page, 0, 50);
    writeChildPointerAt(&page, 1, 200);

    check(findChildPage(&page, 10) == 100, "findChildPage: single-key node, target < key -> first child");
    check(findChildPage(&page, 90) == 200, "findChildPage: single-key node, target > key -> second child");
}

void test_findChildPage_manyKeys_noInfiniteLoop() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::INTERNAL);

    const uint32_t N = 20;
    setNumKeys(&page, N);
    for (uint32_t i = 0; i < N; i++) {
        writeChildPointerAt(&page, i, i * 10);
        writeKeyAt(&page, i, (i + 1) * 100);
    }
    writeChildPointerAt(&page, N, N * 10);

    check(findChildPage(&page, 50) == 0, "findChildPage: many keys, target before first key");
    check(findChildPage(&page, 5000) == 200, "findChildPage: many keys, target after last key");
    check(findChildPage(&page, 250) == 20, "findChildPage: many keys, target between key[1]=200 and key[2]=300");
}

// ---------------------------------------------------------------------
// leaf_node.cpp tests -- readLeafKeyAt / writeLeafKeyAt
// ---------------------------------------------------------------------

void test_writeLeafKeyAt_then_readLeafKeyAt_single() {
    Page page = makeCleanPage();
    writeLeafKeyAt(&page, 0, 47);
    check(readLeafKeyAt(&page, 0) == 47, "leaf_node: write/read key at index 0 round-trips");
    check(page.is_dirty == true, "leaf_node: writeLeafKeyAt marks page dirty");
}

void test_writeLeafKeyAt_then_readLeafKeyAt_multiple() {
    Page page = makeCleanPage();
    writeLeafKeyAt(&page, 0, 12);
    writeLeafKeyAt(&page, 1, 22);
    writeLeafKeyAt(&page, 2, 47);

    check(readLeafKeyAt(&page, 0) == 12, "leaf_node: key at index 0 correct after multiple writes");
    check(readLeafKeyAt(&page, 1) == 22, "leaf_node: key at index 1 correct after multiple writes");
    check(readLeafKeyAt(&page, 2) == 47, "leaf_node: key at index 2 correct after multiple writes");
}

// ---------------------------------------------------------------------
// leaf_node.cpp tests -- readLeafOffsetAt / writeLeafOffsetAt
// ---------------------------------------------------------------------

void test_writeLeafOffsetAt_then_readLeafOffsetAt_single() {
    Page page = makeCleanPage();
    writeLeafOffsetAt(&page, 0, 3800);
    check(readLeafOffsetAt(&page, 0) == 3800, "leaf_node: write/read offset at index 0 round-trips");
}

void test_writeLeafOffsetAt_then_readLeafOffsetAt_multiple() {
    Page page = makeCleanPage();
    writeLeafOffsetAt(&page, 0, 3800);
    writeLeafOffsetAt(&page, 1, 3200);
    writeLeafOffsetAt(&page, 2, 3000);

    check(readLeafOffsetAt(&page, 0) == 3800, "leaf_node: offset 0 correct after multiple writes");
    check(readLeafOffsetAt(&page, 1) == 3200, "leaf_node: offset 1 correct after multiple writes");
    check(readLeafOffsetAt(&page, 2) == 3000, "leaf_node: offset 2 correct after multiple writes");
}

// ---------------------------------------------------------------------
// leaf_node.cpp tests -- readLeafLengthAt / writeLeafLengthAt
// ---------------------------------------------------------------------

void test_writeLeafLengthAt_then_readLeafLengthAt_single() {
    Page page = makeCleanPage();
    writeLeafLengthAt(&page, 0, 187);
    check(readLeafLengthAt(&page, 0) == 187, "leaf_node: write/read length at index 0 round-trips");
}

void test_leaf_key_offset_length_dont_overlap() {
    Page page = makeCleanPage();
    writeLeafKeyAt(&page, 0, 47);
    writeLeafOffsetAt(&page, 0, 3800);
    writeLeafLengthAt(&page, 0, 187);

    check(readLeafKeyAt(&page, 0) == 47, "leaf_node: key unaffected by offset/length writes in same entry");
    check(readLeafOffsetAt(&page, 0) == 3800, "leaf_node: offset unaffected by key/length writes in same entry");
    check(readLeafLengthAt(&page, 0) == 187, "leaf_node: length unaffected by key/offset writes in same entry");
}

// ---------------------------------------------------------------------
// leaf_node.cpp tests -- leafFind (binary search, exact match)
// ---------------------------------------------------------------------

Page makeSampleLeafNode() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 3);

    writeLeafKeyAt(&page, 0, 12);
    writeLeafOffsetAt(&page, 0, 3800);
    writeLeafLengthAt(&page, 0, 100);

    writeLeafKeyAt(&page, 1, 22);
    writeLeafOffsetAt(&page, 1, 3600);
    writeLeafLengthAt(&page, 1, 150);

    writeLeafKeyAt(&page, 2, 47);
    writeLeafOffsetAt(&page, 2, 3300);
    writeLeafLengthAt(&page, 2, 200);

    return page;
}

void test_leafFind_existingKey() {
    Page page = makeSampleLeafNode();
    uint64_t outOffset;
    uint32_t outLength;

    bool found = leafFind(&page, 22, outOffset, outLength);

    check(found == true, "leafFind: existing key is found");
    check(outOffset == 3600, "leafFind: correct offset returned for found key");
    check(outLength == 150, "leafFind: correct length returned for found key");
}

void test_leafFind_firstKey() {
    Page page = makeSampleLeafNode();
    uint64_t outOffset;
    uint32_t outLength;

    check(leafFind(&page, 12, outOffset, outLength) == true, "leafFind: first key is found");
    check(outOffset == 3800, "leafFind: first key's offset correct");
}

void test_leafFind_lastKey() {
    Page page = makeSampleLeafNode();
    uint64_t outOffset;
    uint32_t outLength;

    check(leafFind(&page, 47, outOffset, outLength) == true, "leafFind: last key is found");
    check(outOffset == 3300, "leafFind: last key's offset correct");
}

void test_leafFind_missingKey() {
    Page page = makeSampleLeafNode();
    uint64_t outOffset;
    uint32_t outLength;

    check(leafFind(&page, 999, outOffset, outLength) == false, "leafFind: non-existent key returns false");
}

void test_leafFind_emptyPage() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    uint64_t outOffset;
    uint32_t outLength;
    check(leafFind(&page, 1, outOffset, outLength) == false, "leafFind: empty page always returns false");
}

// ---------------------------------------------------------------------
// leaf_node.cpp tests -- leafInsert + readRecordAt (full round-trip)
// ---------------------------------------------------------------------

void test_leafInsert_then_leafFind_singleRecord() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    std::vector<uint8_t> record = {1, 2, 3, 4, 5};
    bool inserted = leafInsert(&page, 47, record);

    check(inserted == true, "leafInsert: single insert into empty page succeeds");
    check(getNumKeys(&page) == 1, "leafInsert: num_keys incremented after insert");

    uint64_t outOffset;
    uint32_t outLength;
    bool found = leafFind(&page, 47, outOffset, outLength);

    check(found == true, "leafInsert+leafFind: inserted key is findable");
    check(outLength == 5, "leafInsert+leafFind: inserted record's length is correct");
}

void test_leafInsert_then_readRecordAt_roundtrip() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    std::vector<uint8_t> original = {10, 20, 30, 40, 50, 60};
    leafInsert(&page, 100, original);

    uint64_t outOffset;
    uint32_t outLength;
    leafFind(&page, 100, outOffset, outLength);

    std::vector<uint8_t> extracted = readRecordAt(&page, outOffset, outLength);

    check(extracted == original, "leafInsert+readRecordAt: extracted bytes match originally inserted bytes");
}

void test_leafInsert_multipleRecords_allFindable() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    std::vector<uint8_t> recordA = {1, 1, 1};
    std::vector<uint8_t> recordB = {2, 2, 2, 2};
    std::vector<uint8_t> recordC = {3, 3, 3, 3, 3};

    check(leafInsert(&page, 10, recordA) == true, "leafInsert: record A inserted successfully");
    check(leafInsert(&page, 20, recordB) == true, "leafInsert: record B inserted successfully");
    check(leafInsert(&page, 30, recordC) == true, "leafInsert: record C inserted successfully");

    check(getNumKeys(&page) == 3, "leafInsert: num_keys correctly reflects 3 inserts");

    uint64_t offset;
    uint32_t length;

    leafFind(&page, 10, offset, length);
    check(readRecordAt(&page, offset, length) == recordA, "leafInsert: record A retrievable and correct after multiple inserts");

    leafFind(&page, 20, offset, length);
    check(readRecordAt(&page, offset, length) == recordB, "leafInsert: record B retrievable and correct after multiple inserts");

    leafFind(&page, 30, offset, length);
    check(readRecordAt(&page, offset, length) == recordC, "leafInsert: record C retrievable and correct after multiple inserts");
}

void test_leafInsert_recordsDontOverlap() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    std::vector<uint8_t> recordA(50, 0xAA);
    std::vector<uint8_t> recordB(50, 0xBB);

    leafInsert(&page, 1, recordA);
    leafInsert(&page, 2, recordB);

    uint64_t offset;
    uint32_t length;

    leafFind(&page, 1, offset, length);
    std::vector<uint8_t> extractedA = readRecordAt(&page, offset, length);

    leafFind(&page, 2, offset, length);
    std::vector<uint8_t> extractedB = readRecordAt(&page, offset, length);

    check(extractedA == recordA, "leafInsert: record A bytes uncorrupted by record B's insertion");
    check(extractedB == recordB, "leafInsert: record B bytes uncorrupted by record A's insertion");
}

void test_leafInsert_failsWhenPageFull() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    std::vector<uint8_t> bigRecord(200, 0x01);
    int insertedCount = 0;
    while (leafInsert(&page, insertedCount, bigRecord)) {
        insertedCount++;
        if (insertedCount > 1000) break;
    }

    check(insertedCount > 0, "leafInsert: at least one record was successfully inserted before page filled");
    check(insertedCount <= 1000, "leafInsert: loop terminated (page correctly reports full via false, no infinite loop)");

    bool oneMore = leafInsert(&page, 99999, bigRecord);
    check(oneMore == false, "leafInsert: correctly returns false once page has no more room");
}

void test_leafInsert_doesNotCorruptPageWhenFull() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::LEAF);
    setNumKeys(&page, 0);

    std::vector<uint8_t> record = {1, 2, 3};
    leafInsert(&page, 1, record);
    uint32_t numKeysBefore = getNumKeys(&page);

    std::vector<uint8_t> hugeRecord(PAGE_SIZE, 0xFF);
    bool result = leafInsert(&page, 2, hugeRecord);

    check(result == false, "leafInsert: oversized record correctly rejected");
    check(getNumKeys(&page) == numKeysBefore, "leafInsert: failed insert does not change num_keys (page left untouched)");
}


void test_metaPage_roundtrip() {
    Page page = makeCleanPage();
    setNodeType(&page, NodeType::META);

    setTreeRootPage(&page, 42);
    setFormatVersion(&page, 1);

    check(getTreeRootPage(&page) == 42, "meta_page: root page round-trips");
    check(getFormatVersion(&page) == 1, "meta_page: format version round-trips");
    check(getNodeType(&page) == NodeType::META, "meta_page: node_type unaffected by root/version writes");
}

int main() {
    test_setNodeType_then_getNodeType_leaf();
    test_setNodeType_then_getNodeType_internal();
    test_setNumKeys_then_getNumKeys();
    test_nodeType_and_numKeys_dont_clobber_each_other();

    test_writeKeyAt_then_readKeyAt_single();
    test_writeKeyAt_then_readKeyAt_multiple();
    test_writeKeyAt_does_not_clobber_adjacent_entries();

    test_writeChildPointerAt_then_readChildPointerAt_single();
    test_writeChildPointerAt_then_readChildPointerAt_multiple();
    test_key_and_pointer_dont_overlap();

    test_findChildPage_targetLessThanAllKeys();
    test_findChildPage_targetGreaterThanAllKeys();
    test_findChildPage_targetBetweenKeys();
    test_findChildPage_targetExactlyEqualsFirstKey();
    test_findChildPage_targetExactlySecondKey();
    test_findChildPage_singleKeyNode();
    test_findChildPage_manyKeys_noInfiniteLoop();

    test_writeLeafKeyAt_then_readLeafKeyAt_single();
    test_writeLeafKeyAt_then_readLeafKeyAt_multiple();
    test_writeLeafOffsetAt_then_readLeafOffsetAt_single();
    test_writeLeafOffsetAt_then_readLeafOffsetAt_multiple();
    test_writeLeafLengthAt_then_readLeafLengthAt_single();
    test_leaf_key_offset_length_dont_overlap();

    test_leafFind_existingKey();
    test_leafFind_firstKey();
    test_leafFind_lastKey();
    test_leafFind_missingKey();
    test_leafFind_emptyPage();

    test_leafInsert_then_leafFind_singleRecord();
    test_leafInsert_then_readRecordAt_roundtrip();
    test_leafInsert_multipleRecords_allFindable();
    test_leafInsert_recordsDontOverlap();
    test_leafInsert_failsWhenPageFull();
    test_leafInsert_doesNotCorruptPageWhenFull();
    
    test_metaPage_roundtrip();

    std::cout << "\n" << tests_passed << " / " << tests_run << " tests passed\n";

    return (tests_passed == tests_run) ? 0 : 1;
}
