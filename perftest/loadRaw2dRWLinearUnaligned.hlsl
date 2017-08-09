#define LOAD_WIDTH 2
#define LOAD_LINEAR
#define READ_START_ADDRESS 4
RWByteAddressBuffer sourceData : register(u1);
#include "loadRawBody.hlsli"
