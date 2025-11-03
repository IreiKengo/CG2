#pragma once
#include <Windows.h>
#include <cstdint>


//WindowsAPI
class WinApp
{

public:

	static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

	//初期化
	void Initialize();
	
	//終了
	void Finilize();

	//クライアント領域のサイズ
	static const int32_t kClientWidth = 1280;
	static const int32_t kClientHeight = 720;


	//getter
	HWND GetHwnd()const { return hwnd; }
	//getter
	HINSTANCE GetHInstance() const { return wc.hInstance; }

	//メッセージの処理
	bool ProcessMessage();

private:

	HWND hwnd = nullptr;

	//ウィンドウクラスの設定
	WNDCLASS wc{};

};
