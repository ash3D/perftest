#define LOAD_WIDTH 4
#define LOAD_RANDOM
#define READ_START_ADDRESS 4
RWByteAddressBuffer sourceData : register(u1);
#include "loadRawBody.hlsli"
