#define LOAD_WIDTH 3
#define LOAD_LINEAR
RWByteAddressBuffer sourceData : register(u1);
#include "loadRawBody.hlsli"
