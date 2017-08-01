#include "window.h"
#include "directx.h"
#include "graphicsUtil.h"
#include "loadConstantsGPU.h"

// D3D11.3/12
#define ADDITIONAL_TYPED_UAV_FORMATS 0

void benchTest(DirectXDevice& dx, const std::function<void(DirectXDevice&, uint3, uint3)>& dispatch, const std::string& name)
{
	const uint3 workloadThreadCount(1024, 1024, 1);
	const uint3 workloadGroupSize(256, 1, 1);

	QueryHandle query = dx.startPerformanceQuery(name);
	dispatch(dx, workloadThreadCount, workloadGroupSize);
	dx.endPerformanceQuery(query);
}

void benchTest(DirectXDevice& dx, ID3D11ComputeShader* shader, ID3D11Buffer* cb, ID3D11UnorderedAccessView* output, ID3D11ShaderResourceView* source, const std::string& name)
{
	using namespace std::placeholders;
	const auto cbs = { cb };
	const auto srvs = { source };
	const auto uavs = { output };
	benchTest(dx, std::bind(&DirectXDevice::dispatch, _1, shader, _2, _3, cbs, srvs, uavs, std::initializer_list<ID3D11SamplerState*>{}), name);
}

void benchTest(DirectXDevice& dx, ID3D11ComputeShader* shader, ID3D11Buffer* cb, ID3D11UnorderedAccessView* output, ID3D11UnorderedAccessView* source, const std::string& name)
{
	using namespace std::placeholders;
	const auto cbs = { cb };
	const auto uavs = { output, source };
	benchTest(dx, std::bind(&DirectXDevice::dispatch, _1, shader, _2, _3, cbs, std::initializer_list<ID3D11ShaderResourceView*>{}, uavs, std::initializer_list<ID3D11SamplerState*>{}), name);
}

