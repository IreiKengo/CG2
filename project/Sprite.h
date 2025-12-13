#pragma once
#include"Vector2.h"
#include"Vector3.h"
#include"Vector4.h"
#include <wrl.h>
#include <d3d12.h>
#include <cstdint>
#include "Transform.h"
#include "Matrix4x4.h"
#include <string>

class SpriteCommon;
class DirectXCommon;

//スプライト
class Sprite
{
public:
	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
		Vector3 normal;
	};
	//マテリアルデータ
	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};
	//座標変換行列データ
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	void Initialize(SpriteCommon* spriteCommon, DirectXCommon* dxCommon,std::string textureFilePath);

	void Update();

	void Draw();

	//getter
	const Vector2& GetPosition()const { return position; }
	float GetRotation() const { return rotation; }
	const Vector4& GetColor() const { return materialData->color; }
	const Vector2& GetSize() const { return size; }
	//setter
	void SetPosition(const Vector2& position) { this->position = position; }
	void SetRotation(float rotation) { this->rotation = rotation; }
	void SetColor(const Vector4& color) { materialData->color = color; }
	void SetSize(const Vector2& size) { this->size = size; }


private:

	SpriteCommon* spriteCommon = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	Microsoft::WRL::ComPtr<ID3D12Resource> indexResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	uint32_t* indexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView{};
	D3D12_INDEX_BUFFER_VIEW indexBufferView{};

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;;
	//バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResources;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	Transform transform;;

	Transform uvTransformSprite
	{
		{1.0f,1.0f,1.0f},{0.0f,0.0f,0.0f},{0.0f,0.0f,0.0f},
	};


	//座標
	Vector2 position = { 0.0f,0.0f };
	//回転
	float rotation = 0.0f;
	//サイズ
	Vector2 size = { 640.0f,360.0f };

	//テクスチャ番号
	uint32_t textureIndex = 0;


	//DirectXCommon
	DirectXCommon* dxCommon_;

	//頂点データの作成
	void CreateVertexData();
	//マテリアルデータの作成
	void CreateMaterialData();
	//座標返還行列データ作成
	void CreateTransformationMatrixData();

};
