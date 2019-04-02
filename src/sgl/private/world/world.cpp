#include "world/world.h"

World::World(uint32 x, uint32 y, float stepSize, float minVal, float maxVal, float clipBelow, uint32 startRow){
    X = x;
    Y = y;
    N = x*y;
    this->stepSize = stepSize;
    this->minVal = minVal;
    this->maxVal = maxVal;
    this->clipBelow = clipBelow;
    this->startRow = startRow;
    clockTime = 0;

    numTrianglesIndices = (uint32)((x-1)*(y-1)*6);
    numLinesIndices = (uint32)(2*((x-1)*y+(y-1)*x));
    data = (float*)malloc(N*sizeof(float));
    vertices = (Vertex*)malloc(N*sizeof(Vertex));
    trianglesIndices = (uint32*)malloc(numTrianglesIndices*sizeof(uint32));
    linesIndices = (uint32*)malloc(numLinesIndices*sizeof(uint32));

    createWorld();
}

World::~World(){
    free(data);
    free(vertices);
    free(trianglesIndices);
    free(linesIndices);
}

void World::createWorld(){
    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(y<startRow) value = 0.0;
        else{
            value = ((x>0)?data[i-1]:0) + ((y>0)?data[i-X]:0);
            if(x>0 && y>0) value = value/2.0;
            float r = (-0.5+(float)(rand()%1000)/1000.0)*(stepSize/0.5);
            value += r;
        }
        if(value < minVal) value = minVal;
        data[i] = value;
    }

    if(clipBelow>minVal){
        for(uint32 i=0; i<N; i++){
            float val = data[i];
            if(val<clipBelow) val = clipBelow;
            data[i] = val;
        }
    }
    updateVertices();
}

void World::updateWorld(float dt){
    clockTime += dt;
    float temp[N];
    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(y<startRow) value = 0.0;
        else{
            value = ((x>0)?temp[i-1]:0) + ((y>0)?temp[i-X]:0);
            if(x>0 && y>0) value = value/2.0;
            float r = 0.01*(-0.5+(float)(rand()%1000)/1000.0)*(stepSize/0.5);
            value += r;
        }
        temp[i] = 0.005*sin(0.2*(float)x+3*clockTime);
        temp[i] += 0.005*sin(0.3*(float)y+1.7*clockTime);
        temp[i] += 0.005*sin(0.1*(1.25*(float)x+2*(float)y)+4*clockTime);
        temp[i] += 0.005*sin(0.05*(2.5*(float)x+0.5*(float)y)+1.7*clockTime);
    }
    
    for(uint32 i=0; i<N; i++){
        data[i] += temp[i];
        //if(data[i]<clipBelow) data[i] = clipBelow;
        //if(data[i]<minVal) data[i] = minVal;
        //if(data[i]>maxVal) data[i] = maxVal;
    }
    
    //smoothWorld(1);
    updateVertices();
}

void World::smoothWorld(int s){
    float temp[N];
    for(uint32 x=0; x<X; x++){
        for(uint32 y=0; y<Y; y++){
            uint32 startX = (((int32)x-s)>0) ? x-s : 0;
            uint32 startY = (((int32)y-s)>0) ? y-s : 0;
            uint32 endX = (((int32)x+s)<X) ? x+s : X-1;
            uint32 endY = (((int32)y+s)<Y) ? y+s : Y-1;
            float val = 0.0;
            float tot = 0.0;
            for(uint32 j=startX; j<=endX; j++){
                for(uint32 k=startY; k<=endY; k++){
                    val += data[k*X+j];
                    tot += 1.0;
                }
            }
            if(tot>0) temp[y*X+x] = val/tot;
            else temp[y*X+x] = data[y*X+x];
        }
    }
    for(uint32 i=0; i<N; i++) data[i] = temp[i];
}

void World::updateVertices(){
    for(uint32 i=0, ti=0, li=0; i<N; i++){

		float x = getX(i), y = getY(i);

		vertices[i].pos = vec3(x,0,y)*stepSize;
		vertices[i].pos.y = data[i];

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

uint32 World::getX(uint32 i){
    return i%X;
}

uint32 World::getY(uint32 i){
    return i/X;
}