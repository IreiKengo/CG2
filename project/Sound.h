#pragma once
#include "xaudio2.h"
#include <fstream>
#include <wrl.h>

class Sound
{

public:

	//チャンクヘッダ
	struct ChunkHeader
	{
		char id[4];//チャンク毎のID
		int32_t size;//チャンクサイズ
	};

	//RIFFヘッダチャンク
	struct RiffHeader
	{
		ChunkHeader chunk;//"RIFF"
		char type[4];//"WAVE"
	};

	//FMTチャンク
	struct FormatChunk
	{
		ChunkHeader chunk;//"fmt"
		WAVEFORMATEX fmt;//波形フォーマット
	};

	//音声データ
	struct SoundData
	{
		//波形フォーマット
		WAVEFORMATEX wfex;
		//バッファの先頭アドレス
		std::unique_ptr<BYTE[]> pBuffer;
		//バッファのサイズ
		unsigned int bufferSize;
	};

	~Sound();

	//音声再生
	void SoundPlayWave();

	void Initialize(const char* filename);

private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;

	SoundData soundData;
	IXAudio2SourceVoice* sourceVoice = nullptr;

	//音声読み込み
	SoundData SoundLoadWave(const char* filename);
};
