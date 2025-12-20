#pragma once
#include<string>
#include <wrl.h>
#include <d3d12.h>
#include "Transform.h"
#include "Vector3.h"
#include "Vector4.h"
#include "Matrix4x4Math.h"


class Object3dCommon;
class DirectXCommon;
class Model;
class Camera;

//3Dオブジェクト
class Object3d
{

public:




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


	//setter
	void SetModel(Model* model) { this->model = model; }
	void SetScale(const Vector3& scale) { this->transform.scale = scale; }
	void SetRotate(const Vector3& rotate) { this->transform.rotate = rotate; }
	void SetTranslate(const Vector3& translate) { this->transform.translate = translate; }
	void SetModel(const std::string& filePath);
	void SetCamera(Camera* camera) { this->camera = camera; }
	

	//getter
	const Vector3& GetScale()const { return transform.scale; }
	const Vector3& GetRotate()const { return transform.rotate; }
	const Vector3& GetTranslate()const { return transform.translate; }
	


private:

	Object3dCommon* object3dCommon = nullptr;


	//バッファリソース
	Microsoft::WRL::ComPtr<ID3D12Resource> transformationMatrixResources;
	//バッファリソース内のデータを指すポインタ
	TransformationMatrix* transformationMatrixData = nullptr;

	//平行光源用のリソースを作る。今回はカラー１つ分のサイズを用意する
	Microsoft::WRL::ComPtr<ID3D12Resource> directionalLightResource;
	//データを書き込む
	DirectionalLight* directionalLightData = nullptr;

	Transform transform;

	DirectXCommon* dxCommon_ = nullptr;

	Model* model = nullptr;

	Camera* camera = nullptr;
	
	
	//座標返還行列データ作成
	void CreateTransformationMatrixData();
	//平行光源データ作成
	void CreateDirectionalLightData();

};