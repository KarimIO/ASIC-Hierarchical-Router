#pragma once

typedef int Cell;

class OctreeBlock {
public:
    virtual void Search() = 0;
};


class OctreeBlock {
public:
private:
    OctreeBlock *octree_[8];
};

class OctreeLeaf : public OctreeBlock {
public:

private:
    Cell octree_[8];
};