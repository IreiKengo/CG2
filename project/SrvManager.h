#pragma once
#include <d3d12.h>
#include <cstdint>
#include <wrl.h>


class DirectXCommon;

//SRVの管理
class SrvManager
{
public:

	void Initialize(DirectXCommon* dxCommon);


	//最大SRV数（最大テクスチャ枚数）
	static const uint32_t kMaxSRVCount;

	uint32_t Allocate();

	D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(uint32_t index);
	D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(uint32_t index);


	//SRV生成（テクスチャ用）
	void CreateSRVforTexture2D(uint32_t srvIndex, ID3D12Resource* pResource, DXGI_FORMAT Format, UINT MipLevels);
	//SRV生成（Structured Buffer用）
	void CreateSRVforStructuredBuffer(uint32_t srvIndex, ID3D12Resource* pResource, UINT numElements, UINT structureByteStride);

	void PreDraw();

	void SetGraphicsRootDescriptorTable(UINT RootParameterIndex, uint32_t srvIndex);

	bool CanAllocateTexture();


private:

	DirectXCommon* dxCommon_ = nullptr;

	//デスクリプタサイズ
	uint32_t descriptorSize;

	//SRVのデスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> descriptorHeap;

	//次に使用するSRVインデックス
	uint32_t useIndex = 0;



};
