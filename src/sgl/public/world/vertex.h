#include "core_types.h"
#include "coremin.h"
#include "math/math.h"

union Color {
	struct {
		uint8 r,g,b,a;
	};
	uint32 data;
};

struct Vertex {
	vec3 pos;
};