#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VESION 0x0800//DiewctInputのバージョン指定
#include <dinput.h>





//入力
class Input
{

public:

	//namespaceの省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//初期化
	void Initialize(HINSTANCE hInstance,HWND hwnd);
	//更新
	void Update();

private:

	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;


};
