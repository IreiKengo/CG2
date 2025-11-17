#pragma once
#include "DirectXCommon.h"

//スプライト共通部
class SpriteCommon
{

public:

	void Initialize(DirectXCommon* dxCommon);
	//共通画面設定
	void DrawCommon();


	DirectXCommon* GetDxCommon()const { return dxCommon_; }
	

private:

	//ルートシグネチャ
	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	DirectXCommon* dxCommon_;


	//ルートシグネチャの作成
	void InitializeRootSignature();
	//グラフィックスパイプラインの作成
	void InitializeGraphicsPipeline();

	


};
