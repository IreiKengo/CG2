#include "Sound.h"
#include <cassert>


#pragma comment(lib,"xaudio2.lib")


void Sound::Initialize(const char* filename)
{

	

	//XAudioエンジンのインデックスを生成

	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	//音声読み込み
	soundData = SoundLoadWave(filename);

}

Sound::SoundData Sound::SoundLoadWave(const char* filename)
{

	//①ファイルオープン

	//ファイル入力ストリームのインスタンス
	std::ifstream file;
	//.wavファイルをバイナリモードで開く
	file.open(filename, std::ios_base::binary);
	//ファイルオープン失敗を検出する
	assert(file.is_open());

	//②.wavデータ読み込み

	//RIFFヘッダーの読み込み
	RiffHeader riff;
	file.read((char*)&riff, sizeof(riff));
	//ファイルがRIFFかチェック
	if (strncmp(riff.chunk.id, "RIFF", 4) != 0)
	{
		assert(0);
	}
	//タイプがWAVEかチェック
	if (strncmp(riff.type, "WAVE", 4) != 0)
	{
		assert(0);
	}

	//Formatチャンクの読み込み
	FormatChunk format = {};
	//チャンクヘッダーの確認
	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0)
	{
		assert(0);
	}

	//チャンク本体の読み込み
	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	//Dataチャンクの読み込み
	ChunkHeader data;
	file.read((char*)&data, sizeof(data));
	//JUNKチャンクを検出した場合
	if (strncmp(data.id, "JUNK", 4) == 0)
	{
		//読み取り位置をJUNKチャンクの終わりまで進める
		file.seekg(data.size, std::ios_base::cur);
		//再読み込み
		file.read((char*)&data, sizeof(data));
	}

	if (strncmp(data.id, "data", 4) != 0)
	{
		assert(0);
	}


	// バッファ確保＆読み込み
	SoundData soundData = {};

	soundData.wfex = format.fmt;
	soundData.bufferSize = data.size;
	soundData.pBuffer = std::make_unique<BYTE[]>(data.size);
	file.read(reinterpret_cast<char*>(soundData.pBuffer.get()), data.size);
	file.close();

	return soundData;

}



Sound::~Sound()
{
	if (sourceVoice)
	{
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}

	if (masterVoice)
	{
		masterVoice->DestroyVoice();
		masterVoice = nullptr;
	}

	xAudio2.Reset();
}


void Sound::SoundPlayWave()
{
	HRESULT result;

	//波形フォーマットを元にSoundVoiceを生成
	result = xAudio2->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.pBuffer.get();
	buf.AudioBytes = soundData.bufferSize;
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = sourceVoice->SubmitSourceBuffer(&buf);
	result = sourceVoice->Start();

}


