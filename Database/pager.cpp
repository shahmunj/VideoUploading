#include "pager.hpp"

Pager::Pager(const std::string& filename) : file() {
    file.open(filename, std::ios::in | std::ios::out | std::ios::binary);

    if (!file.is_open()) {
        file.clear();
        file.open(filename, std::ios::out | std::ios::binary);
        file.close();
        file.open(filename, std::ios::in | std::ios::out | std::ios::binary);
    }

    if (!file.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }

    file.seekg(0, std::ios::end);
    size_t file_size = file.tellg();
    num_pages = file_size / PAGE_SIZE;
}

Pager::~Pager() {
    for (const auto& [pageNumber, page] : pages) {
        if (page.is_dirty) {
            flushPage(pageNumber);
        }
    }
    file.close();
}

Page* Pager::getPage(size_t pageNumber){
    //check unordered map
    auto it = pages.find(pageNumber);
    if(it != pages.end()){
        return &it->second;
    } 
    //if not in memory, load it from disk
    Page page{};
    file.seekg(pageNumber * PAGE_SIZE, std::ios::beg);
    file.read(reinterpret_cast<char*>(page.data), PAGE_SIZE);

    //add to unordered map
    pages[pageNumber] = page;
    return &pages[pageNumber];
}

void Pager::flushPage(size_t pageNumber){
    Page& page = pages.at(pageNumber);
    if(!page.is_dirty){
        return;
    }
    file.seekp(pageNumber * PAGE_SIZE, std::ios::beg);
    file.write(reinterpret_cast<char*>(page.data), PAGE_SIZE);
    page.is_dirty = false;
}

size_t Pager::allocateNewPage(){
    num_pages++;
    size_t pageNumber = num_pages;
    pages[pageNumber] = Page{};
    pages[pageNumber].is_dirty = true;
    return pageNumber;
}

