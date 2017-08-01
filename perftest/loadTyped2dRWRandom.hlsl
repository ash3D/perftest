#define LOAD_WIDTH 2
#define LOAD_RANDOM
RWBuffer<float2> sourceData : register(u1);
#include "loadTypedBody.hlsli"