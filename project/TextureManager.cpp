#include "TextureManager.h"
#include "SrvManager.h"



TextureManager* TextureManager::instance = nullptr;

//ImGuiで0番目を使用するため、1番から使用
//uint32_t TextureManager::kSRVIndexTop = 1;

void TextureManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{
	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	//SRVの数と同数
	textureDatas.reserve(SrvManager::kMaxSRVCount);

}

TextureManager* TextureManager::GetInstance()
{

	if (instance == nullptr)
	{
		instance = new TextureManager;
	}
	return instance;

}

void TextureManager::Finalize()
{

	delete instance;
	instance = nullptr;

}

void TextureManager::LoadTexture(const std::string& filePath)
{
	assert(dxCommon_ && "TextureManager::Initialize() not called");

	//読み込み済みテクスチャを検索
	if (textureDatas.contains(filePath)) {
		//読み込み済みなら早期return
		return;
	}

	//テクスチャ枚数上限チェック
	assert(srvManager_->CanAllocateTexture());


	//テクスチャファイルを読んでプログラムを扱えるようにする
 // ローカル変数としてScratchImageを生成
	DirectX::ScratchImage image{};


	// std::string -> std::wstring 変換
	std::wstring filePathw(filePath.begin(), filePath.end());

	// WIC経由で画像をロード
	HRESULT hr = DirectX::LoadFromWICFile(
		filePathw.c_str(),
		DirectX::WIC_FLAGS_FORCE_SRGB,
		nullptr,
		image);
	assert(SUCCEEDED(hr));

	// ミップマップ生成
	DirectX::ScratchImage mipImages{};
	hr = DirectX::GenerateMipMaps(
		image.GetImages(),
		image.GetImageCount(),
		image.GetMetadata(),
		DirectX::TEX_FILTER_SRGB,
		0,
		mipImages);
	assert(SUCCEEDED(hr));

	
	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas[filePath];

	//テクスチャデータ書き込み
	
	textureData.metadata = mipImages.GetMetadata();//テクスチャメタデータを取得
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);//テクスチャリソース生成

	//テクスチャデータの要素数番号からSRVのインデックスを計算する
	//uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;

	textureData.srvIndex = srvManager_->Allocate();
	textureData.srvHandleCPU = srvManager_->GetCPUDescriptorHandle(textureData.srvIndex);
	textureData.srvHandleGPU = srvManager_->GetGPUDescriptorHandle(textureData.srvIndex);


	srvManager_->CreateSRVforTexture2D(
		textureData.srvIndex,
		textureData.resource.Get(),
		textureData.metadata.format, 
		static_cast<UINT>(textureData.metadata.mipLevels));

	//テクスチャデータ転送
	//転送用に生成した中間リソースをテクスチャデータ構造体に格納
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource, mipImages);

}

void TextureManager::ReleaseIntermediateResources()
{
	for (auto& tex : textureDatas) {
		if (tex.second.intermediateResource) {
			tex.second.intermediateResource.Reset();
		}
	}
}

uint32_t TextureManager::GetSrvIndex(const std::string& filePath)
{

	auto it = textureDatas.find(filePath);
	assert(it != textureDatas.end());
	return it->second.srvIndex;

}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(const std::string& filePath)
{
	//検索
	auto it = textureDatas.find(filePath);

	assert(it != textureDatas.end());
	return it->second.srvHandleGPU;
}

const DirectX::TexMetadata& TextureManager::GetMetaData(const std::string& filePath)
{
	auto it = textureDatas.find(filePath);
	assert(it != textureDatas.end());
	return it->second.metadata;

}
