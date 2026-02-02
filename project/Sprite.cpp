#include "Sprite.h"
#include <dxgiformat.h>
#include "SpriteCommon.h"
#include "DirectXCommon.h"
#include "Matrix4x4Math.h"
#include "TextureManager.h"
#include "ImguiManger.h"

using namespace math;

void Sprite::Initialize(SpriteCommon* spriteCommon, std::string textureFilePath)
{

	//引数で受け取ってメンバ変数に記録する
	this->spriteCommon = spriteCommon;
	dxCommon_ = spriteCommon->GetDxCommon();

	textureFilePath_ = textureFilePath;

	CreateVertexData();

	CreateMaterialData();

	CreateTransformationMatrixData();

	TextureManager::GetInstance()->LoadTexture(textureFilePath_);

	//単位行列を書き込んでおく
	//textureIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);

	AdjustTextureSize();

}

void Sprite::Update()
{

	float left = 0.0f - anchorPoint.x;
	float right = 1.0f - anchorPoint.x;
	float top = 0.0f - anchorPoint.y;
	float bottom = 1.0f - anchorPoint.y;

	//左右反転
	if (isFlipX_)
	{
		left = -left;
		right = -right;
	}
	//上下反転
	if (isFlipY_)
	{
		top = -top;
		bottom = -bottom;
	}

	const DirectX::TexMetadata& metadata =
		TextureManager::GetInstance()->GetMetaData(textureFilePath_);
	float tex_left = textureLeftTop.x / metadata.width;
	float tex_right = (textureLeftTop.x + textureSize.x) / metadata.width;
	float tex_top = textureLeftTop.y / metadata.height;
	float tex_bottom = (textureLeftTop.y + textureSize.y) / metadata.height;

	//頂点リソースにデータを書き込む（4点分）
	vertexData[0].position = { left,bottom,0.0f,1.0f };//左下
	vertexData[0].texCoord = { tex_left,tex_bottom };
	vertexData[0].normal = { 0.0f,0.0f,-1.0f };

	vertexData[1].position = { left,top,0.0f,1.0f };//左上
	vertexData[1].texCoord = { tex_left,tex_top };
	vertexData[1].normal = { 0.0f,0.0f,-1.0f };

	vertexData[2].position = { right,bottom,0.0f,1.0f };//右下
	vertexData[2].texCoord = { tex_right,tex_bottom };
	vertexData[2].normal = { 0.0f,0.0f,-1.0f };

	vertexData[3].position = { right,top,0.0f,1.0f };//右上
	vertexData[3].texCoord = { tex_right,tex_top };
	vertexData[3].normal = { 0.0f,0.0f,-1.0f };



	//インデックスリソースにデータを書き込む（6個分）
	indexData[0] = 0;
	indexData[1] = 1;
	indexData[2] = 2;
	indexData[3] = 1;
	indexData[4] = 3;
	indexData[5] = 2;


	//Transform情報を作る
	transform = { {size.x,size.y,1.0f},{0.0f,0.0f,rotation},{position.x,position.y,0.0f} };
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 viewMatrix = MakeIdentity4x4();
	Matrix4x4 projectionMatrix = MakeOrthographicMatrix(0.0f, 0.0f, float(WinApp::kClientWidth), float(WinApp::kClientHeight), 0.0f, 100.0f);
	transformationMatrixData->WVP = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->World = worldMatrix;

}

void Sprite::DebugUpdate()
{
#ifdef USE_IMGUI
	ImGui::SetNextWindowSize(ImVec2(500, 100), ImGuiCond_FirstUseEver);
	ImGui::Begin("SpriteSettings");
	/*bool prevUseMonsterBall = useMonsterBall;
	ImGui::Checkbox("Use MonsterBall", &useMonsterBall);


	if (prevUseMonsterBall != useMonsterBall)
	{
		if (useMonsterBall)
		{
			TextureChange("resources/monsterBall.png");
		} else
		{
			TextureChange("resources/uvChecker.png");
		}
	}*/


	

	if (ImGui::SliderFloat2("position", &transform.translate.x, 0.0f, 1280.0f,"%.1f"))
	{
		SetPosition({ transform.translate.x,transform.translate.y });

	}

	ImGui::End();

#endif 


}

void Sprite::Draw()
{

	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	dxCommon_->GetCommandList()->IASetIndexBuffer(&indexBufferView);//IBVを設定
	//マテリアルCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0, materialResource->GetGPUVirtualAddress());
	//TransformationMatrixCBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResources->GetGPUVirtualAddress());
	dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, TextureManager::GetInstance()->GetSrvHandleGPU(textureFilePath_));


	//描画！（DrawCall/ドローコール）
	dxCommon_->GetCommandList()->DrawIndexedInstanced(6, 1, 0, 0, 0);

}

void Sprite::TextureChange(std::string textureFilePath)
{

	//auto texMgr = TextureManager::GetInstance();

	//// テクスチャインデックスを差し替えるだけ
	//textureIndex = texMgr->GetSrvIndex(textureFilePath);

	textureFilePath_ = textureFilePath;
	AdjustTextureSize();

}

void Sprite::CreateVertexData()
{

	//頂点リソースを作る
	vertexResource = dxCommon_->CreateBufferResource(sizeof(VertexData) * 4);
	//インデックス用リソースを作る
	indexResource = dxCommon_->CreateBufferResource(sizeof(uint32_t) * 6);


	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点4つ分のサイズ
	vertexBufferView.SizeInBytes = sizeof(VertexData) * 4;
	//１頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	//リソースの先頭のアドレスから使う
	indexBufferView.BufferLocation = indexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズはインデックス６つ分のサイズ
	indexBufferView.SizeInBytes = sizeof(uint32_t) * 6;
	//インデックスはuint32_tとする
	indexBufferView.Format = DXGI_FORMAT_R32_UINT;


	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));
	indexResource->Map(0, nullptr, reinterpret_cast<void**>(&indexData));

}

void Sprite::CreateMaterialData()
{

	//マテリアルリソースを作る
	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));


	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	//マテリアルデータの初期化を書き込む
	//白を書き込んでいる
	materialData->color = Vector4(1.0f, 1.0f, 1.0f, 1.0f);
	materialData->enableLighting = false;
	materialData->uvTransform = MakeIdentity4x4();


}

void Sprite::CreateTransformationMatrixData()
{
	//座標変換行列リソースを作る
	transformationMatrixResources = dxCommon_->CreateBufferResource(sizeof(TransformationMatrix));

	//書き込むためのアドレスを取得
	transformationMatrixResources->Map(0, nullptr, reinterpret_cast<void**>(&transformationMatrixData));


	//単位行列を書き込んでいく
	transformationMatrixData->WVP = MakeIdentity4x4();
	transformationMatrixData->World = MakeIdentity4x4();
	//transformationMatrixData->World = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);

}

void Sprite::AdjustTextureSize()
{

	//テクスチャメタデータを取得
	const DirectX::TexMetadata& metadata = TextureManager::GetInstance()->GetMetaData(textureFilePath_);

	textureSize.x = static_cast<float>(metadata.width);
	textureSize.y = static_cast<float>(metadata.height);
	//画像サイズをテクスチャサイズに合わせる
	size = textureSize;

}
