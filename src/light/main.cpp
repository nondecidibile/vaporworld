#include "coremin.h"
#include "math/math.h"
#include "gldrv/gldrv.h"

#include <SDL.h>

#if SGL_BUILD_RELEASE
	#define LOG(...)
#else
	#define LOG(...) printf(__VA_ARGS__)
#endif

Malloc * gMalloc = nullptr;

/// Edge connect list
const int32 edgeConnectList[256][16] = {
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 9, 8, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 0, 2, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 8, 3, 2, 10, 8, 10, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 8, 11, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 2, 1, 9, 11, 9, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 1, 11, 10, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 10, 1, 0, 8, 10, 8, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{3, 9, 0, 3, 11, 9, 11, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 7, 3, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 1, 9, 4, 7, 1, 7, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 4, 7, 3, 0, 4, 1, 2, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 2, 10, 9, 0, 2, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 9, 2, 9, 7, 2, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{8, 4, 7, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 4, 7, 11, 2, 4, 2, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 8, 4, 7, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{4, 7, 11, 9, 4, 11, 9, 11, 2, 9, 2, 1, -1, -1, -1, -1},
	{3, 10, 1, 3, 11, 10, 7, 8, 4, -1, -1, -1, -1, -1, -1, -1},
	{1, 11, 10, 1, 4, 11, 1, 0, 4, 7, 11, 4, -1, -1, -1, -1},
	{4, 7, 8, 9, 0, 11, 9, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{4, 7, 11, 4, 11, 9, 9, 11, 10, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 1, 5, 0, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 5, 4, 8, 3, 5, 3, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 10, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 2, 10, 5, 4, 2, 4, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{2, 10, 5, 3, 2, 5, 3, 5, 4, 3, 4, 8, -1, -1, -1, -1},
	{9, 5, 4, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 11, 2, 0, 8, 11, 4, 9, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 5, 4, 0, 1, 5, 2, 3, 11, -1, -1, -1, -1, -1, -1, -1},
	{2, 1, 5, 2, 5, 8, 2, 8, 11, 4, 8, 5, -1, -1, -1, -1},
	{10, 3, 11, 10, 1, 3, 9, 5, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 0, 8, 1, 8, 10, 1, 8, 11, 10, -1, -1, -1, -1},
	{5, 4, 0, 5, 0, 11, 5, 11, 10, 11, 0, 3, -1, -1, -1, -1},
	{5, 4, 8, 5, 8, 10, 10, 8, 11, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 5, 7, 9, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 3, 0, 9, 5, 3, 5, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 8, 0, 1, 7, 1, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 7, 8, 9, 5, 7, 10, 1, 2, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 9, 5, 0, 5, 3, 0, 5, 7, 3, -1, -1, -1, -1},
	{8, 0, 2, 8, 2, 5, 8, 5, 7, 10, 5, 2, -1, -1, -1, -1},
	{2, 10, 5, 2, 5, 3, 3, 5, 7, -1, -1, -1, -1, -1, -1, -1},
	{7, 9, 5, 7, 8, 9, 3, 11, 2, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 7, 9, 7, 2, 9, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{2, 3, 11, 0, 1, 8, 1, 7, 8, 1, 5, 7, -1, -1, -1, -1},
	{11, 2, 1, 11, 1, 7, 7, 1, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 8, 8, 5, 7, 10, 1, 3, 10, 3, 11, -1, -1, -1, -1},
	{5, 7, 0, 5, 0, 9, 7, 11, 0, 1, 0, 10, 11, 10, 0, -1},
	{11, 10, 0, 11, 0, 3, 10, 5, 0, 8, 0, 7, 5, 7, 0, -1},
	{11, 10, 5, 7, 11, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 8, 3, 1, 9, 8, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 2, 6, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 5, 1, 2, 6, 3, 0, 8, -1, -1, -1, -1, -1, -1, -1},
	{9, 6, 5, 9, 0, 6, 0, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 9, 8, 5, 8, 2, 5, 2, 6, 3, 2, 8, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 0, 8, 11, 2, 0, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 1, 9, 2, 9, 11, 2, 9, 8, 11, -1, -1, -1, -1},
	{6, 3, 11, 6, 5, 3, 5, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 11, 0, 11, 5, 0, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{3, 11, 6, 0, 3, 6, 0, 6, 5, 0, 5, 9, -1, -1, -1, -1},
	{6, 5, 9, 6, 9, 11, 11, 9, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 3, 0, 4, 7, 3, 6, 5, 10, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 5, 10, 6, 8, 4, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 6, 5, 1, 9, 7, 1, 7, 3, 7, 9, 4, -1, -1, -1, -1},
	{6, 1, 2, 6, 5, 1, 4, 7, 8, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 5, 5, 2, 6, 3, 0, 4, 3, 4, 7, -1, -1, -1, -1},
	{8, 4, 7, 9, 0, 5, 0, 6, 5, 0, 2, 6, -1, -1, -1, -1},
	{7, 3, 9, 7, 9, 4, 3, 2, 9, 5, 9, 6, 2, 6, 9, -1},
	{3, 11, 2, 7, 8, 4, 10, 6, 5, -1, -1, -1, -1, -1, -1, -1},
	{5, 10, 6, 4, 7, 2, 4, 2, 0, 2, 7, 11, -1, -1, -1, -1},
	{0, 1, 9, 4, 7, 8, 2, 3, 11, 5, 10, 6, -1, -1, -1, -1},
	{9, 2, 1, 9, 11, 2, 9, 4, 11, 7, 11, 4, 5, 10, 6, -1},
	{8, 4, 7, 3, 11, 5, 3, 5, 1, 5, 11, 6, -1, -1, -1, -1},
	{5, 1, 11, 5, 11, 6, 1, 0, 11, 7, 11, 4, 0, 4, 11, -1},
	{0, 5, 9, 0, 6, 5, 0, 3, 6, 11, 6, 3, 8, 4, 7, -1},
	{6, 5, 9, 6, 9, 11, 4, 7, 9, 7, 11, 9, -1, -1, -1, -1},
	{10, 4, 9, 6, 4, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 10, 6, 4, 9, 10, 0, 8, 3, -1, -1, -1, -1, -1, -1, -1},
	{10, 0, 1, 10, 6, 0, 6, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 1, 8, 1, 6, 8, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{1, 4, 9, 1, 2, 4, 2, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 1, 2, 9, 2, 4, 9, 2, 6, 4, -1, -1, -1, -1},
	{0, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 3, 2, 8, 2, 4, 4, 2, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 4, 9, 10, 6, 4, 11, 2, 3, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 2, 2, 8, 11, 4, 9, 10, 4, 10, 6, -1, -1, -1, -1},
	{3, 11, 2, 0, 1, 6, 0, 6, 4, 6, 1, 10, -1, -1, -1, -1},
	{6, 4, 1, 6, 1, 10, 4, 8, 1, 2, 1, 11, 8, 11, 1, -1},
	{9, 6, 4, 9, 3, 6, 9, 1, 3, 11, 6, 3, -1, -1, -1, -1},
	{8, 11, 1, 8, 1, 0, 11, 6, 1, 9, 1, 4, 6, 4, 1, -1},
	{3, 11, 6, 3, 6, 0, 0, 6, 4, -1, -1, -1, -1, -1, -1, -1},
	{6, 4, 8, 11, 6, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 10, 6, 7, 8, 10, 8, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 7, 3, 0, 10, 7, 0, 9, 10, 6, 7, 10, -1, -1, -1, -1},
	{10, 6, 7, 1, 10, 7, 1, 7, 8, 1, 8, 0, -1, -1, -1, -1},
	{10, 6, 7, 10, 7, 1, 1, 7, 3, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 6, 1, 6, 8, 1, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 6, 9, 2, 9, 1, 6, 7, 9, 0, 9, 3, 7, 3, 9, -1},
	{7, 8, 0, 7, 0, 6, 6, 0, 2, -1, -1, -1, -1, -1, -1, -1},
	{7, 3, 2, 6, 7, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 11, 10, 6, 8, 10, 8, 9, 8, 6, 7, -1, -1, -1, -1},
	{2, 0, 7, 2, 7, 11, 0, 9, 7, 6, 7, 10, 9, 10, 7, -1},
	{1, 8, 0, 1, 7, 8, 1, 10, 7, 6, 7, 10, 2, 3, 11, -1},
	{11, 2, 1, 11, 1, 7, 10, 6, 1, 6, 7, 1, -1, -1, -1, -1},
	{8, 9, 6, 8, 6, 7, 9, 1, 6, 11, 6, 3, 1, 3, 6, -1},
	{0, 9, 1, 11, 6, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 8, 0, 7, 0, 6, 3, 11, 0, 11, 6, 0, -1, -1, -1, -1},
	{7, 11, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 8, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 9, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 9, 8, 3, 1, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{10, 1, 2, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 8, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{2, 9, 0, 2, 10, 9, 6, 11, 7, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 2, 10, 3, 10, 8, 3, 10, 9, 8, -1, -1, -1, -1},
	{7, 2, 3, 6, 2, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{7, 0, 8, 7, 6, 0, 6, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{2, 7, 6, 2, 3, 7, 0, 1, 9, -1, -1, -1, -1, -1, -1, -1},
	{1, 6, 2, 1, 8, 6, 1, 9, 8, 8, 7, 6, -1, -1, -1, -1},
	{10, 7, 6, 10, 1, 7, 1, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 6, 1, 7, 10, 1, 8, 7, 1, 0, 8, -1, -1, -1, -1},
	{0, 3, 7, 0, 7, 10, 0, 10, 9, 6, 10, 7, -1, -1, -1, -1},
	{7, 6, 10, 7, 10, 8, 8, 10, 9, -1, -1, -1, -1, -1, -1, -1},
	{6, 8, 4, 11, 8, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 3, 0, 6, 0, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 6, 11, 8, 4, 6, 9, 0, 1, -1, -1, -1, -1, -1, -1, -1},
	{9, 4, 6, 9, 6, 3, 9, 3, 1, 11, 3, 6, -1, -1, -1, -1},
	{6, 8, 4, 6, 11, 8, 2, 10, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 3, 0, 11, 0, 6, 11, 0, 4, 6, -1, -1, -1, -1},
	{4, 11, 8, 4, 6, 11, 0, 2, 9, 2, 10, 9, -1, -1, -1, -1},
	{10, 9, 3, 10, 3, 2, 9, 4, 3, 11, 3, 6, 4, 6, 3, -1},
	{8, 2, 3, 8, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 2, 4, 6, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 9, 0, 2, 3, 4, 2, 4, 6, 4, 3, 8, -1, -1, -1, -1},
	{1, 9, 4, 1, 4, 2, 2, 4, 6, -1, -1, -1, -1, -1, -1, -1},
	{8, 1, 3, 8, 6, 1, 8, 4, 6, 6, 10, 1, -1, -1, -1, -1},
	{10, 1, 0, 10, 0, 6, 6, 0, 4, -1, -1, -1, -1, -1, -1, -1},
	{4, 6, 3, 4, 3, 8, 6, 10, 3, 0, 3, 9, 10, 9, 3, -1},
	{10, 9, 4, 6, 10, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 5, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 5, 11, 7, 6, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 1, 5, 4, 0, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 6, 8, 3, 4, 3, 5, 4, 3, 1, 5, -1, -1, -1, -1},
	{9, 5, 4, 10, 1, 2, 7, 6, 11, -1, -1, -1, -1, -1, -1, -1},
	{6, 11, 7, 1, 2, 10, 0, 8, 3, 4, 9, 5, -1, -1, -1, -1},
	{7, 6, 11, 5, 4, 10, 4, 2, 10, 4, 0, 2, -1, -1, -1, -1},
	{3, 4, 8, 3, 5, 4, 3, 2, 5, 10, 5, 2, 11, 7, 6, -1},
	{7, 2, 3, 7, 6, 2, 5, 4, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 5, 4, 0, 8, 6, 0, 6, 2, 6, 8, 7, -1, -1, -1, -1},
	{3, 6, 2, 3, 7, 6, 1, 5, 0, 5, 4, 0, -1, -1, -1, -1},
	{6, 2, 8, 6, 8, 7, 2, 1, 8, 4, 8, 5, 1, 5, 8, -1},
	{9, 5, 4, 10, 1, 6, 1, 7, 6, 1, 3, 7, -1, -1, -1, -1},
	{1, 6, 10, 1, 7, 6, 1, 0, 7, 8, 7, 0, 9, 5, 4, -1},
	{4, 0, 10, 4, 10, 5, 0, 3, 10, 6, 10, 7, 3, 7, 10, -1},
	{7, 6, 10, 7, 10, 8, 5, 4, 10, 4, 8, 10, -1, -1, -1, -1},
	{6, 9, 5, 6, 11, 9, 11, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{3, 6, 11, 0, 6, 3, 0, 5, 6, 0, 9, 5, -1, -1, -1, -1},
	{0, 11, 8, 0, 5, 11, 0, 1, 5, 5, 6, 11, -1, -1, -1, -1},
	{6, 11, 3, 6, 3, 5, 5, 3, 1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 10, 9, 5, 11, 9, 11, 8, 11, 5, 6, -1, -1, -1, -1},
	{0, 11, 3, 0, 6, 11, 0, 9, 6, 5, 6, 9, 1, 2, 10, -1},
	{11, 8, 5, 11, 5, 6, 8, 0, 5, 10, 5, 2, 0, 2, 5, -1},
	{6, 11, 3, 6, 3, 5, 2, 10, 3, 10, 5, 3, -1, -1, -1, -1},
	{5, 8, 9, 5, 2, 8, 5, 6, 2, 3, 8, 2, -1, -1, -1, -1},
	{9, 5, 6, 9, 6, 0, 0, 6, 2, -1, -1, -1, -1, -1, -1, -1},
	{1, 5, 8, 1, 8, 0, 5, 6, 8, 3, 8, 2, 6, 2, 8, -1},
	{1, 5, 6, 2, 1, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 6, 1, 6, 10, 3, 8, 6, 5, 6, 9, 8, 9, 6, -1},
	{10, 1, 0, 10, 0, 6, 9, 5, 0, 5, 6, 0, -1, -1, -1, -1},
	{0, 3, 8, 5, 6, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{10, 5, 6, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 7, 5, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{11, 5, 10, 11, 7, 5, 8, 3, 0, -1, -1, -1, -1, -1, -1, -1},
	{5, 11, 7, 5, 10, 11, 1, 9, 0, -1, -1, -1, -1, -1, -1, -1},
	{10, 7, 5, 10, 11, 7, 9, 8, 1, 8, 3, 1, -1, -1, -1, -1},
	{11, 1, 2, 11, 7, 1, 7, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 1, 2, 7, 1, 7, 5, 7, 2, 11, -1, -1, -1, -1},
	{9, 7, 5, 9, 2, 7, 9, 0, 2, 2, 11, 7, -1, -1, -1, -1},
	{7, 5, 2, 7, 2, 11, 5, 9, 2, 3, 2, 8, 9, 8, 2, -1},
	{2, 5, 10, 2, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{8, 2, 0, 8, 5, 2, 8, 7, 5, 10, 2, 5, -1, -1, -1, -1},
	{9, 0, 1, 5, 10, 3, 5, 3, 7, 3, 10, 2, -1, -1, -1, -1},
	{9, 8, 2, 9, 2, 1, 8, 7, 2, 10, 2, 5, 7, 5, 2, -1},
	{1, 3, 5, 3, 7, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 7, 0, 7, 1, 1, 7, 5, -1, -1, -1, -1, -1, -1, -1},
	{9, 0, 3, 9, 3, 5, 5, 3, 7, -1, -1, -1, -1, -1, -1, -1},
	{9, 8, 7, 5, 9, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{5, 8, 4, 5, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{5, 0, 4, 5, 11, 0, 5, 10, 11, 11, 3, 0, -1, -1, -1, -1},
	{0, 1, 9, 8, 4, 10, 8, 10, 11, 10, 4, 5, -1, -1, -1, -1},
	{10, 11, 4, 10, 4, 5, 11, 3, 4, 9, 4, 1, 3, 1, 4, -1},
	{2, 5, 1, 2, 8, 5, 2, 11, 8, 4, 5, 8, -1, -1, -1, -1},
	{0, 4, 11, 0, 11, 3, 4, 5, 11, 2, 11, 1, 5, 1, 11, -1},
	{0, 2, 5, 0, 5, 9, 2, 11, 5, 4, 5, 8, 11, 8, 5, -1},
	{9, 4, 5, 2, 11, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 5, 10, 3, 5, 2, 3, 4, 5, 3, 8, 4, -1, -1, -1, -1},
	{5, 10, 2, 5, 2, 4, 4, 2, 0, -1, -1, -1, -1, -1, -1, -1},
	{3, 10, 2, 3, 5, 10, 3, 8, 5, 4, 5, 8, 0, 1, 9, -1},
	{5, 10, 2, 5, 2, 4, 1, 9, 2, 9, 4, 2, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 3, 5, 1, -1, -1, -1, -1, -1, -1, -1},
	{0, 4, 5, 1, 0, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{8, 4, 5, 8, 5, 3, 9, 0, 5, 0, 3, 5, -1, -1, -1, -1},
	{9, 4, 5, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 11, 7, 4, 9, 11, 9, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{0, 8, 3, 4, 9, 7, 9, 11, 7, 9, 10, 11, -1, -1, -1, -1},
	{1, 10, 11, 1, 11, 4, 1, 4, 0, 7, 4, 11, -1, -1, -1, -1},
	{3, 1, 4, 3, 4, 8, 1, 10, 4, 7, 4, 11, 10, 11, 4, -1},
	{4, 11, 7, 9, 11, 4, 9, 2, 11, 9, 1, 2, -1, -1, -1, -1},
	{9, 7, 4, 9, 11, 7, 9, 1, 11, 2, 11, 1, 0, 8, 3, -1},
	{11, 7, 4, 11, 4, 2, 2, 4, 0, -1, -1, -1, -1, -1, -1, -1},
	{11, 7, 4, 11, 4, 2, 8, 3, 4, 3, 2, 4, -1, -1, -1, -1},
	{2, 9, 10, 2, 7, 9, 2, 3, 7, 7, 4, 9, -1, -1, -1, -1},
	{9, 10, 7, 9, 7, 4, 10, 2, 7, 8, 7, 0, 2, 0, 7, -1},
	{3, 7, 10, 3, 10, 2, 7, 4, 10, 1, 10, 0, 4, 0, 10, -1},
	{1, 10, 2, 8, 7, 4, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 7, 1, 3, -1, -1, -1, -1, -1, -1, -1},
	{4, 9, 1, 4, 1, 7, 0, 8, 1, 8, 7, 1, -1, -1, -1, -1},
	{4, 0, 3, 7, 4, 3, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{4, 8, 7, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 8, 10, 11, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 11, 9, 10, -1, -1, -1, -1, -1, -1, -1},
	{0, 1, 10, 0, 10, 8, 8, 10, 11, -1, -1, -1, -1, -1, -1, -1},
	{3, 1, 10, 11, 3, 10, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 2, 11, 1, 11, 9, 9, 11, 8, -1, -1, -1, -1, -1, -1, -1},
	{3, 0, 9, 3, 9, 11, 1, 2, 9, 2, 11, 9, -1, -1, -1, -1},
	{0, 2, 11, 8, 0, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{3, 2, 11, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 10, 8, 9, -1, -1, -1, -1, -1, -1, -1},
	{9, 10, 2, 0, 9, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{2, 3, 8, 2, 8, 10, 0, 1, 8, 1, 10, 8, -1, -1, -1, -1},
	{1, 10, 2, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{1, 3, 8, 9, 1, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 9, 1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{0, 3, 8, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1},
	{-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1}
};

uint32 zeroReg = 0;

/// Screen variables
point2 fboSize;

/// Time variables
float32 dt;
float32 currTime;
uint64 currTick;
uint64 prevTick;

/// Camera variables
vec3 cameraLocation;
vec3 cameraVelocity;
quat cameraRotation;
mat4 cameraTransform;
mat4 projectionMatrix;

union VertexVec3
{
	float32 array[3];

	struct
	{
		float32 x, y, z;
	};

	struct
	{
		float32 r, g, b;
	};
};

struct VertexColor
{
	uint32 data;

	struct
	{
		ubyte r, g, b, a;
	};
};

struct VertexData
{
	VertexVec3 pos;
	VertexVec3 norm;
	VertexColor color;
};

class FileReader
{
protected:
	FILE * fp;
	void * buffer;
	uint64 len;

public:
	FileReader(const char * filename) :
		fp(fopen(filename, "r"))
	{
		if (fp)
		{
			// Get file length
			fseek(fp, 0, SEEK_END);
			len = ftell(fp);
			fseek(fp, 0, SEEK_SET);

			// Allocate buffer
			if (posix_memalign((void**)&buffer, sizeof(void*), len + 1) == 0)
			{
				get<char>()[len] = 0;
				fread(buffer, 1, len, fp);
			}
		}
	}

	~FileReader()
	{
		if (fp) fclose(fp);
		if (buffer) free(buffer);
	}

	FORCE_INLINE bool isValid() const { return fp && buffer && len > 0; }

	template<typename T>
	FORCE_INLINE T * get() { return reinterpret_cast<T*>(buffer); }
};

class ShaderProgram
{
protected:
	/// Program name
	uint32 name;

	/// Uniforms map
	Map<String, uint32> uniforms;

public:
	/// Default constructor
	FORCE_INLINE ShaderProgram() :
		name(glCreateProgram()),
		uniforms{} {}
	
	/// Set active
	FORCE_INLINE void bind()
	{
		glUseProgram(name);
	}

public:
	/// Set shader
	FORCE_INLINE void setShader(uint32 shader)
	{
		glAttachShader(name, shader);
	}

	/// Link program
	FORCE_INLINE void link()
	{
		glLinkProgram(name);
	}

	/// Get program status
	FORCE_INLINE int32 getStatus(uint32 iv = GL_LINK_STATUS)
	{
		int32 status = 0;
		glGetProgramiv(name, iv, &status);
		return status;
	}

protected:
	/// Set uniform
	template<typename T, typename Lambda>
	void setUniform_internal(const String & key, T val, Lambda glFun)
	{
		auto it = uniforms.find(key);
		if (it != uniforms.end())
			glFun(it->second, val);
		else
		{
			uint32 slot = glGetUniformLocation(name, *key);
			glFun(slot, val);

			// Cache in map
			uniforms.insert(key, slot);
		}
	}

public:
	/// Set uniform
	template<typename T>
	void setUniform(const String & key, T val);
};

template<>
FORCE_INLINE void ShaderProgram::setUniform<float32>(const String & key, float32 val)
{
	setUniform_internal(key, val, glUniform1f);
}

template<>
FORCE_INLINE void ShaderProgram::setUniform<int32>(const String & key, int32 val)
{
	setUniform_internal(key, val, glUniform1i);
}

template<>
FORCE_INLINE void ShaderProgram::setUniform<uint32>(const String & key, uint32 val)
{
	setUniform_internal(key, val, glUniform1ui);
}

template<>
FORCE_INLINE void ShaderProgram::setUniform<Vec2<int32>>(const String & key, Vec2<int32> val)
{
	setUniform_internal(key, val, [](uint32 slot, Vec2<int32> val) {

		glUniform2iv(slot, 1, val.buffer);
	});
}

template<>
FORCE_INLINE void ShaderProgram::setUniform<const Vec3<float32, true>&>(const String & key, const Vec3<float32, true> & val)
{
	setUniform_internal(key, val, [](uint32 slot, const Vec3<float32, true> & val) {

		glUniform3fv(slot, 1, val.buffer);
	});
}

template<>
FORCE_INLINE void ShaderProgram::setUniform<const Vec3<float32, false>&>(const String & key, const Vec3<float32, false> & val)
{
	setUniform_internal(key, val, [](uint32 slot, const Vec3<float32, false> & val) {

		glUniform3fv(slot, 1, val.buffer);
	});
}

template<>
FORCE_INLINE void ShaderProgram::setUniform<const mat4&>(const String & key, const mat4 & val)
{
	setUniform_internal(key, val, [](uint32 slot, const mat4 & val) {

		glUniformMatrix4fv(slot, 1, GL_TRUE, val.array);
	});
}

ShaderProgram initTerrainProg()
{
	ShaderProgram prog;

	uint32 vShader = glCreateShader(GL_VERTEX_SHADER);
	uint32 gShader = glCreateShader(GL_GEOMETRY_SHADER);
	uint32 fShader = glCreateShader(GL_FRAGMENT_SHADER);

	{
		FileReader source("/src/light/shaders/terrain/.vert");
		const char * buffer = source.get<char>();
		glShaderSource(vShader, 1, &buffer, nullptr);
		glCompileShader(vShader);
		prog.setShader(vShader);
	}

	{
		FileReader source("/src/light/shaders/terrain/.geom");
		const char * buffer = source.get<char>();
		glShaderSource(gShader, 1, &buffer, nullptr);
		glCompileShader(gShader);
		prog.setShader(gShader);
	}

	{
		FileReader source("/src/light/shaders/terrain/.frag");
		const char * buffer = source.get<char>();
		glShaderSource(fShader, 1, &buffer, nullptr);
		glCompileShader(fShader);
		prog.setShader(fShader);
	}

	prog.link();
	return prog;
}

void setupPerlin()
{
	int32 perms[0x100];
	Vec3<float32, false> grads[0x100];
	const float32 freq = 2.f * M_PI / (float32)0x100;

	// Fill uniformly
	for (uint32 i = 0; i < 0x100; ++i)
		perms[i] = i;
	
	// Shuffle
	for (uint32 i = 0, range = 0x100; i < 0x100; ++i, --range)
	{
		uint8 k = rand() % range + i;
		swap(perms[i], perms[k]);
	}

	// Generate gradients
	for (uint32 i = 0; i < 0x100; ++i)
		grads[i] = Vec3<float32, false>(
			Math::cos(perms[i] * freq),
			Math::cos(perms[perms[i]] * freq),
			Math::sin(perms[i] * freq)
		).normalize();
	
	// Upload to GPU
	sizet offset;
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, (offset = 0x100 * sizeof(perms[0])), perms);
	glBufferSubData(GL_SHADER_STORAGE_BUFFER, offset, 0x100 * sizeof(grads[0]), grads);
};

int32 main()
{
	Memory::createGMalloc();
	srand(clock());

	Map<uint32, float32> keys;
	Map<String, float32> axes;
	axes["mouseX"] = 0.f;
	axes["mouseY"] = 0.f;

	initOpenGL();

	fboSize = point2(1920, 1080);

	SDL_Window * window = SDL_CreateWindow("light", 0, 0, fboSize.x, fboSize.y, SDL_WINDOW_OPENGL | SDL_WINDOW_BORDERLESS);
	SDL_GLContext context = SDL_GL_CreateContext(window);
	SDL_GL_SetSwapInterval(0);
	SDL_SetRelativeMouseMode(SDL_TRUE);

	glEnable(GL_DEPTH_TEST);

	//////////////////////////////////////////////////
	// Program setup
	//////////////////////////////////////////////////

	// Setup perlin noise tables
	uint32 perlinTables;
	const sizet perlinTableSize = 0x100 * (sizeof(int32) + sizeof(Vec3<float32, false>));
	glGenBuffers(1, &perlinTables);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, perlinTables);
	glBufferData(GL_SHADER_STORAGE_BUFFER, perlinTableSize, nullptr, GL_STATIC_DRAW);
	setupPerlin();

	// Setup marching cube tables
	uint32 marchingTable;
	glGenBuffers(1, &marchingTable);
	glBindBuffer(GL_SHADER_STORAGE_BUFFER, marchingTable);
	glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(edgeConnectList), edgeConnectList, GL_STATIC_DRAW);
	
#if 0
	ShaderProgram drawProg;
	uint32 drawShader = glCreateShader(GL_COMPUTE_SHADER);

	{
		FileReader source = "src/light/shaders/volume/.comp";
		const char * buffer = source.get<char>();
		glShaderSource(drawShader, 1, &buffer, nullptr);
		glCompileShader(drawShader);
		drawProg.setShader(drawShader);

		int32 status = 0;
		glGetShaderiv(drawShader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) printf("shader not compiled\n");
	}
	
	drawProg.link();
	drawProg.bind();
	if (drawProg.getStatus() == 0) printf("draw program not linked correctly\n");
	
	ShaderProgram genProg;
	uint32 cShader = glCreateShader(GL_COMPUTE_SHADER);

	{
		FileReader source = "src/light/shaders/generation/.comp";
		const char * buffer = source.get<char>();
		glShaderSource(cShader, 1, &buffer, nullptr);
		glCompileShader(cShader);
		genProg.setShader(cShader);

		int32 status = 0;
		glGetShaderiv(cShader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) printf("shader not compiled\n");
	}
	
	genProg.link();
	genProg.bind();
	if (genProg.getStatus() == 0) printf("generation program not linked correctly\n");

	ShaderProgram terrainProg = initTerrainProg();

	uint32 drawFbo;
	glGenFramebuffers(1, &drawFbo);
	glBindFramebuffer(GL_FRAMEBUFFER, drawFbo);

	uint32 colorBuffer;
	glGenTextures(1, &colorBuffer);
	glBindTexture(GL_TEXTURE_2D, colorBuffer);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, fboSize.x, fboSize.y, 0, GL_RGBA, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, colorBuffer, 0);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	uint32 volumeData;
	glGenTextures(1, &volumeData);
	glBindTexture(GL_TEXTURE_3D, volumeData);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 256, 256, 256, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	/// Generate volume data
	LOG("generating volume data ...\n");
	genProg.bind();
	glBindImageTexture(0, volumeData, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, perlinTables);
	glDispatchCompute(256 / 8, 256 / 8, 256 / 8);
	glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);
	LOG("volume data generated ...\n");
#else
	ShaderProgram genProg;
	{
		uint32 shader = glCreateShader(GL_COMPUTE_SHADER);
		FileReader source = "src/light/shaders/march/gen.comp";
		const char * buffer = source.get<char>();
		glShaderSource(shader, 1, &buffer, nullptr);
		glCompileShader(shader);
		genProg.setShader(shader);

		int32 status = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) printf("shader not compiled\n");
	}
	
	genProg.link();
	genProg.bind();
	if (genProg.getStatus() == 0) printf("generation program not linked correctly\n");

	ShaderProgram marchProg;
	{
		uint32 shader = glCreateShader(GL_COMPUTE_SHADER);
		FileReader source = "src/light/shaders/march/march.comp";
		const char * buffer = source.get<char>();
		glShaderSource(shader, 1, &buffer, nullptr);
		glCompileShader(shader);
		marchProg.setShader(shader);

		int32 status = 0;
		glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
		if (status == GL_FALSE) printf("shader not compiled\n");
	}

	marchProg.link();
	marchProg.bind();
	if (genProg.getStatus() == 0) printf("marching program not linked correctly\n");

	ShaderProgram drawProg;
	{
		uint32 shader = glCreateShader(GL_VERTEX_SHADER);
		FileReader source = "src/light/shaders/default/.vert";
		const char * buffer = source.get<char>();
		glShaderSource(shader, 1, &buffer, nullptr);
		glCompileShader(shader);
		drawProg.setShader(shader);
	}
	{
		uint32 shader = glCreateShader(GL_FRAGMENT_SHADER);
		FileReader source = "src/light/shaders/default/.frag";
		const char * buffer = source.get<char>();
		glShaderSource(shader, 1, &buffer, nullptr);
		glCompileShader(shader);
		drawProg.setShader(shader);
	}
	
	drawProg.link();
	drawProg.bind();
	if (drawProg.getStatus() == 0) printf("drawing program not linked correctly\n");

	//////////////////////////////////////////////////
	// Buffer setup
	//////////////////////////////////////////////////

	struct Chunk
	{
		/// Vertex index, zero indicates no geometry generated
		uint32 index;

		/// Chunk origin
		VertexVec3 origin;

		struct
		{
			/// Vertex position
			VertexVec3 pos;

			/// Vertex normal
			VertexVec3 norm;
		} vertices[32 * 32 * 32 * 5];
	} chunk;

#if SGL_BUILD_DEBUG
	for (uint32 i = 0; i < 32 * 32 * 5; ++i)
		chunk.vertices[i] = {
			VertexVec3{rand() / (float32)RAND_MAX, rand() / (float32)RAND_MAX, rand() / (float32)RAND_MAX},
			VertexVec3{rand() / (float32)RAND_MAX, rand() / (float32)RAND_MAX, rand() / (float32)RAND_MAX}
		};
#endif

	uint32 vao, vbo;
	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, sizeof(chunk), &chunk, GL_DYNAMIC_COPY);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * 4, (void*)offsetof(Chunk, vertices));
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * 4, (void*)(offsetof(Chunk, vertices) + sizeof(VertexVec3)));
	glEnableVertexAttribArray(0);

	uint32 volumeData;
	glGenTextures(1, &volumeData);
	glBindTexture(GL_TEXTURE_3D, volumeData);
	glTexImage3D(GL_TEXTURE_3D, 0, GL_R32F, 33, 33, 33, 0, GL_RED, GL_FLOAT, nullptr);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
#endif

	point3 groupSize(0);
	for (uint32 i = 0; i < 3; ++i)
		glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, i, groupSize.buffer + i);
	
	groupSize.print();

	//////////////////////////////////////////////////
	// Camera setup
	//////////////////////////////////////////////////
	
	projectionMatrix = mat4::glProjection(M_PI_2, 0.5f);
	cameraLocation = vec3(0.f, 0.f, -5.f);
	cameraVelocity = vec3::zero;
	cameraRotation = quat(0.f, vec3::up);

	//////////////////////////////////////////////////
	// Main loop
	//////////////////////////////////////////////////

	bool bRunning = true;
	while (bRunning)
	{
		// Update time variables
		currTime += (dt = ((currTick = SDL_GetPerformanceCounter()) - prevTick) / (float32)SDL_GetPerformanceFrequency());
		prevTick = currTick;

		//printf("%f s -> %f fps\n", dt, 1.f / dt);

		SDL_Event e;
		while (SDL_PollEvent(&e))
		{
			switch (e.type)
			{
				case SDL_QUIT:
					bRunning = false;
					break;
				
				case SDL_KEYDOWN:
					keys[e.key.keysym.sym] = 1.f;
					bRunning &= e.key.keysym.sym != SDLK_ESCAPE;
					break;
				
				case SDL_KEYUP:
					keys[e.key.keysym.sym] = 0.f;
					break;

				case SDL_MOUSEMOTION:
					axes["mouseX"] = e.motion.xrel;
					axes["mouseY"] = e.motion.yrel;
					break;
			}
		}

		//////////////////////////////////////////////////
		// Camera position and rotation
		//////////////////////////////////////////////////

		const float32 cameraSpeed = 4.f;
		const float32 cameraBrake = 2.f;
		vec3 cameraAcceleration = cameraRotation * vec3(
			keys[SDLK_d] - keys[SDLK_a],
			keys[SDLK_SPACE] - keys[SDLK_LCTRL],
			keys[SDLK_w] - keys[SDLK_s]
		) * cameraSpeed - (cameraVelocity) * cameraBrake;
		cameraVelocity += cameraAcceleration * dt;
		cameraLocation += cameraVelocity * dt;

		cameraRotation
			= quat((keys[SDLK_RIGHT] - keys[SDLK_LEFT] + axes["mouseX"] * 1.5f) * dt, cameraRotation.up())
			* quat((keys[SDLK_LEFT] - keys[SDLK_RIGHT] + axes["mouseX"] * 0.5f) * dt, cameraRotation.forward())
			* quat((keys[SDLK_DOWN] - keys[SDLK_UP] + axes["mouseY"] * 1.5f) * dt, cameraRotation.right())
			* cameraRotation;

		cameraTransform = mat4::rotation(!cameraRotation) * mat4::translation(-cameraLocation);
		const mat4 viewMatrix = projectionMatrix * cameraTransform;

		//////////////////////////////////////////////////
		// Draw
		//////////////////////////////////////////////////
		
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	#if 0
		glBindFramebuffer(GL_FRAMEBUFFER, 0);

		const point2 hFboSize = fboSize;

		drawProg.bind();
		drawProg.setUniform<float32>("time", currTime);
		drawProg.setUniform<point2>("fboSize", hFboSize);
		drawProg.setUniform<float32>("samplingStep", 0.5f);
		drawProg.setUniform<const mat4&>("viewMatrix", viewMatrix);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_3D, volumeData);
		glBindImageTexture(0, colorBuffer, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_RGBA32F);
		glDispatchCompute(hFboSize.x / 32, hFboSize.y / 32, 1);

		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		// Test generation by blitting to viewport
		glBindFramebuffer(GL_READ_FRAMEBUFFER, drawFbo);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glReadBuffer(GL_COLOR_ATTACHMENT0);
		glBlitFramebuffer(0, 0, hFboSize.x, hFboSize.y, 0, 0, fboSize.x, fboSize.y, GL_COLOR_BUFFER_BIT, GL_LINEAR);

		glBindFramebuffer(GL_FRAMEBUFFER, 0);
	#else
		/// Generate density data
		genProg.bind();
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, perlinTables);
		glBindImageTexture(0, volumeData, 0, GL_FALSE, 0, GL_WRITE_ONLY, GL_R32F);
		glDispatchCompute(1, 33, 1);
		glMemoryBarrier(GL_SHADER_IMAGE_ACCESS_BARRIER_BIT);

		marchProg.bind();
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(uint32), &zeroReg);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, marchingTable);
		glBindImageTexture(0, volumeData, 0, GL_FALSE, 0, GL_READ_ONLY, GL_R32F);
		glDispatchCompute(1, 32, 1);
		glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

		drawProg.bind();
		drawProg.setUniform<const mat4&>("modelMatrix", mat4::eye(1.f));
		drawProg.setUniform<const mat4&>("viewMatrix", viewMatrix);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);

		uint32 * mappedMemory = (uint32*)glMapBufferRange(GL_ARRAY_BUFFER, 0, sizeof(uint32), GL_MAP_READ_BIT);
		printf("%u\n", *mappedMemory);
		glUnmapBuffer(GL_ARRAY_BUFFER);

		glDrawArrays(GL_TRIANGLES, 0, *mappedMemory);
	#endif

		// Consume mouse input
		axes["mouseX"] = 0.f;
		axes["mouseY"] = 0.f;

		SDL_GL_SwapWindow(window);
	}

	return 0;
}