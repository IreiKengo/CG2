#pragma once

#ifdef USE_IMGUI
#include "externals/imgui/imgui.h"
#include "externals/imgui/imgui_impl_win32.h"
#include "externals/imgui/imgui_impl_dx12.h"
#endif 


class WinApp;
class DirectXCommon;
class SrvManager;

class ImguiManager
{
public:

	void Initialize(WinApp* winApp, DirectXCommon* dxCommon, SrvManager* srvManager);
	void Finalize();

	void Begin();
	void End();
	void Draw();

private:

	WinApp* winApp_ = nullptr;
	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;

};
