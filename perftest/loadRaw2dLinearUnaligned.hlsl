#define LOAD_WIDTH 2
#define LOAD_LINEAR
#define READ_START_ADDRESS 4
ByteAddressBuffer sourceData : register(t0);
#include "loadRawBody.hlsli"