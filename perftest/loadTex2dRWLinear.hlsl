#define LOAD_WIDTH 2
#define LOAD_LINEAR
RWTexture2D<float2> sourceData : register(u1);
#include "loadTexBody.hlsli"