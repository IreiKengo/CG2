#pragma once
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include <string>
#include <wrl.h>
#include <vector>
#include <d3d12.h>
#include "Transform.h"
#include "Matrix4x4Math.h"


class Object3dCommon;
class DirectXCommon;

//3Dオブジェクト
class Object3d
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

	struct MaterialData
	{
		std::string textureFilePath;
		uint32_t textureIndex = 0;
	};

	struct ModelData
	{
		std::vector<VertexData> vertices;
		MaterialData material;
	};

	//座標変換行列データ
	struct TransformationMatrix
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
	};

	struct DirectionalLight
	{
		Vector4 color; //ライトの色
		Vector3 direction; //ライトの向き
		float intensity; //輝度
	};

	void Initialize(Object3dCommon* object3dCommon);
	void Update();
	void Draw();

	
	

private:

	Object3dCommon* object3dCommon = nullptr;

	ModelData modelData;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;
	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;
	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;


	//.mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResources;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	//平行光源用のリソースを作る。今回はカラー１つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;

	Transform transform;
	Transform cameraTransform;


	DirectXCommon* dxCommon_ = nullptr;

	//頂点データの作成
	void CreateVertexData();
	//マテリアルデータの作成
	void CreateMaterialData();
	//座標返還行列データ作成
	void CreateTransformationMatrixData();
	//平行光源データ作成
	void CreateDirectionalLightData();

};