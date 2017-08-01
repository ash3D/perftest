#define LOAD_WIDTH 4
#define LOAD_INVARIANT
RWTexture2D<float4> sourceData : register(u1);
#include "loadTexBody.hlsli"