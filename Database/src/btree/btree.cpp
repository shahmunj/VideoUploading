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

        rootPage = rootPageNum;

    } else {
        Page* metapage = pager.getPage(0);
        rootPage = getTreeRootPage(metapage);
    }
}