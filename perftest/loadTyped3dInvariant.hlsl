#define LOAD_WIDTH 3
#define LOAD_INVARIANT
Buffer<float3> sourceData : register(t0);
#include "loadTypedBody.hlsli"