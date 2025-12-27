#pragma once
#include <d3d12.h>
#include <dxgi1_6.h>
#include <wrl.h>
#include <array>
#include "WinApp.h"
#include <dxcapi.h>
#include <string>
#include "externals/DirectXTex/DirectXTex.h"
#include <chrono>




//DirectX基盤
class DirectXCommon
{
public:

	void Initialize(WinApp*winApp);

	//描画前処理
	void PreDraw();
	//描画後処理
	void PostDraw();
	

	


	//getter
	ID3D12Device* GetDevice() const { return device.Get(); }
	ID3D12GraphicsCommandList* GetCommandList() const { return commandList.Get(); }
	HANDLE GetFenceEvent()const { return fenceEvent; }

	//シェーダーのコンパイル
	Microsoft::WRL::ComPtr<IDxcBlob> CompileShader(
		const std::wstring& filePath,
		const wchar_t* profile);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateBufferResource(size_t sizeInBytess);

	Microsoft::WRL::ComPtr<ID3D12Resource> CreateTextureResource(const DirectX::TexMetadata& metadata);

	[[nodiscard]]
	Microsoft::WRL::ComPtr<ID3D12Resource> UploadTextureData(const Microsoft::WRL::ComPtr<ID3D12Resource>& texture, const DirectX::ScratchImage& mipImages);
	

	//デスクリプタヒープ生成関数
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> CreateDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE heapType, UINT numDesctiptors, bool shaderVisible);

private:

	//DirectX12デバイス
	Microsoft::WRL::ComPtr<ID3D12Device> device;
	//DXGIファクトリ
	Microsoft::WRL::ComPtr<IDXGIFactory7> dxgiFactory;

	//コマンドアロケータ
	Microsoft::WRL::ComPtr<ID3D12CommandAllocator> commandAllocator = nullptr;
	//コマンドリスト
	Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> commandList = nullptr;
	//コマンドキュー
	Microsoft::WRL::ComPtr<ID3D12CommandQueue> commandQueue = nullptr;
	//スワップチェーン
	Microsoft::WRL::ComPtr<IDXGISwapChain4> swapChain = nullptr;
	DXGI_SWAP_CHAIN_DESC1 swapChainDesc{};
	Microsoft::WRL::ComPtr<ID3D12Resource> depthResource = nullptr;
	//深度バッファ
//	HRESULT hr;
	//各種デスクリプタサイズ
	uint32_t descriptorSizeRTV;
	uint32_t descriptorSizeDSV;

	//RTVのデスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> rtvDescriptorHeap;
	
	//DSVのデスクリプターヒープ
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> dsvDescriptorHeap;

	//RTVを２つ作るのでディスクリプタを２つ用意
	D3D12_CPU_DESCRIPTOR_HANDLE rtvHandles[2];

	//スワップチェーンリソース
	std::array<Microsoft::WRL::ComPtr<ID3D12Resource>, 2> swapChainResources;

	//初期値0でFenceを作る
	Microsoft::WRL::ComPtr<ID3D12Fence> fence = nullptr;
	uint64_t fenceValue = 0;
	HANDLE fenceEvent;

	//ビューポート
	D3D12_VIEWPORT viewport{};
	//シザー矩形
	D3D12_RECT scissorRect{};

	//dxcCompilerを初期化
	Microsoft::WRL::ComPtr<IDxcUtils> dxcUtils = nullptr;
	Microsoft::WRL::ComPtr < IDxcCompiler3> dxcCompiler = nullptr;

	//現時点でincludeはしないが、includeに対応するための設定を行っておく
	Microsoft::WRL::ComPtr <IDxcIncludeHandler> includeHandler = nullptr;

	D3D12_RESOURCE_BARRIER barrier{};


	DXGI_FORMAT rtvFormat_ = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;

	//WindowsAPI
WinApp* winApp_ = nullptr;


//記録時間
std::chrono::steady_clock::time_point reference_;







//デバイスの初期化
void InitializeDevice();

//コマンド関連の初期化
void InitializeCommand();

//スワップチェーンの生成
void InitializeSwapChain();

//深度バッファの生成
void CreateDepthStencilTextureResource();

//各種デスクリプタヒープの生成
void CreateDescriptorHeaps();

//レンダーターゲットビューの初期化
void InitializeRenderTargetView();

//深度ステンシルビューの初期化
void InitializeDepthStencilView();

//フェンスの生成
void CreateFence();

//ビューポート矩形の初期化
void InitializeViewportRect();

//シザリング矩形の初期化
void InitializescissorRect();

//DXCコンパイラの生成
void CreateDXCCompiler();

//ImGuiの初期化
//void InitializeImGui();

//FPS固定初期化
void InitializeFixFPS();
//FPS固定更新
void UpdateFixFPS();





static D3D12_CPU_DESCRIPTOR_HANDLE GetCPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);
static D3D12_GPU_DESCRIPTOR_HANDLE GetGPUDescriptorHandle(const Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descriptorHeap, uint32_t descriptorSize, uint32_t index);



};