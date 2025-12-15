#include "Object3d.h"
#include "Object3dCommon.h"
#include "Matrix4x4Math.h"
#include "TextureManager.h"
#include "Model.h"
#include "ModelManager.h"

using namespace math;


void Object3d::Initialize(Object3dCommon* object3dCommon)
{

	//引数で受け取ってメンバ変数に記録する
	this->object3dCommon = object3dCommon;
	
	dxCommon_ = object3dCommon->GetCommon();


	CreateTransformationMatrixData();

	CreateDirectionalLightData();


	

	//Transform変数を作る
	transform = { {1.0f,1.0f,1.0f},{0.0f,3.156f,0.0f},{0.0f,0.0f,0.0f} };
	cameraTransform = { {1.0f,1.0f,1.0f},{0.3f,0.0f,0.0f},{0.0f,4.0f,-10.0f} };


}

void Object3d::Update()
{

	transform.rotate.y += 0.03f;
	////三角形用のWVPMatrixの作成
	Matrix4x4 worldMatrix = MakeAffineMatrix(transform.scale, transform.rotate, transform.translate);
	Matrix4x4 cameraMatrix = MakeAffineMatrix(cameraTransform.scale, cameraTransform.rotate, cameraTransform.translate);
	Matrix4x4 viewMatrix = Inverse(cameraMatrix);
	Matrix4x4 projectionMatrix = MakePerspectiveFovMatrix(0.45f, float(WinApp::kClientWidth) / float(WinApp::kClientHeight), 0.1f, 100.0f);
	Matrix4x4 worldViewProjectionMatrix = Multiply(worldMatrix, Multiply(viewMatrix, projectionMatrix));
	transformationMatrixData->WVP = worldViewProjectionMatrix;
	transformationMatrixData->World = worldMatrix;



}

void Object3d::Draw()
{

	
	//座標変換行列CBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(1, transformationMatrixResources->GetGPUVirtualAddress());
	//平行光源CBufferの場所を設定
	dxCommon_->GetCommandList()->SetGraphicsRootConstantBufferView(3, directionalLightResource->GetGPUVirtualAddress());
	
	//3Dモデルが割り当てられていれば描画する
	if (model)
	{
		model->Draw();
	}


}

void Object3d::SetModel(const std::string& filePath)
{

	//モデルを検索してセットする
	model = ModelManager::GetInstance()->FindModel(filePath);

}


void Object3d::CreateTransformationMatrixData()
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

void Object3d::CreateDirectionalLightData()
{

	directionalLightResource = dxCommon_->CreateBufferResource(sizeof(DirectionalLight));

	//書き込むためのアドレスを取得
	directionalLightResource->Map(0, nullptr, reinterpret_cast<void**>(&directionalLightData));
	//デフォルト値はとりあえず以下のようにしておく
	directionalLightData->color = { 1.0f,1.0f,1.0f,1.0f };
	directionalLightData->direction = { 0.0f,-1.0f,0.0f };
	directionalLightData->intensity = 1.0f;

}
