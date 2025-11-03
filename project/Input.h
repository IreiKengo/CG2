#pragma once
#include <windows.h>
#include <wrl.h>
#define DIRECTINPUT_VESION 0x0800//DiewctInputのバージョン指定
#include <dinput.h>
#include "WinApp.h"




//入力
class Input
{

public:

	//namespaceの省略
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

	//初期化
	void Initialize(WinApp* winApp);
	//更新
	void Update();

	//押されているか
	bool PushKey(BYTE keyNumber);

	//トリガーか
	bool TriggerKey(BYTE keyNumber);

private:

	//キーボードのデバイス
	ComPtr<IDirectInputDevice8> keyboard;

	//全キーの状態
	BYTE key[256] = {};
	//前回の全キーの状態
	BYTE keyPre[256] = {};
	//DirectInputのインスタンス
	ComPtr<IDirectInput8> directInput;

	//WindowsAPI
	WinApp* winApp_ = nullptr;


};
