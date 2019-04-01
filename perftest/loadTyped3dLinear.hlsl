#define LOAD_WIDTH 3
#define LOAD_LINEAR
Buffer<float3> sourceData : register(t0);
#include "loadTypedBody.hlsli"