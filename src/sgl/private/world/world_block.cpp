#include "world/world_block.h"

WorldBlock::WorldBlock(uint32 x, uint32 y, float stepSize, float startY){
    X = x;
    Y = y;
    N = x*y;

    this->stepSize = stepSize;
    this->startY = startY;
    sizeY = stepSize*(Y-1);
    endY = startY + sizeY;

    numIndices = (uint32)((x-1)*(y-1)*6);
    data = (float*)malloc(N*sizeof(float));
    vertices = (Vertex*)malloc(N*sizeof(Vertex));
    indices = (uint32*)malloc(numIndices*sizeof(uint32));
}

WorldBlock::~WorldBlock(){
    free(data);
    free(vertices);
    free(indices);
}

void WorldBlock::init(float minVal, float maxVal, float clipBelow, float clipAbove){
    init(minVal,maxVal,clipBelow,clipAbove,0,0);
}

void WorldBlock::init(float minVal, float maxVal, float clipBelow, float clipAbove, uint32 startCol, uint32 startRow){

    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(x<startCol || y<startRow || (x==0 && y==0)) value = 0.0;
        else{
            value = ((x>0)?data[i-1]:0) + ((y>0)?data[i-Y]:0);
            if(x>0 && y>0) value = value/2.0;
            float r = -0.5+(float)(rand()%1000)/1000.0;
            value += r;
        }
        if(value < minVal) value = minVal;
        if(value > maxVal) value = maxVal;
        data[i] = value;
    }

    if(clipBelow>minVal || clipAbove<maxVal){
        for(uint32 i=0; i<N; i++){
            float val = data[i];
            if(val<clipBelow) val = clipBelow;
            if(val>clipAbove) val = clipAbove;
            data[i] = val;
        }
    }
    updateVertices();
}

void WorldBlock::initRight(WorldBlock *leftBlock, float minVal, float maxVal, float clipBelow, float clipAbove){
    initRight(leftBlock,minVal,maxVal,clipBelow,clipAbove,0);
}

void WorldBlock::initRight(WorldBlock *leftBlock, float minVal, float maxVal, float clipBelow, float clipAbove, uint32 startRow){
    if(leftBlock->Y != Y){
        printf("Error in WorldBlock::initRight. Y sizes do not match.\n");
    }

    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(x==0){
            float leftValue = leftBlock->data[y*Y + (leftBlock->X - 1)];
            value = leftValue;
        }
        else if(y<startRow) value = 0.0;
        else{
            value = ((x>0)?data[i-1]:0) + ((y>0)?data[i-Y]:0);
            if(x>0 && y>0) value = value/2.0;
            float r = -0.5+(float)(rand()%1000)/1000.0;
            value += r;
        }
        if(value < minVal) value = minVal;
        if(value > maxVal) value = maxVal;
        data[i] = value;
    }

    if(clipBelow>minVal || clipAbove<maxVal){
        for(uint32 i=0; i<N; i++){
            float val = data[i];
            if(val<clipBelow) val = clipBelow;
            if(val>clipAbove) val = clipAbove;
            data[i] = val;
        }
    }
    updateVertices();
}

void WorldBlock::updateVertices(){
    for(uint32 i=0, idx=0; i<N; i++){

		float x = getX(i), y = getY(i);

		vertices[i].pos = vec3(x,0,y)*stepSize;
		vertices[i].pos.y = data[i];
		vertices[i].color = Color{150,0,200,255};

		if(x<X-1 && y<Y-1){
			indices[idx+0] = i;
			indices[idx+1] = i+1;
			indices[idx+2] = i+Y;
			indices[idx+3] = i+1;
			indices[idx+4] = i+Y+1;
			indices[idx+5] = i+Y;
			idx += 6;
		}
	}
}

uint32 WorldBlock::getX(uint32 i){
    return i%X;
}

uint32 WorldBlock::getY(uint32 i){
    return i/X;
}