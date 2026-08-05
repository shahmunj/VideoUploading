#pragma once
#include <unordered_map>
#include <fstream>
#include <cstdint>
#include <string>

constexpr size_t PAGE_SIZE = 4096;

struct Page {
    uint8_t data[PAGE_SIZE];
    bool is_dirty = false;
};

class Pager {
    public:
        Pager(const std::string& filename);
        ~Pager(); 

        Page* getPage(uint64_t pageNumber);
        void flushPage(uint64_t pageNumber);
        uint64_t allocateNewPage();

    private:
        std::fstream file;
        std::unordered_map<uint64_t, Page> pages;
        uint64_t num_pages = 0;
};