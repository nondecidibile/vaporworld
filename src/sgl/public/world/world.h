#include "core_types.h"
#include "coremin.h"
#include "math/math.h"
#include "vertex.h"

class World {

    public:
        World(uint32 x, uint32 y, float stepSize, float minVal, float maxVal, float clipBelow, uint32 startRow);
        ~World();
        void createWorld();
        void updateWorld(float dt);
        void smoothWorld(int s);
        void updateVertices();
        uint32 getX(uint32 i);
        uint32 getY(uint32 i);
        
        uint32 X, Y, N;
        float stepSize;
        float minVal, maxVal, clipBelow;
        uint32 startRow;
        float clockTime;

        float *data;
        Vertex *vertices;
        uint32 numTrianglesIndices, numLinesIndices;
        uint32 *trianglesIndices, *linesIndices;
};