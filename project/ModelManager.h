#pragma once
#include <string>
#include <wrl.h>
#include <map>
#include <memory>



class Model;
class ModelCommon;
class DirectXCommon;

//テクスチャマネージャー
class ModelManager
{

public:

	//シングルトンインスタンスの取得
	static ModelManager* GetInstance();
	//終了
	void Finalize();

	void Initialize(DirectXCommon* dxCommon);

	//モデルファイル読み込み関数
	void LoadModel(const std::string& filePath);
	//モデルデータ取得関数
	Model* FindModel(const std::string& filePath);


private:
	static ModelManager* instance;
	//モデルデータ
	std::map<std::string, std::unique_ptr<Model>> models;

	ModelCommon* modelCommon = nullptr;

	ModelManager() = default;
	~ModelManager() = default;
	ModelManager(ModelManager&) = delete;
	ModelManager& operator=(ModelManager&) = default;

	

};
