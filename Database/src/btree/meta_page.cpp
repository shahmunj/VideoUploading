#include "../../include/btree/meta_page.hpp"

constexpr uint64_t ROOT_PAGE_OFFSET = HEADER_SIZE;
constexpr uint64_t FORMAT_VERSION_OFFSET = ROOT_PAGE_OFFSET + 8;

uint64_t getTreeRootPage(Page* metaPage){
    return readFieldAt<uint64_t>(metaPage, ROOT_PAGE_OFFSET);
}
void setTreeRootPage(Page* metaPage, uint64_t rootPage){
    writeFieldAt(metaPage, ROOT_PAGE_OFFSET, rootPage);
}

uint32_t getFormatVersion(Page* metaPage){
    return readFieldAt<uint32_t>(metaPage, FORMAT_VERSION_OFFSET);
}
void setFormatVersion(Page* metaPage, uint32_t version){
    writeFieldAt(metaPage, FORMAT_VERSION_OFFSET, version);
}
