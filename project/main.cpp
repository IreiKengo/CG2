#include <Windows.h>
#include <cstdint>
#include <string>
#include <format>
#include <filesystem>
#include <fstream>
#include <chrono>
#include <dbghelp.h>
#include <strsafe.h>
#include <dxgidebug.h>


#include <sstream>

#include "D3DResourceLeakChecker.h"

#include "vector"
#include <numbers>


#include "Input.h"
#include "WinApp.h"
#include "DirectXCommon.h"
#include "Logger.h"
#include "StringUtility.h"
#include "SpriteCommon.h"
#include "Sprite.h"
#include "TextureManager.h"
#include "Object3dCommon.h"
#include "Object3d.h"
#include "ModelCommon.h"
#include "Model.h"
#include"ModelManager.h"
#include "Camera.h"
#include "SrvManager.h"
#include "ParticleManager.h"
#include "ParticleEmitter.h"

#include "ImguiManger.h"
#include "Sound.h"

#include "Transform.h"
#include "Vector2.h"
#include "Vector3.h"
#include "Vector4.h"
#include"Matrix4x4.h"
#include "Matrix4x4Math.h"

using namespace StringUtility;
using namespace Logger;
using namespace math;


#pragma comment(lib,"Dbghelp.lib")



static LONG WINAPI ExportDump(EXCEPTION_POINTERS* exception)
{
	SYSTEMTIME time;
	GetLocalTime(&time);
	wchar_t filePath[MAX_PATH] = { 0 };
	CreateDirectory(L"./Dumps", nullptr);
	StringCchPrintfW(filePath, MAX_PATH, L"./Dumps/%04d-%02d%02d-%02d%02d.dmp", time.wYear, time.wMonth, time.wDay, time.wHour, time.wMinute);
	HANDLE dumpFileHandle = CreateFile(filePath, GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE | FILE_SHARE_READ, 0, CREATE_ALWAYS, 0, 0);
	//processId(このexeのId)とクラッシュ(例外)の発生したthreadIdを取得
	DWORD processId = GetCurrentProcessId();
	DWORD threadId = GetCurrentThreadId();
	//設定情報を入力
	MINIDUMP_EXCEPTION_INFORMATION minidumpInformation{ 0 };
	minidumpInformation.ThreadId = threadId;
	minidumpInformation.ExceptionPointers = exception;
	minidumpInformation.ClientPointers = TRUE;
	//Dumpを入力。MiniDumpNormalは最低限の情報を出力するフラグ
	MiniDumpWriteDump(GetCurrentProcess(), processId, dumpFileHandle, MiniDumpNormal, &minidumpInformation, nullptr, nullptr);
	//他に関連づけられているSEH例外ハンドラがあれば実行。通常はプロセスを終了する

	return EXCEPTION_EXECUTE_HANDLER;
}


#pragma endregion

//Windowsアプリでのエントリーポイント(main関数)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{

	D3DResourceLeakChecker leakCheck;

	SetUnhandledExceptionFilter(ExportDump);

	//log出力用のフォルダ「logs」を作成
	std::filesystem::create_directory("logs");

	//ここからファイルを作成し、ofstreamを取得する
	//現時刻を取得
	std::chrono::system_clock::time_point now = std::chrono::system_clock::now();
	//ログファイルの名前にコンマ何秒はいらないので、削って秒にする
	std::chrono::time_point<std::chrono::system_clock, std::chrono::seconds>
		nowSeconds = std::chrono::time_point_cast<std::chrono::seconds>(now);
	//日本時間に変換
	std::chrono::zoned_time localTime{ std::chrono::current_zone(),nowSeconds };
	//formatを使って年月日_時分秒の文字列に変換
	std::string dateString = std::format("{:%Y%m%d_%H%M%S}", localTime);
	//時刻を使ってファイル名を決定
	std::string logFilePath = std::format("logs/") + dateString + ".log";
	//ファイルを使って書き込み準備
	std::ofstream logStream(logFilePath);