int main(int argc, char *argv[])
{
	// Enumerate adapters
	std::vector<com_ptr<IDXGIAdapter>> adapters = enumerateAdapters();
	printf("PerfTest\nTo select adapter, use: PerfTest.exe [ADAPTER_INDEX]\n\n");
	printf("Adapters found:\n");
	int index = 0;
	for (auto&& adapter : adapters)
	{
		DXGI_ADAPTER_DESC desc;
		adapter->GetDesc(&desc);
		printf("%d: %S\n", index++, desc.Description);
	}

	// Command line index can be used to select adapter
	int selectedAdapterIdx = 0;
	if (argc == 2)
	{
		selectedAdapterIdx = std::stoi(argv[1]);
		selectedAdapterIdx = min(max(0, selectedAdapterIdx), (int)adapters.size() - 1);
	}
	printf("Using adapter %d\n", selectedAdapterIdx);

	// Init systems
	uint2 resolution(256, 256);
	HWND window = createWindow(resolution);
	DirectXDevice dx(window, resolution, adapters[selectedAdapterIdx]);

	// Load shaders 
	com_ptr<ID3D11ComputeShader> shaderLoadTyped1dInvariant = loadComputeShader(dx, "shaders/loadTyped1dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped1dLinear = loadComputeShader(dx, "shaders/loadTyped1dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped1dRandom = loadComputeShader(dx, "shaders/loadTyped1dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped2dInvariant = loadComputeShader(dx, "shaders/loadTyped2dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped2dLinear = loadComputeShader(dx, "shaders/loadTyped2dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped2dRandom = loadComputeShader(dx, "shaders/loadTyped2dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped4dInvariant = loadComputeShader(dx, "shaders/loadTyped4dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped4dLinear = loadComputeShader(dx, "shaders/loadTyped4dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped4dRandom = loadComputeShader(dx, "shaders/loadTyped4dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped1dRWInvariant = loadComputeShader(dx, "shaders/loadTyped1dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped1dRWLinear = loadComputeShader(dx, "shaders/loadTyped1dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped1dRWRandom = loadComputeShader(dx, "shaders/loadTyped1dRWRandom.cso");
#if ADDITIONAL_TYPED_UAV_FORMATS
	com_ptr<ID3D11ComputeShader> shaderLoadTyped2dRWInvariant = loadComputeShader(dx, "shaders/loadTyped2dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped2dRWLinear = loadComputeShader(dx, "shaders/loadTyped2dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped2dRWRandom = loadComputeShader(dx, "shaders/loadTyped2dRWRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped4dRWInvariant = loadComputeShader(dx, "shaders/loadTyped4dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped4dRWLinear = loadComputeShader(dx, "shaders/loadTyped4dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTyped4dRWRandom = loadComputeShader(dx, "shaders/loadTyped4dRWRandom.cso");
#endif
	com_ptr<ID3D11ComputeShader> shaderLoadRaw1dInvariant = loadComputeShader(dx, "shaders/loadRaw1dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw1dLinear = loadComputeShader(dx, "shaders/loadRaw1dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw1dRandom = loadComputeShader(dx, "shaders/loadRaw1dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw2dInvariant = loadComputeShader(dx, "shaders/loadRaw2dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw2dLinear = loadComputeShader(dx, "shaders/loadRaw2dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw2dRandom = loadComputeShader(dx, "shaders/loadRaw2dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw3dInvariant = loadComputeShader(dx, "shaders/loadRaw3dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw3dLinear = loadComputeShader(dx, "shaders/loadRaw3dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw3dRandom = loadComputeShader(dx, "shaders/loadRaw3dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw4dInvariant = loadComputeShader(dx, "shaders/loadRaw4dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw4dLinear = loadComputeShader(dx, "shaders/loadRaw4dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw4dRandom = loadComputeShader(dx, "shaders/loadRaw4dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw1dRWInvariant = loadComputeShader(dx, "shaders/loadRaw1dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw1dRWLinear = loadComputeShader(dx, "shaders/loadRaw1dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw1dRWRandom = loadComputeShader(dx, "shaders/loadRaw1dRWRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw2dRWInvariant = loadComputeShader(dx, "shaders/loadRaw2dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw2dRWLinear = loadComputeShader(dx, "shaders/loadRaw2dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw2dRWRandom = loadComputeShader(dx, "shaders/loadRaw2dRWRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw3dRWInvariant = loadComputeShader(dx, "shaders/loadRaw3dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw3dRWLinear = loadComputeShader(dx, "shaders/loadRaw3dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw3dRWRandom = loadComputeShader(dx, "shaders/loadRaw3dRWRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw4dRWInvariant = loadComputeShader(dx, "shaders/loadRaw4dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw4dRWLinear = loadComputeShader(dx, "shaders/loadRaw4dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadRaw4dRWRandom = loadComputeShader(dx, "shaders/loadRaw4dRWRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex1dInvariant = loadComputeShader(dx, "shaders/loadTex1dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex1dLinear = loadComputeShader(dx, "shaders/loadTex1dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex1dRandom = loadComputeShader(dx, "shaders/loadTex1dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex2dInvariant = loadComputeShader(dx, "shaders/loadTex2dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex2dLinear = loadComputeShader(dx, "shaders/loadTex2dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex2dRandom = loadComputeShader(dx, "shaders/loadTex2dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex4dInvariant = loadComputeShader(dx, "shaders/loadTex4dInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex4dLinear = loadComputeShader(dx, "shaders/loadTex4dLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex4dRandom = loadComputeShader(dx, "shaders/loadTex4dRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex1dRWInvariant = loadComputeShader(dx, "shaders/loadTex1dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex1dRWLinear = loadComputeShader(dx, "shaders/loadTex1dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex1dRWRandom = loadComputeShader(dx, "shaders/loadTex1dRWRandom.cso");
#if ADDITIONAL_TYPED_UAV_FORMATS
	com_ptr<ID3D11ComputeShader> shaderLoadTex2dRWInvariant = loadComputeShader(dx, "shaders/loadTex2dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex2dRWLinear = loadComputeShader(dx, "shaders/loadTex2dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex2dRWRandom = loadComputeShader(dx, "shaders/loadTex2dRWRandom.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex4dRWInvariant = loadComputeShader(dx, "shaders/loadTex4dRWInvariant.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex4dRWLinear = loadComputeShader(dx, "shaders/loadTex4dRWLinear.cso");
	com_ptr<ID3D11ComputeShader> shaderLoadTex4dRWRandom = loadComputeShader(dx, "shaders/loadTex4dRWRandom.cso");
#endif

	// Create buffers and output UAV
	com_ptr<ID3D11Buffer> bufferOutput = dx.createBuffer(1024, 32, DirectXDevice::BufferType::ByteAddress);
	com_ptr<ID3D11Buffer> bufferInput = dx.createBuffer(1024, 32, DirectXDevice::BufferType::ByteAddress);
	com_ptr<ID3D11UnorderedAccessView> outputUAV = dx.createTypedUAV(bufferOutput, 1024 * 8, DXGI_FORMAT_R32_FLOAT);

	// SRVs for benchmarking different buffer view formats/types
	com_ptr<ID3D11ShaderResourceView> typedSRV_R8 = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R8_UNORM);
	com_ptr<ID3D11ShaderResourceView> typedSRV_R16F = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R16_FLOAT);
	com_ptr<ID3D11ShaderResourceView> typedSRV_R32F = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R32_FLOAT);
	com_ptr<ID3D11ShaderResourceView> typedSRV_RG8 = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R8G8_UNORM);
	com_ptr<ID3D11ShaderResourceView> typedSRV_RG16F = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R16G16_FLOAT);
	com_ptr<ID3D11ShaderResourceView> typedSRV_RG32F = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R32G32_FLOAT);
	com_ptr<ID3D11ShaderResourceView> typedSRV_RGBA8 = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R8G8B8A8_UNORM);
	com_ptr<ID3D11ShaderResourceView> typedSRV_RGBA16F = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	com_ptr<ID3D11ShaderResourceView> typedSRV_RGBA32F = dx.createTypedSRV(bufferInput, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT);
	com_ptr<ID3D11ShaderResourceView> byteAddressSRV = dx.createByteAddressSRV(bufferInput, 1024);

	// UAVs for benchmarking different buffer view formats/types
#if ADDITIONAL_TYPED_UAV_FORMATS
	com_ptr<ID3D11UnorderedAccessView> typedUAV_R8 = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R8_UNORM);
	com_ptr<ID3D11UnorderedAccessView> typedUAV_R16F = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R16_FLOAT);
#endif
	com_ptr<ID3D11UnorderedAccessView> typedUAV_R32F = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R32_FLOAT);
#if ADDITIONAL_TYPED_UAV_FORMATS
	com_ptr<ID3D11UnorderedAccessView> typedUAV_RG8 = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R8G8_UNORM);
	com_ptr<ID3D11UnorderedAccessView> typedUAV_RG16F = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R16G16_FLOAT);
	com_ptr<ID3D11UnorderedAccessView> typedUAV_RG32F = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R32G32_FLOAT);
	com_ptr<ID3D11UnorderedAccessView> typedUAV_RGBA8 = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R8G8B8A8_UNORM);
	com_ptr<ID3D11UnorderedAccessView> typedUAV_RGBA16F = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R16G16B16A16_FLOAT);
	com_ptr<ID3D11UnorderedAccessView> typedUAV_RGBA32F = dx.createTypedUAV(bufferInput, 1024, DXGI_FORMAT_R32G32B32A32_FLOAT);
#endif
	com_ptr<ID3D11UnorderedAccessView> byteAddressUAV = dx.createByteAddressUAV(bufferInput, 1024);

	// Create input textures
	com_ptr<ID3D11Texture2D> texR8 = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R8_UNORM, 1);
	com_ptr<ID3D11Texture2D> texR16F = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R16_FLOAT, 1);
	com_ptr<ID3D11Texture2D> texR32F = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R32_FLOAT, 1);
	com_ptr<ID3D11Texture2D> texRG8 = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R8G8_UNORM, 1);
	com_ptr<ID3D11Texture2D> texRG16F = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R16G16_FLOAT, 1);
	com_ptr<ID3D11Texture2D> texRG32F = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R32G32_FLOAT, 1);
	com_ptr<ID3D11Texture2D> texRGBA8 = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R8G8B8A8_UNORM, 1);
	com_ptr<ID3D11Texture2D> texRGBA16F = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R16G16B16A16_FLOAT, 1);
	com_ptr<ID3D11Texture2D> texRGBA32F = dx.createTexture2d(uint2(32, 32), DXGI_FORMAT_R32G32B32A32_FLOAT, 1);

	// Texture SRVs
	com_ptr<ID3D11ShaderResourceView> texSRV_R8 = dx.createSRV(texR8);
	com_ptr<ID3D11ShaderResourceView> texSRV_R16F = dx.createSRV(texR16F);
	com_ptr<ID3D11ShaderResourceView> texSRV_R32F = dx.createSRV(texR32F);
	com_ptr<ID3D11ShaderResourceView> texSRV_RG8 = dx.createSRV(texRG8);
	com_ptr<ID3D11ShaderResourceView> texSRV_RG16F = dx.createSRV(texRG16F);
	com_ptr<ID3D11ShaderResourceView> texSRV_RG32F = dx.createSRV(texRG32F);
	com_ptr<ID3D11ShaderResourceView> texSRV_RGBA8 = dx.createSRV(texRGBA8);
	com_ptr<ID3D11ShaderResourceView> texSRV_RGBA16F = dx.createSRV(texRGBA16F);
	com_ptr<ID3D11ShaderResourceView> texSRV_RGBA32F = dx.createSRV(texRGBA32F);

	// Texture UAVs
#if ADDITIONAL_TYPED_UAV_FORMATS
	com_ptr<ID3D11UnorderedAccessView> texUAV_R8 = dx.createUAV(texR8);
	com_ptr<ID3D11UnorderedAccessView> texUAV_R16F = dx.createUAV(texR16F);
#endif
	com_ptr<ID3D11UnorderedAccessView> texUAV_R32F = dx.createUAV(texR32F);
#if ADDITIONAL_TYPED_UAV_FORMATS
	com_ptr<ID3D11UnorderedAccessView> texUAV_RG8 = dx.createUAV(texRG8);
	com_ptr<ID3D11UnorderedAccessView> texUAV_RG16F = dx.createUAV(texRG16F);
	com_ptr<ID3D11UnorderedAccessView> texUAV_RG32F = dx.createUAV(texRG32F);
	com_ptr<ID3D11UnorderedAccessView> texUAV_RGBA8 = dx.createUAV(texRGBA8);
	com_ptr<ID3D11UnorderedAccessView> texUAV_RGBA16F = dx.createUAV(texRGBA16F);
	com_ptr<ID3D11UnorderedAccessView> texUAV_RGBA32F = dx.createUAV(texRGBA32F);
#endif

	// Setup the constant buffer
	LoadConstants loadConstants;
	com_ptr<ID3D11Buffer> loadCB = dx.createConstantBuffer(sizeof(LoadConstants));
	com_ptr<ID3D11Buffer> loadCBUnaligned = dx.createConstantBuffer(sizeof(LoadConstants));
	loadConstants.elementsMask = 0;				// Dummy mask to prevent unwanted compiler optimizations
	loadConstants.writeIndex = 0xffffffff;		// Never write
	loadConstants.readStartAddress = 0;			// Aligned
	dx.updateConstantBuffer(loadCB, loadConstants);
	loadConstants.readStartAddress = 4;			// Unaligned
	dx.updateConstantBuffer(loadCBUnaligned, loadConstants);

	// Frame loop
	MessageStatus status = MessageStatus::Default;
	do
	{
		dx.processPerformanceResults([](float timeMillis, std::string& name)
		{
			printf("%s: %.3fms\n", name.c_str(), timeMillis);
		});

		benchTest(dx, shaderLoadTyped1dInvariant, loadCB, outputUAV, typedSRV_R8, "Load R8 SRV invariant");
		benchTest(dx, shaderLoadTyped1dLinear, loadCB, outputUAV, typedSRV_R8, "Load R8 SRV linear");
		benchTest(dx, shaderLoadTyped1dRandom, loadCB, outputUAV, typedSRV_R8, "Load R8 SRV random");
		benchTest(dx, shaderLoadTyped2dInvariant, loadCB, outputUAV, typedSRV_RG8, "Load RG8 SRV invariant");
		benchTest(dx, shaderLoadTyped2dLinear, loadCB, outputUAV, typedSRV_RG8, "Load RG8 SRV linear");
		benchTest(dx, shaderLoadTyped2dRandom, loadCB, outputUAV, typedSRV_RG8, "Load RG8 SRV random");
		benchTest(dx, shaderLoadTyped4dInvariant, loadCB, outputUAV, typedSRV_RGBA8, "Load RGBA8 SRV invariant");
		benchTest(dx, shaderLoadTyped4dLinear, loadCB, outputUAV, typedSRV_RGBA8, "Load RGBA8 SRV linear");
		benchTest(dx, shaderLoadTyped4dRandom, loadCB, outputUAV, typedSRV_RGBA8, "Load RGBA8 SRV random");

		benchTest(dx, shaderLoadTyped1dInvariant, loadCB, outputUAV, typedSRV_R16F, "Load R16f SRV invariant");
		benchTest(dx, shaderLoadTyped1dLinear, loadCB, outputUAV, typedSRV_R16F, "Load R16f SRV linear");
		benchTest(dx, shaderLoadTyped1dRandom, loadCB, outputUAV, typedSRV_R16F, "Load R16f SRV random");
		benchTest(dx, shaderLoadTyped2dInvariant, loadCB, outputUAV, typedSRV_RG16F, "Load RG16f SRV invariant");
		benchTest(dx, shaderLoadTyped2dLinear, loadCB, outputUAV, typedSRV_RG16F, "Load RG16f SRV linear");
		benchTest(dx, shaderLoadTyped2dRandom, loadCB, outputUAV, typedSRV_RG16F, "Load RG16f SRV random");
		benchTest(dx, shaderLoadTyped4dInvariant, loadCB, outputUAV, typedSRV_RGBA16F, "Load RGBA16f SRV invariant");
		benchTest(dx, shaderLoadTyped4dLinear, loadCB, outputUAV, typedSRV_RGBA16F, "Load RGBA16f SRV linear");
		benchTest(dx, shaderLoadTyped4dRandom, loadCB, outputUAV, typedSRV_RGBA16F, "Load RGBA16f SRV random");

		benchTest(dx, shaderLoadTyped1dInvariant, loadCB, outputUAV, typedSRV_R32F, "Load R32f SRV invariant");
		benchTest(dx, shaderLoadTyped1dLinear, loadCB, outputUAV, typedSRV_R32F, "Load R32f SRV linear");
		benchTest(dx, shaderLoadTyped1dRandom, loadCB, outputUAV, typedSRV_R32F, "Load R32f SRV random");
		benchTest(dx, shaderLoadTyped2dInvariant, loadCB, outputUAV, typedSRV_RG32F, "Load RG32f SRV invariant");
		benchTest(dx, shaderLoadTyped2dLinear, loadCB, outputUAV, typedSRV_RG32F, "Load RG32f SRV linear");
		benchTest(dx, shaderLoadTyped2dRandom, loadCB, outputUAV, typedSRV_RG32F, "Load RG32f SRV random");
		benchTest(dx, shaderLoadTyped4dInvariant, loadCB, outputUAV, typedSRV_RGBA32F, "Load RGBA32f SRV invariant");
		benchTest(dx, shaderLoadTyped4dLinear, loadCB, outputUAV, typedSRV_RGBA32F, "Load RGBA32f SRV linear");
		benchTest(dx, shaderLoadTyped4dRandom, loadCB, outputUAV, typedSRV_RGBA32F, "Load RGBA32f SRV random");

#if ADDITIONAL_TYPED_UAV_FORMATS
		benchTest(dx, shaderLoadTyped1dRWInvariant, loadCB, outputUAV, typedUAV_R8, "Load R8 UAV invariant");
		benchTest(dx, shaderLoadTyped1dRWLinear, loadCB, outputUAV, typedUAV_R8, "Load R8 UAV linear");
		benchTest(dx, shaderLoadTyped1dRWRandom, loadCB, outputUAV, typedUAV_R8, "Load R8 UAV random");
		benchTest(dx, shaderLoadTyped2dRWInvariant, loadCB, outputUAV, typedUAV_RG8, "Load RG8 UAV invariant");
		benchTest(dx, shaderLoadTyped2dRWLinear, loadCB, outputUAV, typedUAV_RG8, "Load RG8 UAV linear");
		benchTest(dx, shaderLoadTyped2dRWRandom, loadCB, outputUAV, typedUAV_RG8, "Load RG8 UAV random");
		benchTest(dx, shaderLoadTyped4dRWInvariant, loadCB, outputUAV, typedUAV_RGBA8, "Load RGBA8 UAV invariant");
		benchTest(dx, shaderLoadTyped4dRWLinear, loadCB, outputUAV, typedUAV_RGBA8, "Load RGBA8 UAV linear");
		benchTest(dx, shaderLoadTyped4dRWRandom, loadCB, outputUAV, typedUAV_RGBA8, "Load RGBA8 UAV random");

		benchTest(dx, shaderLoadTyped1dRWInvariant, loadCB, outputUAV, typedUAV_R16F, "Load R16f UAV invariant");
		benchTest(dx, shaderLoadTyped1dRWLinear, loadCB, outputUAV, typedUAV_R16F, "Load R16f UAV linear");
		benchTest(dx, shaderLoadTyped1dRWRandom, loadCB, outputUAV, typedUAV_R16F, "Load R16f UAV random");
		benchTest(dx, shaderLoadTyped2dRWInvariant, loadCB, outputUAV, typedUAV_RG16F, "Load RG16f UAV invariant");
		benchTest(dx, shaderLoadTyped2dRWLinear, loadCB, outputUAV, typedUAV_RG16F, "Load RG16f UAV linear");
		benchTest(dx, shaderLoadTyped2dRWRandom, loadCB, outputUAV, typedUAV_RG16F, "Load RG16f UAV random");
		benchTest(dx, shaderLoadTyped4dRWInvariant, loadCB, outputUAV, typedUAV_RGBA16F, "Load RGBA16f UAV invariant");
		benchTest(dx, shaderLoadTyped4dRWLinear, loadCB, outputUAV, typedUAV_RGBA16F, "Load RGBA16f UAV linear");
		benchTest(dx, shaderLoadTyped4dRWRandom, loadCB, outputUAV, typedUAV_RGBA16F, "Load RGBA16f UAV random");
#endif

		benchTest(dx, shaderLoadTyped1dRWInvariant, loadCB, outputUAV, typedUAV_R32F, "Load R32f UAV invariant");
		benchTest(dx, shaderLoadTyped1dRWLinear, loadCB, outputUAV, typedUAV_R32F, "Load R32f UAV linear");
		benchTest(dx, shaderLoadTyped1dRWRandom, loadCB, outputUAV, typedUAV_R32F, "Load R32f UAV random");
#if ADDITIONAL_TYPED_UAV_FORMATS
		benchTest(dx, shaderLoadTyped2dRWInvariant, loadCB, outputUAV, typedUAV_RG32F, "Load RG32f UAV invariant");
		benchTest(dx, shaderLoadTyped2dRWLinear, loadCB, outputUAV, typedUAV_RG32F, "Load RG32f UAV linear");
		benchTest(dx, shaderLoadTyped2dRWRandom, loadCB, outputUAV, typedUAV_RG32F, "Load RG32f UAV random");
		benchTest(dx, shaderLoadTyped4dRWInvariant, loadCB, outputUAV, typedUAV_RGBA32F, "Load RGBA32f UAV invariant");
		benchTest(dx, shaderLoadTyped4dRWLinear, loadCB, outputUAV, typedUAV_RGBA32F, "Load RGBA32f UAV linear");
		benchTest(dx, shaderLoadTyped4dRWRandom, loadCB, outputUAV, typedUAV_RGBA32F, "Load RGBA32f UAV random");
#endif

		benchTest(dx, shaderLoadRaw1dInvariant, loadCB, outputUAV, byteAddressSRV, "Load1 raw32 SRV invariant");
		benchTest(dx, shaderLoadRaw1dLinear, loadCB, outputUAV, byteAddressSRV, "Load1 raw32 SRV linear");
		benchTest(dx, shaderLoadRaw1dRandom, loadCB, outputUAV, byteAddressSRV, "Load1 raw32 SRV random");
		benchTest(dx, shaderLoadRaw2dInvariant, loadCB, outputUAV, byteAddressSRV, "Load2 raw32 SRV invariant");
		benchTest(dx, shaderLoadRaw2dLinear, loadCB, outputUAV, byteAddressSRV, "Load2 raw32 SRV linear");
		benchTest(dx, shaderLoadRaw2dRandom, loadCB, outputUAV, byteAddressSRV, "Load2 raw32 SRV random");
		benchTest(dx, shaderLoadRaw3dInvariant, loadCB, outputUAV, byteAddressSRV, "Load3 raw32 SRV invariant");
		benchTest(dx, shaderLoadRaw3dLinear, loadCB, outputUAV, byteAddressSRV, "Load3 raw32 SRV linear");
		benchTest(dx, shaderLoadRaw3dRandom, loadCB, outputUAV, byteAddressSRV, "Load3 raw32 SRV random");
		benchTest(dx, shaderLoadRaw4dInvariant, loadCB, outputUAV, byteAddressSRV, "Load4 raw32 SRV invariant");
		benchTest(dx, shaderLoadRaw4dLinear, loadCB, outputUAV, byteAddressSRV, "Load4 raw32 SRV linear");
		benchTest(dx, shaderLoadRaw4dRandom, loadCB, outputUAV, byteAddressSRV, "Load4 raw32 SRV random");

		benchTest(dx, shaderLoadRaw2dInvariant, loadCBUnaligned, outputUAV, byteAddressSRV, "Load2 raw32 SRV unaligned invariant");
		benchTest(dx, shaderLoadRaw2dLinear, loadCBUnaligned, outputUAV, byteAddressSRV, "Load2 raw32 SRV unaligned linear");
		benchTest(dx, shaderLoadRaw2dRandom, loadCBUnaligned, outputUAV, byteAddressSRV, "Load2 raw32 SRV unaligned random");
		benchTest(dx, shaderLoadRaw4dInvariant, loadCBUnaligned, outputUAV, byteAddressSRV, "Load4 raw32 SRV unaligned invariant");
		benchTest(dx, shaderLoadRaw4dLinear, loadCBUnaligned, outputUAV, byteAddressSRV, "Load4 raw32 SRV unaligned linear");
		benchTest(dx, shaderLoadRaw4dRandom, loadCBUnaligned, outputUAV, byteAddressSRV, "Load4 raw32 SRV unaligned random");

		benchTest(dx, shaderLoadRaw1dRWInvariant, loadCB, outputUAV, byteAddressUAV, "Load1 raw32 UAV invariant");
		benchTest(dx, shaderLoadRaw1dRWLinear, loadCB, outputUAV, byteAddressUAV, "Load1 raw32 UAV linear");
		benchTest(dx, shaderLoadRaw1dRWRandom, loadCB, outputUAV, byteAddressUAV, "Load1 raw32 UAV random");
		benchTest(dx, shaderLoadRaw2dRWInvariant, loadCB, outputUAV, byteAddressUAV, "Load2 raw32 UAV invariant");
		benchTest(dx, shaderLoadRaw2dRWLinear, loadCB, outputUAV, byteAddressUAV, "Load2 raw32 UAV linear");
		benchTest(dx, shaderLoadRaw2dRWRandom, loadCB, outputUAV, byteAddressUAV, "Load2 raw32 UAV random");
		benchTest(dx, shaderLoadRaw3dRWInvariant, loadCB, outputUAV, byteAddressUAV, "Load3 raw32 UAV invariant");
		benchTest(dx, shaderLoadRaw3dRWLinear, loadCB, outputUAV, byteAddressUAV, "Load3 raw32 UAV linear");
		benchTest(dx, shaderLoadRaw3dRWRandom, loadCB, outputUAV, byteAddressUAV, "Load3 raw32 UAV random");
		benchTest(dx, shaderLoadRaw4dRWInvariant, loadCB, outputUAV, byteAddressUAV, "Load4 raw32 UAV invariant");
		benchTest(dx, shaderLoadRaw4dRWLinear, loadCB, outputUAV, byteAddressUAV, "Load4 raw32 UAV linear");
		benchTest(dx, shaderLoadRaw4dRWRandom, loadCB, outputUAV, byteAddressUAV, "Load4 raw32 UAV random");

		benchTest(dx, shaderLoadRaw2dRWInvariant, loadCBUnaligned, outputUAV, byteAddressUAV, "Load2 raw32 UAV unaligned invariant");
		benchTest(dx, shaderLoadRaw2dRWLinear, loadCBUnaligned, outputUAV, byteAddressUAV, "Load2 raw32 UAV unaligned linear");
		benchTest(dx, shaderLoadRaw2dRWRandom, loadCBUnaligned, outputUAV, byteAddressUAV, "Load2 raw32 UAV unaligned random");
		benchTest(dx, shaderLoadRaw4dRWInvariant, loadCBUnaligned, outputUAV, byteAddressUAV, "Load4 raw32 UAV unaligned invariant");
		benchTest(dx, shaderLoadRaw4dRWLinear, loadCBUnaligned, outputUAV, byteAddressUAV, "Load4 raw32 UAV unaligned linear");
		benchTest(dx, shaderLoadRaw4dRWRandom, loadCBUnaligned, outputUAV, byteAddressUAV, "Load4 raw32 UAV unaligned random");

		benchTest(dx, shaderLoadTex1dInvariant, loadCB, outputUAV, texSRV_R8, "Tex2D load R8 SRV invariant");
		benchTest(dx, shaderLoadTex1dLinear, loadCB, outputUAV, texSRV_R8, "Tex2D load R8 SRV linear");
		benchTest(dx, shaderLoadTex1dRandom, loadCB, outputUAV, texSRV_R8, "Tex2D load R8 SRV random");
		benchTest(dx, shaderLoadTex2dInvariant, loadCB, outputUAV, texSRV_RG8, "Tex2D load RG8 SRV invariant");
		benchTest(dx, shaderLoadTex2dLinear, loadCB, outputUAV, texSRV_RG8, "Tex2D load RG8 SRV linear");
		benchTest(dx, shaderLoadTex2dRandom, loadCB, outputUAV, texSRV_RG8, "Tex2D load RG8 SRV random");
		benchTest(dx, shaderLoadTex4dInvariant, loadCB, outputUAV, texSRV_RGBA8, "Tex2D load RGBA8 SRV invariant");
		benchTest(dx, shaderLoadTex4dLinear, loadCB, outputUAV, texSRV_RGBA8, "Tex2D load RGBA8 SRV linear");
		benchTest(dx, shaderLoadTex4dRandom, loadCB, outputUAV, texSRV_RGBA8, "Tex2D load RGBA8 SRV random");

		benchTest(dx, shaderLoadTex1dInvariant, loadCB, outputUAV, texSRV_R16F, "Tex2D load R16F SRV invariant");
		benchTest(dx, shaderLoadTex1dLinear, loadCB, outputUAV, texSRV_R16F, "Tex2D load R16F SRV linear");
		benchTest(dx, shaderLoadTex1dRandom, loadCB, outputUAV, texSRV_R16F, "Tex2D load R16F SRV random");
		benchTest(dx, shaderLoadTex2dInvariant, loadCB, outputUAV, texSRV_RG16F, "Tex2D load RG16F SRV invariant");
		benchTest(dx, shaderLoadTex2dLinear, loadCB, outputUAV, texSRV_RG16F, "Tex2D load RG16F SRV linear");
		benchTest(dx, shaderLoadTex2dRandom, loadCB, outputUAV, texSRV_RG16F, "Tex2D load RG16F SRV random");
		benchTest(dx, shaderLoadTex4dInvariant, loadCB, outputUAV, texSRV_RGBA16F, "Tex2D load RGBA16F SRV invariant");
		benchTest(dx, shaderLoadTex4dLinear, loadCB, outputUAV, texSRV_RGBA16F, "Tex2D load RGBA16F SRV linear");
		benchTest(dx, shaderLoadTex4dRandom, loadCB, outputUAV, texSRV_RGBA16F, "Tex2D load RGBA16F SRV random");

		benchTest(dx, shaderLoadTex1dInvariant, loadCB, outputUAV, texSRV_R32F, "Tex2D load R32F SRV invariant");
		benchTest(dx, shaderLoadTex1dLinear, loadCB, outputUAV, texSRV_R32F, "Tex2D load R32F SRV linear");
		benchTest(dx, shaderLoadTex1dRandom, loadCB, outputUAV, texSRV_R32F, "Tex2D load R32F SRV random");
		benchTest(dx, shaderLoadTex2dInvariant, loadCB, outputUAV, texSRV_RG32F, "Tex2D load RG32F SRV invariant");
		benchTest(dx, shaderLoadTex2dLinear, loadCB, outputUAV, texSRV_RG32F, "Tex2D load RG32F SRV linear");
		benchTest(dx, shaderLoadTex2dRandom, loadCB, outputUAV, texSRV_RG32F, "Tex2D load RG32F SRV random");
		benchTest(dx, shaderLoadTex4dInvariant, loadCB, outputUAV, texSRV_RGBA32F, "Tex2D load RGBA32F SRV invariant");
		benchTest(dx, shaderLoadTex4dLinear, loadCB, outputUAV, texSRV_RGBA32F, "Tex2D load RGBA32F SRV linear");
		benchTest(dx, shaderLoadTex4dRandom, loadCB, outputUAV, texSRV_RGBA32F, "Tex2D load RGBA32F SRV random");

#if ADDITIONAL_TYPED_UAV_FORMATS
		benchTest(dx, shaderLoadTex1dRWInvariant, loadCB, outputUAV, texUAV_R8, "Tex2D load R8 UAV invariant");
		benchTest(dx, shaderLoadTex1dRWLinear, loadCB, outputUAV, texUAV_R8, "Tex2D load R8 UAV linear");
		benchTest(dx, shaderLoadTex1dRWRandom, loadCB, outputUAV, texUAV_R8, "Tex2D load R8 UAV random");
		benchTest(dx, shaderLoadTex2dRWInvariant, loadCB, outputUAV, texUAV_RG8, "Tex2D load RG8 UAV invariant");
		benchTest(dx, shaderLoadTex2dRWLinear, loadCB, outputUAV, texUAV_RG8, "Tex2D load RG8 UAV linear");
		benchTest(dx, shaderLoadTex2dRWRandom, loadCB, outputUAV, texUAV_RG8, "Tex2D load RG8 UAV random");
		benchTest(dx, shaderLoadTex4dRWInvariant, loadCB, outputUAV, texUAV_RGBA8, "Tex2D load RGBA8 UAV invariant");
		benchTest(dx, shaderLoadTex4dRWLinear, loadCB, outputUAV, texUAV_RGBA8, "Tex2D load RGBA8 UAV linear");
		benchTest(dx, shaderLoadTex4dRWRandom, loadCB, outputUAV, texUAV_RGBA8, "Tex2D load RGBA8 UAV random");

		benchTest(dx, shaderLoadTex1dRWInvariant, loadCB, outputUAV, texUAV_R16F, "Tex2D load R16F UAV invariant");
		benchTest(dx, shaderLoadTex1dRWLinear, loadCB, outputUAV, texUAV_R16F, "Tex2D load R16F UAV linear");
		benchTest(dx, shaderLoadTex1dRWRandom, loadCB, outputUAV, texUAV_R16F, "Tex2D load R16F UAV random");
		benchTest(dx, shaderLoadTex2dRWInvariant, loadCB, outputUAV, texUAV_RG16F, "Tex2D load RG16F UAV invariant");
		benchTest(dx, shaderLoadTex2dRWLinear, loadCB, outputUAV, texUAV_RG16F, "Tex2D load RG16F UAV linear");
		benchTest(dx, shaderLoadTex2dRWRandom, loadCB, outputUAV, texUAV_RG16F, "Tex2D load RG16F UAV random");
		benchTest(dx, shaderLoadTex4dRWInvariant, loadCB, outputUAV, texUAV_RGBA16F, "Tex2D load RGBA16F UAV invariant");
		benchTest(dx, shaderLoadTex4dRWLinear, loadCB, outputUAV, texUAV_RGBA16F, "Tex2D load RGBA16F UAV linear");
		benchTest(dx, shaderLoadTex4dRWRandom, loadCB, outputUAV, texUAV_RGBA16F, "Tex2D load RGBA16F UAV random");
#endif

		benchTest(dx, shaderLoadTex1dRWInvariant, loadCB, outputUAV, texUAV_R32F, "Tex2D load R32F UAV invariant");
		benchTest(dx, shaderLoadTex1dRWLinear, loadCB, outputUAV, texUAV_R32F, "Tex2D load R32F UAV linear");
		benchTest(dx, shaderLoadTex1dRWRandom, loadCB, outputUAV, texUAV_R32F, "Tex2D load R32F UAV random");
#if ADDITIONAL_TYPED_UAV_FORMATS
		benchTest(dx, shaderLoadTex2dRWInvariant, loadCB, outputUAV, texUAV_RG32F, "Tex2D load RG32F UAV invariant");
		benchTest(dx, shaderLoadTex2dRWLinear, loadCB, outputUAV, texUAV_RG32F, "Tex2D load RG32F UAV linear");
		benchTest(dx, shaderLoadTex2dRWRandom, loadCB, outputUAV, texUAV_RG32F, "Tex2D load RG32F UAV random");
		benchTest(dx, shaderLoadTex4dRWInvariant, loadCB, outputUAV, texUAV_RGBA32F, "Tex2D load RGBA32F UAV invariant");
		benchTest(dx, shaderLoadTex4dRWLinear, loadCB, outputUAV, texUAV_RGBA32F, "Tex2D load RGBA32F UAV linear");
		benchTest(dx, shaderLoadTex4dRWRandom, loadCB, outputUAV, texUAV_RGBA32F, "Tex2D load RGBA32F UAV random");
#endif

		dx.presentFrame();

		status = messagePump();
	}
	while (status != MessageStatus::Exit);

	return 0;
}