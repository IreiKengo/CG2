#pragma once
#include "DirectXCommon.h"

//3Dオブジェクト共通部
class Object3dCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);
	//共通画面設定
	void ScreenCommon();



	DirectXCommon* GetCommon()const { return dxCommon_; }

private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;

	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	DirectXCommon* dxCommon_;

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();

};
