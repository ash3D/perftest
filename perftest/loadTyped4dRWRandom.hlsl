#define LOAD_WIDTH 4
#define LOAD_RANDOM
RWBuffer<float4> sourceData : register(u1);
#include "loadTypedBody.hlsli"
