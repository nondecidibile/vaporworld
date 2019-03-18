#include "world/world.h"

World::World(uint32 x, uint32 y, float minVal, float maxVal, float stepSize, float pos_x, float pos_y){
    this->X = x;
    this->Y = y;
    this->N = x*y;
    this->stepSize = stepSize;
    this->minVal = minVal;
    this->maxVal = maxVal;
    this->pos_x = pos_x;
    this->pos_y = pos_y;
    numIndices = (uint32)((x-1)*(y-1)*6);
    data = (float*)malloc(N*sizeof(float));
    vertices = (Vertex*)malloc(N*sizeof(Vertex));
    indices = (uint32*)malloc(numIndices*sizeof(uint32));
    init();
}

World::~World(){
    free(data);
    free(vertices);
    free(indices);
}

void World::init(){
    for(uint32 i=0; i<N; i++){
        uint32 x = getX(i), y = getY(i);

        float value;
        if(x==0 && y==0) value = 0.0;
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
    updateVertices();
}

void World::updateVertices(){
    for(uint32 i=0, idx=0; i<N; i++){

        uint32 x = getX(i), y = getY(i);
        float vertex_x = -(float)X/2+(float)x, vertex_y = -(float)Y/2+(float)y;

        vertices[i].pos = vec3(vertex_x,0,vertex_y)*stepSize;
        vertices[i].pos.x += pos_x;
        vertices[i].pos.z += pos_y;
        vertices[i].pos.y = data[i];
        vertices[i].color = Color{150,0,200,255};

        if(x<X-1 && y<Y-1){
            indices[idx+0] = i;
            indices[idx+1] = i+1;
            indices[idx+2] = i+X;
            indices[idx+3] = i+1;
            indices[idx+4] = i+X+1;
            indices[idx+5] = i+X;
            idx += 6;
        }
    }
}

void World::updateWorld(float new_x, float new_y){
    float dx = new_x-pos_x, dy = new_y-pos_y;
    if(abs(dx)<stepSize && abs(dy)<stepSize) {return;}
    if(abs(dx)>=stepSize*X || abs(dy)>=stepSize*Y){
        init();
        pos_x = new_x;
        pos_y = new_y;
        return;
    }
    
    int DX = (int)(dx/stepSize), DY = (int)(dy/stepSize);
    float temp[X][Y];
    for(uint32 i=0; i<N; i++){
        int x = (int)getX(i), y = (int)getY(i);
        if(x-DX >= 0 && x-DX < (int)X && y-DY >= 0 && y-DY < (int)Y){
            temp[x-DX][y-DY] = data[i];
        }
    }

    if(DX>=0 && DY>=0){

        for(int y=0; y<Y; y++){
            for(int x=0; x<X; x++){

                uint32 i = getI(x,y);
                if(x+DX >= 0 && x+DX < (int)X && y+DY >= 0 && y+DY < (int)Y){
                    data[i] = temp[x][y];
                }
                else{
                    float value = ((x>0)?data[getI(x-1,y)]:0) + ((y>0)?data[getI(x,y-1)]:0);
                    if(x>0 && y>0) value = value/2.0;
                    float r = (-0.5+(float)(rand()%1000)/1000.0)*(stepSize/0.5);
                    value += r;

                    if(value < minVal) value = minVal;
                    if(value > maxVal) value = maxVal;
                    data[i] = value;
                }
            }
        }
        
    }
    /*
    else if(DX>=0 && DY<=0){

    }
    else if(DX<=0 && DY>=0){

    }
    else{ //DX<0 and DY<0

    }
    */

    if(abs(DX)>0) pos_x = new_x;
    if(abs(DY)>0) pos_y = new_y;
    updateVertices();
}

uint32 World::getX(uint32 i){
    return i%X;
}

uint32 World::getY(uint32 i){
    return i/X;
}

uint32 World::getI(uint32 x, uint32 y){
    return y*X+x;
}

uint32 World::getNumVert(){
    return N;
}

uint32 World::getNumInd(){
    return numIndices;
}