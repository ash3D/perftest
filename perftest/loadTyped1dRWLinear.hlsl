#define LOAD_WIDTH 1
#define LOAD_LINEAR
RWBuffer<float> sourceData : register(u1);
#include "loadTypedBody.hlsli"