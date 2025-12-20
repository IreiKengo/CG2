#pragma once
#include "DirectXCommon.h"

class Camera;

//3Dオブジェクト共通部
class Object3dCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);
	//共通画面設定
	void ScreenCommon();

	DirectXCommon* GetCommon()const { return dxCommon_; }

	//setter
	void SetDefaultCamera(Camera* camera) { this->defaultCamera = camera; }

	//getter
	Camera* GetDefaultCamera()const { return defaultCamera; }

private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	DirectXCommon* dxCommon_;

	Camera* defaultCamera = nullptr;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

};
