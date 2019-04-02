#define LOAD_WIDTH 3
#define LOAD_RANDOM
Buffer<float3> sourceData : register(t0);
#include "loadTypedBody.hlsli"