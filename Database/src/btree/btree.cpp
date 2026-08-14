#include "../../include/btree/btree.hpp"

BTree::BTree(Pager& pager) : pager(pager) {
    if(pager.getNumPages() == 0){
        uint64_t metaPageNum = pager.allocateNewPage();
        Page* metaPage = pager.getPage(metaPageNum);
        setNodeType(metaPage, NodeType::META);

        uint64_t rootPageNum = pager.allocateNewPage();
        Page* rootPage = pager.getPage(rootPageNum);
        setNodeType(rootPage, NodeType::LEAF);
        setNumKeys(rootPage, 0);

        setTreeRootPage(metaPage, rootPageNum);
        setFormatVersion(metaPage, 1);

        this->rootPage = rootPageNum;
    } else {
        Page* metapage = pager.getPage(0);
        this->rootPage = getTreeRootPage(metapage);
    }
}

void BTree::insert(const Video& video){
    std::vector<uint8_t> record = serialize_video(video);
    Page* root = pager.getPage(rootPage);

    bool success = leafInsert(root, video.id, record);

    if (!success) {
        throw std::runtime_error("Page full — splitting not yet implemented");
    }
}

Video BTree::find(uint32_t video_id){
    Page* root = pager.getPage(rootPage);
    uint64_t offset;
    uint32_t length;

    bool success = leafFind(root, video_id, offset, length);

    if(!success){
        throw std::runtime_error("Video not found");
    }

    std::vector<uint8_t> video_data = readRecordAt(root, offset, length);
    Video video = deserialize_video(video_data);

    return video;
}