#pragma once
#include <string>
#include <vector>
#include <wrl.h>
#include <d3d12.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4Math.h"


class ModelCommon;
class DirectXCommon;

//3Dモデル
class Model
{

public:

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
		Vector3 normal;
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

	//マテリアルデータ
	struct Material
	{
		Vector4 color;
		int32_t enableLighting;
		float padding[3];
		Matrix4x4 uvTransform;
	};


	void Initialize(ModelCommon* modelCommon,const std::string& directoryPath,const std::string&filename);

	void Draw();

private:

	ModelCommon* modelCommon_ = nullptr;

	ModelData modelData;

	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;

	//バッファリソース内のデータを指すポインタ
	VertexData* vertexData = nullptr;

	//マテリアルリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> materialResource;
	//バッファリソース内のデータを指すポインタ
	Material* materialData = nullptr;


	//.mtlファイルの読み取り
	static MaterialData LoadMaterialTemplateFile(const std::string& directoryPath, const std::string& filename);
	//.objファイルの読み取り
	static ModelData LoadObjFile(const std::string& directoryPath, const std::string& filename);

	DirectXCommon* dxCommon_ = nullptr;

	//頂点データの作成
	void CreateVertexData();
	//マテリアルデータの作成
	void CreateMaterialData();

};
