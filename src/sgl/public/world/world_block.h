#include "coremin.h"
#include "core_types.h"
#include "math/math.h"
#include "vertex.h"

class WorldBlock {

    public:
        WorldBlock(uint32 x, uint32 y, float stepSize, float startY);
        ~WorldBlock();
        void init(float minVal, float maxVal, float clipBelow, float clipAbove);
        void init(float minVal, float maxVal, float clipBelow, float clipAbove, uint32 startCol, uint32 startRow);
        void initRight(WorldBlock *leftBlock, float minVal, float maxVal, float clipBelow, float clipAbove);
        void initRight(WorldBlock *leftBlock, float minVal, float maxVal, float clipBelow, float clipAbove, uint32 startRow);
        void updateVertices();
        uint32 getX(uint32 i);
        uint32 getY(uint32 i);

        uint32 X, Y, N;
        float stepSize;

        float startY;
        float sizeY;
        float endY;

        uint32 numIndices;
        float *data;
        Vertex *vertices;
        uint32 *indices;

};