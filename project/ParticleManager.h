#pragma once
#include <d3d12.h>
#include <wrl.h>
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4.h"
#include "Transform.h"
#include <random>
#include <string>
#include <vector>
#include <list>
#include <unordered_map>


class DirectXCommon;
class SrvManager;

class ParticleManager
{

public:

	struct Particle
	{
		Transform transform;
		Vector3 velocity;
		Vector4 color;
		float lifeTime;
		float currentTime;
	};

	struct ParticleForGPU
	{
		Matrix4x4 WVP;
		Matrix4x4 World;
		Vector4 color;
	};

	//頂点データ
	struct VertexData {
		Vector4 position;
		Vector2 texCoord;
		Vector3 normal;
	};

	//マテリアルデータ
	struct MaterialData
	{

		std::string textureFilePath;//テクスチャファイルパス
		uint32_t srvIndex;//テクスチャ用SRVインデックス

	};

	struct ModelData
	{
		std::vector<VertexData> vertices;
	
	};


	struct ParticleGroup
	{

		MaterialData materialData;
		std::list<Particle> particles;//パーティクルのリスト
		uint32_t instancingSrvIndex;//インスタンシングデータ用SRVインデックス
		Microsoft::WRL::ComPtr<ID3D12Resource> instancingResource;//インスタンシングリソース
		uint32_t numInstance = 0;//インスタンス数
		ParticleForGPU* instancingData = nullptr;//インスタンシングデータを書き込むためのポインタ

	};

	void Initialize(DirectXCommon* dxCommon, SrvManager* srvManager);
	void Update();

	//パーティクル生成
	Particle MakeNewParticle(std::mt19937& randomEngine, const Vector3& translate);

	void CreateParticleGroup(const std::string name, const std::string textureFilePath);

private:

	DirectXCommon* dxCommon_ = nullptr;
	SrvManager* srvManager_ = nullptr;


	Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature = nullptr;
	Microsoft::WRL::ComPtr<ID3D12PipelineState> graphicsPipelineState = nullptr;

	ModelData modelData;


	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> vertexResource;

	//バッファリソースの使い道を補足するバッファビュー
	D3D12_VERTEX_BUFFER_VIEW vertexBufferView;


	std::unordered_map < std::string, ParticleGroup> particleGroups;


	const uint32_t kNumMaxInstance = 100;//最大インスタンス数

	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};

	//Δtを定義。とりあえず60fps固定してあるが、実時間を計測して可変fpsで動かせるようにしておくとなお良い
	const float kDeltaTime = 1.0f / 60.0f;


	//bool useBillboard = true;

	

	//ルートシグネチャの作成
	void CreateRootSignature();
	//グラフィックスパイプラインの作成
	void CreateGraphicsPipeline();


};
