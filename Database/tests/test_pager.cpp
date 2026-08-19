#include "../include/pager.hpp"
#include <iostream>
#include <cstdio>   // for std::remove
#include <cstring>  // for memset

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

// Helper: delete a test file if it exists, so each test starts clean
void removeFileIfExists(const std::string& filename) {
    std::remove(filename.c_str());
}

void test_constructor_creates_new_file() {
    std::string filename = "test_new.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    check(true, "constructor: opens fresh file without throwing");
    uint64_t firstPage = pager.allocateNewPage();
    check(firstPage == 0, "constructor: fresh file starts with 0 pages (first allocation is page 0)");

    removeFileIfExists(filename);
}

void test_allocate_new_page_increments() {
    std::string filename = "test_allocate.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    uint64_t p0 = pager.allocateNewPage();
    uint64_t p1 = pager.allocateNewPage();
    uint64_t p2 = pager.allocateNewPage();

    check(p0 == 0, "allocateNewPage: first call returns 0");
    check(p1 == 1, "allocateNewPage: second call returns 1");
    check(p2 == 2, "allocateNewPage: third call returns 2");

    removeFileIfExists(filename);
}

void test_getPage_returns_same_pointer_when_cached() {
    std::string filename = "test_cache.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    uint64_t pageNum = pager.allocateNewPage();

    Page* p1 = pager.getPage(pageNum);
    Page* p2 = pager.getPage(pageNum);

    check(p1 == p2, "getPage: returns the SAME pointer on repeated calls (confirms caching works)");

    removeFileIfExists(filename);
}

void test_getPage_out_of_range_throws() {
    std::string filename = "test_oob.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    bool threw = false;
    try {
        pager.getPage(999);
    } catch (const std::out_of_range&) {
        threw = true;
    }
    check(threw, "getPage: throws std::out_of_range for an unallocated page number");

    removeFileIfExists(filename);
}

void test_write_read_within_same_pager_instance() {
    std::string filename = "test_write_read.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    uint64_t pageNum = pager.allocateNewPage();

    Page* page = pager.getPage(pageNum);
    page->data[0] = 42;
    page->data[1] = 200;
    page->is_dirty = true;

    Page* page2 = pager.getPage(pageNum);
    check(page2->data[0] == 42, "write/read (same instance): byte 0 matches what was written");
    check(page2->data[1] == 200, "write/read (same instance): byte 1 matches what was written");

    removeFileIfExists(filename);
}

void test_persistence_across_pager_restart() {
    std::string filename = "test_persistence.dat";
    removeFileIfExists(filename);

    uint64_t pageNum;
    {
        Pager pager(filename);
        pageNum = pager.allocateNewPage();
        Page* page = pager.getPage(pageNum);
        memset(page->data, 0, PAGE_SIZE);
        page->data[0] = 123;
        page->data[100] = 77;
        page->data[PAGE_SIZE - 1] = 255;
        page->is_dirty = true;
    }

    {
        Pager pager(filename);
        Page* page = pager.getPage(pageNum);
        check(page->data[0] == 123, "persistence: byte 0 survived restart");
        check(page->data[100] == 77, "persistence: byte 100 survived restart");
        check(page->data[PAGE_SIZE - 1] == 255, "persistence: last byte of page survived restart");
    }

    removeFileIfExists(filename);
}

void test_flushPage_skips_clean_pages() {
    std::string filename = "test_clean_flush.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    uint64_t pageNum = pager.allocateNewPage();
    Page* page = pager.getPage(pageNum);
    page->is_dirty = false;

    bool threw = false;
    try {
        pager.flushPage(pageNum);
    } catch (...) {
        threw = true;
    }
    check(!threw, "flushPage: does not throw when flushing a clean page (no-op)");
    check(page->is_dirty == false, "flushPage: clean page remains marked clean after flush");

    removeFileIfExists(filename);
}

void test_flushPage_clears_dirty_flag() {
    std::string filename = "test_dirty_clear.dat";
    removeFileIfExists(filename);

    Pager pager(filename);
    uint64_t pageNum = pager.allocateNewPage();
    Page* page = pager.getPage(pageNum);
    page->is_dirty = true;

    pager.flushPage(pageNum);

    check(page->is_dirty == false, "flushPage: dirty flag is cleared after flushing");

    removeFileIfExists(filename);
}

void test_multiple_pages_persist_independently() {
    std::string filename = "test_multi_page.dat";
    removeFileIfExists(filename);

    {
        Pager pager(filename);
        uint64_t p0 = pager.allocateNewPage();
        uint64_t p1 = pager.allocateNewPage();
        uint64_t p2 = pager.allocateNewPage();

        Page* page0 = pager.getPage(p0);
        Page* page1 = pager.getPage(p1);
        Page* page2 = pager.getPage(p2);

        memset(page0->data, 0, PAGE_SIZE);
        memset(page1->data, 0, PAGE_SIZE);
        memset(page2->data, 0, PAGE_SIZE);

        page0->data[0] = 10;
        page1->data[0] = 20;
        page2->data[0] = 30;

        page0->is_dirty = true;
        page1->is_dirty = true;
        page2->is_dirty = true;
    }

    {
        Pager pager(filename);
        check(pager.getPage(0)->data[0] == 10, "multi-page: page 0 has correct data after restart");
        check(pager.getPage(1)->data[0] == 20, "multi-page: page 1 has correct data after restart");
        check(pager.getPage(2)->data[0] == 30, "multi-page: page 2 has correct data after restart");
    }

    removeFileIfExists(filename);
}

int main() {
    test_constructor_creates_new_file();
    test_allocate_new_page_increments();
    test_getPage_returns_same_pointer_when_cached();
    test_getPage_out_of_range_throws();
    test_write_read_within_same_pager_instance();
    test_persistence_across_pager_restart();
    test_flushPage_skips_clean_pages();
    test_flushPage_clears_dirty_flag();
    test_multiple_pages_persist_independently();

    std::cout << "\n" << tests_passed << " / " << tests_run << " tests passed\n";

    return (tests_passed == tests_run) ? 0 : 1;
}