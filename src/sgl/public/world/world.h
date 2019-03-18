#include "coremin.h"
#include "core_types.h"
#include "math/math.h"
#include "vertex.h"

class World {

    public:
        World(uint32 x, uint32 y, float minVal, float maxVal, float stepSize=0.5, float pos_x=0.0, float pos_y=0.0);
        ~World();

        void init();
        void updateWorld(float new_x, float new_y);

        Vertex *vertices;
        uint32 *indices;

        uint32 getNumVert();
        uint32 getNumInd();

    private:
        void updateVertices();

        uint32 getX(uint32 i);
        uint32 getY(uint32 i);
        uint32 getI(uint32 x, uint32 y);
        
        uint32 X, Y, N;
        float pos_x, pos_y;

        float *data;

        float stepSize;
        float minVal, maxVal;
        uint32 numIndices;


};