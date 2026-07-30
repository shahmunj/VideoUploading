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

        Page* getPage(size_t pageNumber);
        void flushPage(size_t pageNumber);
        size_t allocateNewPage();

    private:
        std::fstream file;
        std::unordered_map<size_t, Page> pages;
        size_t num_pages = 0;
};