#include "world/world_block.h"

WorldBlock::WorldBlock(uint32 x, uint32 y, float stepSize, float startY){
    X = x;
    Y = y;
    N = x*y;

    this->stepSize = stepSize;
    this->startY = startY;
    sizeY = stepSize*(Y-1);
    endY = startY + sizeY;

    numTrianglesIndices = (uint32)((x-1)*(y-1)*6);
    numLinesIndices = (uint32)(2*((x-1)*y+(y-1)*x));
    data = (float*)malloc(N*sizeof(float));
    vertices = (Vertex*)malloc(N*sizeof(Vertex));
    trianglesIndices = (uint32*)malloc(numTrianglesIndices*sizeof(uint32));
    linesIndices = (uint32*)malloc(numLinesIndices*sizeof(uint32));
}

WorldBlock::~WorldBlock(){
    free(data);
    free(vertices);
    free(trianglesIndices);
    free(linesIndices);
}

void WorldBlock::init(float minVal, float maxVal, float clipBelow, float clipAbove){
    init(minVal,maxVal,clipBelow,clipAbove,0,0);
}

void WorldBlock::init(float minVal, float maxVal, float clipBelow, float clipAbove, uint32 flatY, uint32 flatX){

    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(x<flatX || y<flatY || (x==0 && y==0)) value = 0.0;
        else{
            value = ((x>0)?data[i-1]:0) + ((y>0)?data[i-X]:0);
            if(x>0 && y>0) value = value/2.0;
            float r = (-0.5+(float)(rand()%1000)/1000.0)*(stepSize/0.5);
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

void WorldBlock::initAfter(WorldBlock *prevBlock, float minVal, float maxVal, float clipBelow, float clipAbove){
    initAfter(prevBlock,minVal,maxVal,clipBelow,clipAbove,0);
}

void WorldBlock::initAfter(WorldBlock *prevBlock, float minVal, float maxVal, float clipBelow, float clipAbove, uint32 flatX){
    if(prevBlock->X != X){
        printf("Error in WorldBlock::initRight. X sizes do not match.\n");
    }

    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(y==0){
            float prevValue = prevBlock->data[(prevBlock->Y-1)*X + x];
            value = prevValue;
        }
        else if(x<flatX) value = 0.0;
        else{
            value = ((x>0)?data[i-1]:0) + ((y>0)?data[i-X]:0);
            if(x>0 && y>0) value = value/2.0;
            float r =  (-0.5+(float)(rand()%1000)/1000.0)*(stepSize/0.5);
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
    for(uint32 i=0, ti=0, li=0; i<N; i++){

		float x = getX(i), y = getY(i);

		vertices[i].pos = vec3(x,0,y)*stepSize;
		vertices[i].pos.y = data[i];
		vertices[i].color = Color{150,0,200,255};

		if(x<X-1 && y<Y-1){
			trianglesIndices[ti+0] = i;
			trianglesIndices[ti+1] = i+1;
			trianglesIndices[ti+2] = i+X;
			trianglesIndices[ti+3] = i+1;
			trianglesIndices[ti+4] = i+X+1;
			trianglesIndices[ti+5] = i+X;
			ti += 6;
		}

        if(x>0){
            linesIndices[li+0] = i-1;
            linesIndices[li+1] = i;
            li += 2;
        }
        if(y>0){
            linesIndices[li+0] = i-X;
            linesIndices[li+1] = i;
            li += 2;
        }

	}
}

uint32 WorldBlock::getX(uint32 i){
    return i%X;
}

uint32 WorldBlock::getY(uint32 i){
    return i/X;
}