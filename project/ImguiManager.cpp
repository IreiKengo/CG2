#include "ImguiManger.h"



#include "WinApp.h"
#include "DirectXCommon.h"
#include "SrvManager.h"

void ImguiManager::Initialize([[maybe_unused]] WinApp* winApp, [[maybe_unused]] DirectXCommon* dxCommon, [[maybe_unused]] SrvManager* srvManager)
{
#ifdef USE_IMGUI


	winApp_ = winApp;
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui::StyleColorsDark();
	ImGui_ImplWin32_Init(winApp_->GetHwnd());
	/*ImGui_ImplDX12_Init(device.Get(),
		swapChainDesc.BufferCount,
		rtvFormat_,
		srvDescriptorHeap.Get(),
		srvDescriptorHeap->GetCPUDescriptorHandleForHeapStart(),
		srvDescriptorHeap->GetGPUDescriptorHandleForHeapStart());*/

	ImGui_ImplDX12_InitInfo init_info = {};
	init_info.Device = dxCommon_->GetDevice();
	init_info.CommandQueue = dxCommon_->GetCommandQueue();
	init_info.NumFramesInFlight = dxCommon_->GetSwapChainDesc().BufferCount;
	init_info.RTVFormat = dxCommon_->GetRTVFormat();
	init_info.DSVFormat = DXGI_FORMAT_UNKNOWN;//深度不要
	// Allocating SRV descriptors (for textures) is up to the application, so we provide callbacks.
	// (current version of the backend will only allocate one descriptor, future versions will need to allocate more)
	init_info.SrvDescriptorHeap = srvManager_->GetDescriptorHeap();

	init_info.SrvDescriptorAllocFn = []
	(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE* cpu, D3D12_GPU_DESCRIPTOR_HANDLE* gpu) {
		SrvManager* srv = static_cast<SrvManager*>(info->UserData);
		uint32_t index = srv->Allocate();
		*cpu = srv->GetCPUDescriptorHandle(index);
		*gpu = srv->GetGPUDescriptorHandle(index);
		};

	init_info.SrvDescriptorFreeFn = []
	(ImGui_ImplDX12_InitInfo* info, D3D12_CPU_DESCRIPTOR_HANDLE cpu, D3D12_GPU_DESCRIPTOR_HANDLE gpu) {};
	init_info.UserData = srvManager_;  // ← これ超重要
	ImGui_ImplDX12_Init(&init_info);

#endif
}

void ImguiManager::Finalize()
{
#ifdef USE_IMGUI
	ImGui_ImplDX12_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
#endif
}

void ImguiManager::Begin()
{
#ifdef USE_IMGUI
	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#endif
}

void ImguiManager::End()
{
#ifdef USE_IMGUI
	//描画前準備
	ImGui::Render();
#endif
}

void ImguiManager::Draw()
{
#ifdef USE_IMGUI
	ID3D12GraphicsCommandList* commandList = dxCommon_->GetCommandList();

	//デスクリプタヒープの配列をセットするコマンド
	ID3D12DescriptorHeap* ppHeaps[] = { srvManager_->GetDescriptorHeap() };
	commandList->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
	//描画コマンドを発行
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), commandList);
#endif
}