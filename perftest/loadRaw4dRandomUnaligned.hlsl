#define LOAD_WIDTH 4
#define LOAD_RANDOM
#define READ_START_ADDRESS 4
ByteAddressBuffer sourceData : register(t0);
#include "loadRawBody.hlsli"