#pragma once
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include <externals/DirectXTex/DirectXTex.h>
#include "DirectXCommon.h"
#include <unordered_map>

class SrvManager;

//テクスチャマネージャー
class TextureManager
{
public:
	//シングルトンインスタンスの取得
	static TextureManager* GetInstance();
	//終了
	void Finalize();

	//初期化
	void Initialize(DirectXCommon* dxCommon,SrvManager* srvManager);

	//テクスチャファイルの読み込み
	void LoadTexture(const std::string& filePath);

	void ReleaseIntermediateResources();

	//メタデータを取得
	const DirectX::TexMetadata& GetMetaData(const std::string& filePath);

	//SRVインデックスの取得
	uint32_t GetSrvIndex(const std::string& filePath);

	//GPUハンドルの取得
	D3D12_GPU_DESCRIPTOR_HANDLE GetSrvHandleGPU(const std::string& filePath);


private:
	static TextureManager* instance;

	TextureManager() = default;
	~TextureManager() = default;
	TextureManager(TextureManager&) = delete;
	TextureManager& operator=(TextureManager&) = default;

	//テクスチャ1枚分のデータ
	struct TextureData
	{

		DirectX::TexMetadata metadata;//画像の幅や高さなどの情報
		Microsoft::WRL::ComPtr<ID3D12Resource> resource;//テクスチャリソース
		uint32_t srvIndex;
		D3D12_CPU_DESCRIPTOR_HANDLE srvHandleCPU;//SRV作成時に必要なCPUハンドル
		D3D12_GPU_DESCRIPTOR_HANDLE srvHandleGPU;//描画コマンドに必要なGPUハンドル
		Microsoft::WRL::ComPtr<ID3D12Resource> intermediateResource;//転送用中間リソース

	};

	//テクスチャデータ
	std::unordered_map<std::string,TextureData> textureDatas;
	

	DirectXCommon* dxCommon_ = nullptr;

	//SRVインデックスの開始番号
	static uint32_t kSRVIndexTop;

	SrvManager* srvManager_ = nullptr;

};
