#define LOAD_WIDTH 3
#define LOAD_LINEAR
Texture2D<float3> sourceData : register(t0);
#include "loadTexBody.hlsli"