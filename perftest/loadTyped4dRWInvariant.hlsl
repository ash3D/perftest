#define LOAD_WIDTH 4
#define LOAD_INVARIANT
RWBuffer<float4> sourceData : register(u1);
#include "loadTypedBody.hlsli"