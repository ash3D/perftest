#define LOAD_WIDTH 2
#define LOAD_LINEAR
RWByteAddressBuffer sourceData : register(u1);
#include "loadRawBody.hlsli"
