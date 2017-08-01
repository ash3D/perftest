#define LOAD_WIDTH 1
#define LOAD_RANDOM
RWTexture2D<float> sourceData : register(u1);
#include "loadTexBody.hlsli"