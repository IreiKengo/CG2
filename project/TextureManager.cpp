#include "TextureManager.h"



TextureManager* TextureManager::instance = nullptr;

//ImGuiで0番目を使用するため、1番から使用
uint32_t TextureManager::kSRVIndexTop = 1;

void TextureManager::Initialize(DirectXCommon* dxCommon)
{
	dxCommon_ = dxCommon;


	//SRVの数と同数
	textureDatas.reserve(DirectXCommon::kMaxSRVCount);

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
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		//読み込み済みなら早期return
		return;
	}

	//テクスチャ枚数上限チェック
	assert(textureDatas.size() + kSRVIndexTop < DirectXCommon::kMaxSRVCount);


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

	//テクスチャデータを追加
	textureDatas.resize(textureDatas.size() + 1);
	//追加したテクスチャデータの参照を取得する
	TextureData& textureData = textureDatas.back();

	//テクスチャデータ書き込み
	textureData.filePath = filePath;//ファイルパス
	textureData.metadata = mipImages.GetMetadata();//テクスチャメタデータを取得
	textureData.resource = dxCommon_->CreateTextureResource(textureData.metadata);//テクスチャリソース生成

	//テクスチャデータの要素数番号からSRVのインデックスを計算する
	uint32_t srvIndex = static_cast<uint32_t>(textureDatas.size() - 1) + kSRVIndexTop;


	textureData.srvHandleCPU = dxCommon_->GetSRVCPUDescriptorHandle(srvIndex);
	textureData.srvHandleGPU = dxCommon_->GetSRVGPUDescriptorHandle(srvIndex);

	//SRVの設定を行う
	D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc{};
	srvDesc.Format = textureData.metadata.format;
	srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;//2Dテクスチャ
	srvDesc.Texture2D.MipLevels = UINT(textureData.metadata.mipLevels);
	dxCommon_->GetDevice()->CreateShaderResourceView(textureData.resource.Get(), &srvDesc, textureData.srvHandleCPU);//設定をもとにSRVの生成

	//テクスチャデータ転送
	//転送用に生成した中間リソースをテクスチャデータ構造体に格納
	textureData.intermediateResource = dxCommon_->UploadTextureData(textureData.resource, mipImages);

}

void TextureManager::ReleaseIntermediateResources()
{
	for (auto& tex : textureDatas) {
		if (tex.intermediateResource) {
			tex.intermediateResource.Reset();
		}
	}
}

uint32_t TextureManager::GetTextureIndexByFilePath(const std::string& filePath)
{
	
	//読み込み済みテクスチャデータを検索
	auto it = std::find_if(
		textureDatas.begin(),
		textureDatas.end(),
		[&](TextureData& textureData) {return textureData.filePath == filePath; }
	);
	if (it != textureDatas.end()) {
		//読み込み済みなら要素番号を返す
		uint32_t textureIndex = static_cast<uint32_t>(std::distance(textureDatas.begin(), it));
		return textureIndex;
	}

	assert(0);
	return 0;

}

D3D12_GPU_DESCRIPTOR_HANDLE TextureManager::GetSrvHandleGPU(uint32_t textureIndex)
{

	//範囲外指定違反チェック
	assert(textureIndex < textureDatas.size());

	TextureData& textureData = textureDatas[textureIndex];
	return textureData.srvHandleGPU;
}
