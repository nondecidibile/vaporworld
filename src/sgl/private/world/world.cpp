#include "world/world.h"

World::World(uint32 x, uint32 y, float stepSize, float minVal, float maxVal, float clipBelow, float clipAbove, uint32 startCol, uint32 startRow){
    this->X = x;
    this->Y = y;
    this->stepSize = stepSize;
    this->minVal = minVal;
    this->maxVal = maxVal;
    this->clipBelow = clipBelow;
    this->clipAbove = clipAbove;
    this->startCol = startCol;
    this->startRow = startRow;
    updateWorld(0);
}

World::~World(){

}

void World::updateWorld(float y){

    if(worldBlocks.getCount()==0){
        WorldBlock *b = new WorldBlock(X,Y,stepSize,0.0);
        b->init(minVal,maxVal,clipBelow,clipAbove,startCol,startRow);
        worldBlocks += *b;
    }

    WorldBlock *lastBlock = &(worldBlocks[worldBlocks.getCount()-1]);
    while(y >= lastBlock->startY){
        WorldBlock *b = new WorldBlock(X,Y,stepSize,lastBlock->endY);
        b->initAfter(lastBlock,minVal,maxVal,clipBelow,clipAbove,startRow);
        worldBlocks += *b;
        lastBlock = b;
    }
    
}