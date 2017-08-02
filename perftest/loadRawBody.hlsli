#include "hash.hlsli"
#include "loadConstantsGPU.h"

#ifndef LOOP_SCALEDOWN
#define LOOP_SCALEDOWN 2
#endif

#ifndef UNROLL
#define UNROLL 1
#endif

#ifndef ENABLE_READ_START_ADDRESS
#define ENABLE_READ_START_ADDRESS 0
#endif

#ifndef ADDRESS_MASK
#define ADDRESS_MASK 0
#endif

RWBuffer<float> output : register(u0);

cbuffer CB0 : register(b0)
{
	LoadConstants loadConstants;
};

#define THREAD_GROUP_SIZE 256

groupshared float dummyLDS[THREAD_GROUP_SIZE];

[numthreads(THREAD_GROUP_SIZE, 1, 1)]
void main(uint3 tid : SV_DispatchThreadID, uint gix : SV_GroupIndex)
{
	float4 value = 0.0;
	
#if defined(LOAD_INVARIANT)
    // All threads load from same address. Index is wave invariant.
	uint htid = 0;
#elif defined(LOAD_LINEAR)
	// Linearly increasing starting address to allow memory coalescing
	uint htid = gix;
#elif defined(LOAD_RANDOM)
    // Randomize start address offset (0-15) to prevent memory coalescing
	uint htid = hash1(gix) & 0xf;
#endif

	// Moved out all math from the inner loop
	htid *= 4 * LOAD_WIDTH;
#if ENABLE_READ_START_ADDRESS
	htid += loadConstants.readStartAddress;
#endif

#if UNROLL
	[unroll]
#else
	[loop]
#endif
	for (int i = 0; i < 256 / LOOP_SCALEDOWN; ++i)
	{
		// Mask with runtime constant to prevent unwanted compiler optimizations
		uint address = htid + i * (4 * LOAD_WIDTH);
#if ADDRESS_MASK == 1
		address |= loadConstants.elementsMask;
#elif ADDRESS_MASK == 2
		address &= ~loadConstants.elementsMask;
#endif

#if LOAD_WIDTH == 1
		value += sourceData.Load(address).xxxx;
#elif LOAD_WIDTH == 2
		value += sourceData.Load2(address).xyxy;
#elif LOAD_WIDTH == 3
		value += sourceData.Load3(address).xyzx; 
#elif LOAD_WIDTH == 4
		value += sourceData.Load4(address).xyzw;
#endif
	}

    // Linear write to LDS (no bank conflicts). Significantly faster than memory loads.
	dummyLDS[gix] = value.x + value.y + value.z + value.w;

	GroupMemoryBarrierWithGroupSync();

	// This branch is never taken, but the compiler doesn't know it
	// Optimizer would remove all the memory loads if the data wouldn't be potentially used
	[branch]
	if (loadConstants.writeIndex != 0xffffffff)
	{
        output[tid.x + tid.y] = dummyLDS[loadConstants.writeIndex];
    }
}