#pragma region WindowsAPIの初期化

	//ポインタ
	WinApp* winApp = nullptr;

	//WindowsAPIの初期化
	winApp = new WinApp();
	winApp->Initialize();


#pragma endregion

	Log("Hello DirectX!\n");
	Log(
		ConvertString(
			std::format(
				L"clientSize:{},{}\n",
				WinApp::kClientWidth,
				WinApp::kClientHeight
			)
		)
	);


	HRESULT result;


#pragma region DirectInputの初期化

	Input* input = nullptr;

	//入力の初期化
	input = new Input();
	input->Initialize(winApp);

#pragma endregion

#pragma region カメラの初期化
	Camera* camera = new Camera();
	camera->SetRotate({ 0.0f,0.0f,0.0f });
	camera->SetTranslate({ 0.0f,0.0f,-10.0f });
#pragma endregion 

#pragma region DirectXの初期化

	//ポインタ
	DirectXCommon* dxCommon = nullptr;

	//DirectXの初期化
	dxCommon = new DirectXCommon();
	dxCommon->Initialize(winApp);

	//SpriteCommonの初期化
	SpriteCommon* spriteCommon = nullptr;
	//スプライト共通部の初期化
	spriteCommon = new SpriteCommon;
	spriteCommon->Initialize(dxCommon);


	//3Dオブジェクト共通部
	Object3dCommon* object3dCommon = nullptr;
	//3Dオブジェクト共通部の初期化
	object3dCommon = new Object3dCommon;
	object3dCommon->SetDefaultCamera(camera);
	object3dCommon->Initialize(dxCommon);

	SrvManager* srvManager = nullptr;
	//SRVマネージャーの初期化
	srvManager = new SrvManager();
	srvManager->Initialize(dxCommon);

	//テクスチャマネージャの初期化
	TextureManager::GetInstance()->Initialize(dxCommon,srvManager);

#pragma endregion

#pragma region Sound
	
	Sound* sound = new Sound();

	sound->Initialize("resources/fanfare.wav");

#pragma endregion

#pragma region マネージャの初期化


	//3Dモデルマネージャの初期化
	ModelManager::GetInstance()->Initialize(dxCommon);


#pragma endregion

#pragma region スプライトの初期化

	std::vector<Sprite*> sprites;
	for (uint32_t i = 0; i < 1; ++i)
	{
		Sprite* sprite = new Sprite();

		/*std::string texturePath;
		if (i % 2 == 0) {
			texturePath = "resources/uvChecker.png";
		} else {

			texturePath = "resources/monsterBall.png";
		}*/

		sprite->Initialize(spriteCommon, "resources/uvChecker.png");
		sprites.push_back(sprite);
	}


#pragma endregion

#pragma region 最初のシーンの初期化

	//.objファイルからモデルを読み込む
	ModelManager::GetInstance()->LoadModel("plane.obj");
	ModelManager::GetInstance()->LoadModel("multiMesh.obj");
	ModelManager::GetInstance()->LoadModel("multiMaterial.obj");
	ModelManager::GetInstance()->LoadModel("axis.obj");

	std::vector<Object3d*> objects;
	for (int i = 0; i < 2; ++i)
	{
		Object3d* object3d = new Object3d();

		std::string modelPath;
		if (i % 2 == 0) {
			modelPath = "plane.obj";
		} else {

			modelPath = "axis.obj";
		}

		object3d->Initialize(object3dCommon);
		object3d->SetModel(modelPath);
		objects.push_back(object3d);
	}




#pragma endregion

	ParticleManager::GetInstance()->Initialize(dxCommon, srvManager);
	ParticleManager::GetInstance()->SetCamera(camera);

	ParticleManager::GetInstance()->CreateParticleGroup(
		"circle",
		"resources/circle.png"
	);

	ParticleEmitter* particleCircle = new ParticleEmitter
	(
		"circle",
		Vector3{ 0, 0, 0 },
		5,
		0.1f
	);

	ParticleManager::GetInstance()->CreateParticleGroup(
		"uvChecker",              //新しい名前にする
		"resources/uvChecker.png" //使いたい画像のパス
	);

	
	ParticleEmitter* particleChecker = new ParticleEmitter(
		"uvChecker",             
		Vector3{ 2.0f, 0, 0 },    // 位置を少しずらすと見やすいです
		5,                        // 発生数
		0.1f                      // 発生頻度
	);

