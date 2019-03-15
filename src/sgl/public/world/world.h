#include "core_types.h"
#include "coremin.h"
#include "math/math.h"
#include "world/world_block.h"

class World {

    public:
        World(uint32 x, uint32 y, float stepSize, float minVal, float maxVal, float clipBelow, float clipAbove, uint32 startCol, uint32 startRow);
        ~World();
        void updateWorld(float y);
        
        uint32 X, Y;
        float stepSize;
        float minVal, maxVal, clipBelow, clipAbove;
        uint32 startCol, startRow;
        LinkedList<WorldBlock> worldBlocks;

};