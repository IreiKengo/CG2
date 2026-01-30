#include "ParticleManager.h"
#include "DirectXCommon.h"
#include"SrvManager.h"
#include "Logger.h"
#include "StringUtility.h"
#include <iostream>
#include "Matrix4x4Math.h"
#include "TextureManager.h"
#include <numbers>
#include "Camera.h"


using namespace StringUtility;
using namespace Logger;
using namespace math;



ParticleManager* ParticleManager::GetInstance()
{
	static ParticleManager instance;
	return &instance;
}


void ParticleManager::Initialize(DirectXCommon* dxCommon, SrvManager* srvManager)
{

	dxCommon_ = dxCommon;
	srvManager_ = srvManager;

	std::random_device seedGenerator;
	randomEngine_ = std::mt19937(seedGenerator());

	CreateGraphicsPipeline();

	modelData.vertices.push_back({ .position = {1.0f,1.0f,0.0f,1.0f},.texCoord = {0.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//左上
	modelData.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texCoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texCoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左下
	modelData.vertices.push_back({ .position = {1.0f,-1.0f,0.0f,1.0f},.texCoord = {0.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//左下
	modelData.vertices.push_back({ .position = {-1.0f,1.0f,0.0f,1.0f},.texCoord = {1.0f,0.0f},.normal = {0.0f,0.0f,1.0f} });//右上
	modelData.vertices.push_back({ .position = {-1.0f,-1.0f,0.0f,1.0f},.texCoord = {1.0f,1.0f},.normal = {0.0f,0.0f,1.0f} });//右下


	//頂点リソース
	vertexResource = dxCommon->CreateBufferResource(sizeof(VertexData) * modelData.vertices.size());

	//リソースの先頭のアドレスから使う
	vertexBufferView.BufferLocation = vertexResource->GetGPUVirtualAddress();
	//使用するリソースのサイズは頂点のサイズ
	vertexBufferView.SizeInBytes = UINT(sizeof(VertexData) * modelData.vertices.size());
	//１頂点当たりのサイズ
	vertexBufferView.StrideInBytes = sizeof(VertexData);

	VertexData* vertexData = nullptr;
	//頂点リソースにデータを書き込む
	vertexResource->Map(0, nullptr, reinterpret_cast<void**>(&vertexData));//書き込むためのアドレスを取得
	std::memcpy(vertexData, modelData.vertices.data(), sizeof(VertexData) * modelData.vertices.size());//頂点データをリソースにコピー

	accelerationField.acceleration = { -10.0f,0.0f,0.0f };
	accelerationField.area.min = { -5.0f,-5.0f,-5.0f };
	accelerationField.area.max = { 1.0f,1.0f,1.0f };


	materialResource = dxCommon_->CreateBufferResource(sizeof(Material));
	materialResource->Map(0, nullptr, reinterpret_cast<void**>(&materialData));
	materialData->color = { 1,1,1,1 };
	materialData->uvTransform = MakeIdentity4x4();

}

void ParticleManager::Update()
{
	//ビルボード行列の計算
	Matrix4x4 backToFrontMatrix = MakeRotateYMatrix(std::numbers::pi_v<float>);
	Matrix4x4 billboardMatrix = Multiply(backToFrontMatrix, camera_->GetWorldMatrix());
	billboardMatrix.m[3][0] = 0.0f;
	billboardMatrix.m[3][1] = 0.0f;
	billboardMatrix.m[3][2] = 0.0f;

	//ビュー行列とプロジェクション行列をカメラから取得
	Matrix4x4 viewMatrix = camera_->GetViewMatrix();
	Matrix4x4 projectionMatrix = camera_->GetProjectionMatrix();

	//全てのパーティクルグループについて処理する
	//グループ内の全てのパーティクルについて処理する
	for (auto& [groupName, group] : particleGroups)
	{
		group.numInstance = 0;

		for (auto particleIterator = group.particles.begin(); particleIterator != group.particles.end(); )
		{
			//寿命に達していたらグループから外す
			if ((*particleIterator).lifeTime <= (*particleIterator).currentTime)//生存期間を過ぎていたら更新せず描画対象にしない
			{
				particleIterator = group.particles.erase(particleIterator);//生存時間が過ぎたParticleはlistから消す。戻り値が次のイテレータとなる
				continue;
			}

			//場の影響を計算（加速）
			//Fieldの範囲内のParticleには加速度を適用する
			if (IsCollision(accelerationField.area, (*particleIterator).transform.translate))
			{
				(*particleIterator).velocity += accelerationField.acceleration * kDeltaTime;
			}

			//移動処理（速度を座標に加算）
			(*particleIterator).transform.translate += (*particleIterator).velocity * kDeltaTime;
			//経過時間を加算
			(*particleIterator).currentTime += kDeltaTime;//経過時間を足す

			Matrix4x4 scaleMatrix = MakeScaleMatrix((*particleIterator).transform.scale);
			Matrix4x4 translateMatrix = MakeTranslateMatrix((*particleIterator).transform.translate);

			//ワールド行列を計算
			Matrix4x4 worldMatrix = scaleMatrix * billboardMatrix * translateMatrix;

			//ワールドビュープロジェクション行列を合成
			Matrix4x4 worldViewProjectionMatrixParticle = Multiply(Multiply(worldMatrix, viewMatrix), projectionMatrix);


			float alpha = 1.0f - ((*particleIterator).currentTime / (*particleIterator).lifeTime);

			//インスタンシング用データ1個分の書き込み
			if (group.numInstance < kNumMaxInstance)
			{
				group.instancingData[group.numInstance].WVP = worldViewProjectionMatrixParticle;

				group.instancingData[group.numInstance].World = worldMatrix;
				group.instancingData[group.numInstance].color = (*particleIterator).color;
				group.instancingData[group.numInstance].color.w = alpha;
				++group.numInstance;//生きているParticleの数を1つカウントする
			}


			++particleIterator;//次のイテレータに進める

		}

	}

	//全パーティクルグループ内の全パーティクルについて二重for文で処理する

}

void ParticleManager::Draw()
{
	dxCommon_->GetCommandList()->SetGraphicsRootSignature(rootSignature.Get());
	dxCommon_->GetCommandList()->SetPipelineState(graphicsPipelineState.Get());//PSOを設定
	dxCommon_->GetCommandList()->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	dxCommon_->GetCommandList()->IASetVertexBuffers(0, 1, &vertexBufferView);//VBVを設定
	for (auto& [groupName, group] : particleGroups)
	{
		dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(0,materialResource->GetGPUVirtualAddress());
		//インスタンシングデータのSRVのDescriptorTableを設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(1, srvManager_->GetGPUDescriptorHandle(group.instancingSrvIndex));
		//テクスチャのSRVのDescriptorTableを設定
		dxCommon_->GetCommandList()->SetGraphicsRootDescriptorTable(2, srvManager_->GetGPUDescriptorHandle(group.materialData.srvIndex));
		if (group.numInstance == 0) continue;
		//DrawCall(インスタンシング描画)
		dxCommon_->GetCommandList()->DrawInstanced(UINT(modelData.vertices.size()), group.numInstance, 0, 0);

	}
	
}

void ParticleManager::CreateRootSignature()
{

	//RootSignature作成
	D3D12_ROOT_SIGNATURE_DESC descriptionRootSignature{};
	descriptionRootSignature.Flags =
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;

	//DescriptorRange
	D3D12_DESCRIPTOR_RANGE descriptorRange[1] = {};
	descriptorRange[0].BaseShaderRegister = 0;//0から始まる
	descriptorRange[0].NumDescriptors = 1;//数は1つ
	descriptorRange[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRange[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;//offsetを自動計算

	D3D12_DESCRIPTOR_RANGE descriptorRangeForInstancing[1] = {};
	descriptorRangeForInstancing[0].BaseShaderRegister = 0;//0から始まる
	descriptorRangeForInstancing[0].NumDescriptors = 1;//数は1つ
	descriptorRangeForInstancing[0].RangeType = D3D12_DESCRIPTOR_RANGE_TYPE_SRV;//SRVを使う
	descriptorRangeForInstancing[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND;


	//Sampler
	D3D12_STATIC_SAMPLER_DESC staticSamplers[1] = {};
	staticSamplers[0].Filter = D3D12_FILTER_MIN_MAG_MIP_LINEAR;//バイリニアフィルタ
	staticSamplers[0].AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;//0～1の範囲外をリピート
	staticSamplers[0].AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	staticSamplers[0].ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;//比較しない
	staticSamplers[0].MaxLOD = D3D12_FLOAT32_MAX;//ありったけのMipmapを使う
	staticSamplers[0].ShaderRegister = 0;//レジスタ番号0を使う
	staticSamplers[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	descriptionRootSignature.pStaticSamplers = staticSamplers;
	descriptionRootSignature.NumStaticSamplers = _countof(staticSamplers);


	//RootParameter作成。複数設定できるので配列。PixelShaderのMaterialとVertexShaderのTransform
	D3D12_ROOT_PARAMETER rootParameters[4] = {};
	rootParameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[0].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//Pixelを使う
	rootParameters[0].Descriptor.ShaderRegister = 0;//レジスタ番号0とバインド

	rootParameters[1].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[1].ShaderVisibility = D3D12_SHADER_VISIBILITY_VERTEX;//VertexShaderで使う
	rootParameters[1].DescriptorTable.pDescriptorRanges = descriptorRangeForInstancing;
	rootParameters[1].DescriptorTable.NumDescriptorRanges = _countof(descriptorRangeForInstancing);

	rootParameters[2].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;//DescriptorTableを使う
	rootParameters[2].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShaderを使う
	rootParameters[2].DescriptorTable.pDescriptorRanges = descriptorRange;//Tableの中身の配列を指定
	rootParameters[2].DescriptorTable.NumDescriptorRanges = _countof(descriptorRange);//Tableで利用する数

	rootParameters[3].ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV;//CBVを使う
	rootParameters[3].ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;//PixelShdaderで使う
	rootParameters[3].Descriptor.ShaderRegister = 1;//レジスタ番号1を使う

	descriptionRootSignature.pParameters = rootParameters;//ルートパラメータ配列へのポインタ
	descriptionRootSignature.NumParameters = _countof(rootParameters);//配列の長さ

	//シリアライズしてバイナリにする
	Microsoft::WRL::ComPtr< ID3DBlob> signatureBlob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> errorBlob = nullptr;
	HRESULT hr = D3D12SerializeRootSignature(&descriptionRootSignature,
		D3D_ROOT_SIGNATURE_VERSION_1, &signatureBlob, &errorBlob);

	std::ostream& logStream = std::cerr;

	if (FAILED(hr))
	{
		Log(logStream, reinterpret_cast<char*>(errorBlob->GetBufferPointer()));
		assert(false);
	}

	//バイナリを元に生成

	hr = dxCommon_->GetDevice()->CreateRootSignature(0,
		signatureBlob->GetBufferPointer(), signatureBlob->GetBufferSize(),
		IID_PPV_ARGS(&rootSignature));
	assert(SUCCEEDED(hr));

}

void ParticleManager::CreateGraphicsPipeline()
{

	CreateRootSignature();

	//InputLayout
	D3D12_INPUT_ELEMENT_DESC inputElementDescs[3] = {};
	inputElementDescs[0].SemanticName = "POSITION";
	inputElementDescs[0].SemanticIndex = 0;
	inputElementDescs[0].Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	inputElementDescs[0].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[1].SemanticName = "TEXCOORD";
	inputElementDescs[1].SemanticIndex = 0;
	inputElementDescs[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	inputElementDescs[1].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	inputElementDescs[2].SemanticName = "NORMAL";
	inputElementDescs[2].SemanticIndex = 0;
	inputElementDescs[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	inputElementDescs[2].AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT;
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc{};
	inputLayoutDesc.pInputElementDescs = inputElementDescs;
	inputLayoutDesc.NumElements = _countof(inputElementDescs);

	//BlendStateの設定
	D3D12_BLEND_DESC blendDesc{};
	//すべての色要素を書き込む

	blendDesc.RenderTarget[0].RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL;
	blendDesc.RenderTarget[0].BlendEnable = TRUE;
	blendDesc.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
	blendDesc.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlend = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
	blendDesc.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
	blendDesc.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_ZERO;

	//RasterizerStateの設定
	D3D12_RASTERIZER_DESC rasterizerDesc{};
	//裏面（時計回り）を表示しない
	rasterizerDesc.CullMode = D3D12_CULL_MODE_BACK;
	//rasterizerDesc.CullMode = D3D12_CULL_MODE_NONE;//裏面あり
	//三角形の中を塗りつぶす
	rasterizerDesc.FillMode = D3D12_FILL_MODE_SOLID;

	//Shaderをコンパイルする
	Microsoft::WRL::ComPtr<IDxcBlob> vertexShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Particle.VS.hlsl",
		L"vs_6_0");
	assert(vertexShaderBlob != nullptr);

	Microsoft::WRL::ComPtr<IDxcBlob> pixelShaderBlob = dxCommon_->CompileShader(L"resources/shaders/Particle.PS.hlsl",
		L"ps_6_0");
	assert(pixelShaderBlob != nullptr);


	//DepthStencilStateの設定
	D3D12_DEPTH_STENCIL_DESC depthStencilDesc{};
	//Depthの機能を有効化する
	depthStencilDesc.DepthEnable = false;
	//書き込みします
	//depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ALL;
	depthStencilDesc.DepthWriteMask = D3D12_DEPTH_WRITE_MASK_ZERO;//Depthの書き込みを行わない
	//比較関数はLessEqual。つまり、近ければ描画される
	depthStencilDesc.DepthFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;


	//PSOを生成する
	D3D12_GRAPHICS_PIPELINE_STATE_DESC graphicsPipelineStateDesc{};
	graphicsPipelineStateDesc.pRootSignature = rootSignature.Get();//RootSignature
	graphicsPipelineStateDesc.InputLayout = inputLayoutDesc;//InputLayout
	graphicsPipelineStateDesc.VS = { vertexShaderBlob->GetBufferPointer(),
	vertexShaderBlob->GetBufferSize() };//VertexShader
	graphicsPipelineStateDesc.PS = { pixelShaderBlob->GetBufferPointer(),
	pixelShaderBlob->GetBufferSize() };//PixelShader
	graphicsPipelineStateDesc.BlendState = blendDesc;//BlendState
	graphicsPipelineStateDesc.RasterizerState = rasterizerDesc;//RasterizerState
	//書き込むRTVの情報
	graphicsPipelineStateDesc.NumRenderTargets = 1;
	graphicsPipelineStateDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM_SRGB;
	//利用するトポロジ（形状）のタイプ。三角形
	graphicsPipelineStateDesc.PrimitiveTopologyType =
		D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
	//どのように画面に色を打ち込むかの設定（気にしなくていい）
	graphicsPipelineStateDesc.SampleDesc.Count = 1;
	graphicsPipelineStateDesc.SampleMask = D3D12_DEFAULT_SAMPLE_MASK;

	//DepthStencilの設定
	graphicsPipelineStateDesc.DepthStencilState = depthStencilDesc;
	graphicsPipelineStateDesc.DSVFormat = DXGI_FORMAT_D24_UNORM_S8_UINT;

	//実際に生成

	HRESULT hr = dxCommon_->GetDevice()->CreateGraphicsPipelineState(&graphicsPipelineStateDesc,
		IID_PPV_ARGS(&graphicsPipelineState));

	assert(SUCCEEDED(hr));

}

ParticleManager::Particle ParticleManager::MakeNewParticle(const Vector3& translate)
{

	std::uniform_real_distribution <float> distribution(-1.0f, 1.0f);
	Particle particle;
	particle.transform.scale = { 1.0f,1.0f,1.0f };
	particle.transform.rotate = { 0.0f,0.0f,0.0f };
	Vector3 randomTranslate{ distribution(randomEngine_),distribution(randomEngine_),distribution(randomEngine_) };
	particle.transform.translate = translate + randomTranslate;
	particle.velocity = { distribution(randomEngine_),distribution(randomEngine_),distribution(randomEngine_) };

	//色
	std::uniform_real_distribution<float> distColor(0.0f, 1.0f);
	particle.color = { distColor(randomEngine_),distColor(randomEngine_) ,distColor(randomEngine_),1.0f };

	//生存時間
	std::uniform_real_distribution<float> distTime(1.0f, 3.0f);
	particle.lifeTime = distTime(randomEngine_);
	particle.currentTime = 0;

	return particle;

}

void ParticleManager::CreateParticleGroup(const std::string name,const std::string textureFilePath)
{

	//登録済みの名前かチェックしてassert
	assert(!name.empty());

	//新たな空のパーティクルグループを作成し、コンテナに登録
	ParticleManager::ParticleGroup particleGroup{};

	//新たなパーティクルグループの
	//マテリアルデータにテクスチャファイルパスを設定
	particleGroup.materialData.textureFilePath = textureFilePath;
	//テクスチャを読み込む(事前にやっておいても良い)
	TextureManager::GetInstance()->LoadTexture(textureFilePath);
	// マテリアルデータにテクスチャのSRVインデックスを記録
	particleGroup.materialData.srvIndex = TextureManager::GetInstance()->GetSrvIndex(textureFilePath);
	// インスタンシング用リソースの生成

	particleGroup.instancingSrvIndex = srvManager_->Allocate();

	//Instancing用のParticleForGPUリソースを作る
	particleGroup.instancingResource =
		dxCommon_->CreateBufferResource(sizeof(ParticleForGPU) * kNumMaxInstance);
	//書き込むためのアドレスを取得

	particleGroup.instancingResource->Map(0, nullptr, reinterpret_cast<void**>(&particleGroup.instancingData));
	//単位行列を書き込んでおく
	for (uint32_t index = 0; index < kNumMaxInstance; ++index)
	{
		particleGroup.instancingData[index].WVP = MakeIdentity4x4();
		particleGroup.instancingData[index].World = MakeIdentity4x4();
		particleGroup.instancingData[index].color = { 1.0f, 1.0f, 1.0f, 0.0f };

	}
	// インスタンシング用にSRVを確保してSRVインデックスを記録
	D3D12_SHADER_RESOURCE_VIEW_DESC instancingSrvDesc{};

	instancingSrvDesc.Format = DXGI_FORMAT_UNKNOWN;
	instancingSrvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
	instancingSrvDesc.ViewDimension = D3D12_SRV_DIMENSION_BUFFER;
	instancingSrvDesc.Buffer.FirstElement = 0;
	instancingSrvDesc.Buffer.Flags = D3D12_BUFFER_SRV_FLAG_NONE;
	instancingSrvDesc.Buffer.NumElements = kNumMaxInstance;
	instancingSrvDesc.Buffer.StructureByteStride = sizeof(ParticleForGPU);
	D3D12_CPU_DESCRIPTOR_HANDLE instancingSrvHandleCPU = srvManager_->GetCPUDescriptorHandle(particleGroup.instancingSrvIndex);
	D3D12_GPU_DESCRIPTOR_HANDLE instancingSrvHandleGPU = srvManager_->GetGPUDescriptorHandle(particleGroup.instancingSrvIndex);
	dxCommon_->GetDevice()->CreateShaderResourceView(particleGroup.instancingResource.Get(), &instancingSrvDesc, instancingSrvHandleCPU);

	particleGroups[name] = std::move(particleGroup);


}

void ParticleManager::Emit(const std::string name, const Vector3& position, uint32_t count)
{

	//登録済みの名前かチェックしてassert
	assert(!name.empty());

	//新たなパーティクルを作成し、指定されたパーティクルグループに登録

	auto it = particleGroups.find(name);
	assert(it != particleGroups.end()); // グループ未作成なら止める

	// マップからグループの参照を取得
	ParticleGroup& group = it->second;

	// 指定された数（count）だけパーティクルを生成して追加
	for (uint32_t i = 0; i < count; ++i)
	{
		// 新たなパーティクルを作成
		// MakeNewParticle内部で座標(position)にランダムなオフセットや速度が付与される
		Particle newParticle = MakeNewParticle(position);

		// 指定されたパーティクルグループのリストに登録
		group.particles.push_back(newParticle);
	}

}

bool ParticleManager::IsCollision(const AABB& aabb, const Vector3& point)
{

	return (point.x >= aabb.min.x && point.x <= aabb.max.x) &&
		(point.y >= aabb.min.y && point.y <= aabb.max.y) &&
		(point.z >= aabb.min.z && point.z <= aabb.max.z);

}


void  ParticleManager::Finalize()
{

	// 1. パーティクルグループごとのリソース解放
	// std::unordered_mapの中に格納された各グループが持っているバッファを解放します
	for (auto& [name, group] : particleGroups)
	{
		group.instancingResource.Reset();
	}
	particleGroups.clear();

	// 2. 基本リソースの解放
	// これらを解放し忘れると "Live Object" エラーになります
	vertexResource.Reset();
	materialResource.Reset();
	graphicsPipelineState.Reset();
	rootSignature.Reset();

	// 3. 外部ポインタの無効化
	dxCommon_ = nullptr;
	srvManager_ = nullptr;
	camera_ = nullptr;

}