#pragma region Lighting



	////平行光源の切り替え	
	bool useLighting = false;
#pragma endregion

#pragma region Imguiの初期化

	ImguiManager* imgui = new ImguiManager();

	imgui->Initialize(winApp,dxCommon,srvManager);

#pragma endregion

		Log("文字列リテラルを出力するよ\n");
		std::string a("stringに埋め込んだ文字列を出力するよ\n");
		Log(a.c_str());


	//ウィンドウの×ボタンが押されるまでループ
	while (true)
	{
		//winApp->ProcessMessage();

		//Windowsのメッセージ処理
		if (winApp->ProcessMessage())
		{
			//ゲームループを抜ける
			break;
		}

		imgui->Begin();

		camera->DebugUpdate();

		sprites[0]->DebugUpdate();


		//ImGui::ShowDemoWindow();
		imgui->End();

		/////////// Update /////////////


		//入力の更新
		input->Update();

		if (input->TriggerKey(DIK_W))
		{
			sound->SoundPlayWave();
		}


		//カメラの更新
		camera->Update();

		for (uint32_t i = 0; i < sprites.size(); ++i)
		{
			sprites[i]->Update();
		}

		objects[0]->SetTranslate({ -1.0f,-1.0f,0.0f });
		objects[1]->SetTranslate({ 1.0f,-1.0f,0.0f });
		for (uint32_t i = 0; i < objects.size(); ++i)
		{
			objects[i]->Update();
		}

		float deltaTime = 1.0f / 60.0f; // 本来は実時間計測



		//particleCircle->Update(deltaTime);
		//particleChecker->Update(deltaTime);
		ParticleManager::GetInstance()->Update();


		/////////// Draw /////////////


		//DirectXの描画基準。全ての描画に共通宇のグラッフィックスコマンドを積む
		dxCommon->PreDraw();
		srvManager->PreDraw();

		//3Dオブジェクトの描画準備。3Dオブジェクトの描画に共通のグラフィックスコマンドを積む
		object3dCommon->ScreenCommon();


		//全てのObject3d個々の描画
		for (uint32_t i = 0; i < objects.size(); ++i)
		{
			//objects[i]->Draw();

		}

		//Spriteの描画基準。Spriteの描画の共通のグラッフィックスコマンドを積む
		spriteCommon->ScreenCommon();


		//全てのSprite個々の描画
		for (uint32_t i = 0; i < sprites.size(); ++i)

		{
			sprites[0]->Draw();
		}

		ParticleManager::GetInstance()->Draw();


		imgui->Draw();

		//描画後処理
		dxCommon->PostDraw();

		TextureManager::GetInstance()->ReleaseIntermediateResources();

	}

	CloseHandle(dxCommon->GetFenceEvent());

	imgui->Finalize();
	

	

	

	delete particleChecker;
	particleChecker = nullptr;

	delete particleCircle;
	particleCircle = nullptr;

	ParticleManager::GetInstance()->Finalize();

	delete camera;

	for (uint32_t i = 0; i < objects.size(); ++i)
	{
		delete objects[i];
	}


	//Sprite解放
	for (uint32_t i = 0; i < sprites.size(); ++i)
	{

		delete sprites[i];
	}

	//Dモデルマネージャの終了
	ModelManager::GetInstance()->Finalize();

	//TextureManager解放
	TextureManager::GetInstance()->Finalize();

	delete srvManager;

	delete object3dCommon;

	//SpriteCommon解放
	delete spriteCommon;

	//DirectX解放
	delete dxCommon;

	//入力解放
	delete input;

	//WindowsAPIの終了処理
	winApp->Finilize();

	//WindowsAPIの解放
	delete winApp;

	return 0;
}

