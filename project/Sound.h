#pragma once
#include "xaudio2.h"
#include <fstream>
#include <wrl.h>
#include <vector>

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
		//バッファ
		std::vector<BYTE> buffer;
	};

	void Finalize();

	//音声再生
	void SoundPlayWave();

	void Initialize(const char* filename);

	//音声データ解放
	void SoundUnload();

private:

	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* masterVoice = nullptr;

	SoundData soundData;
	IXAudio2SourceVoice* sourceVoice = nullptr;

	//音声読み込み（wav,mp3,aacなど）
	SoundData SoundLoadFile(const std::string& filename);
};
