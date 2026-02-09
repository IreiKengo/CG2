#include "Sound.h"
#include <cassert>
#include <mfapi.h>
#include <mfobjects.h>
#include <mfidl.h>
#include <mfreadwrite.h>
#include"StringUtility.h"

#pragma comment(lib,"xaudio2.lib")
#pragma comment(lib,"mfplat.lib")
#pragma comment(lib,"Mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")

using namespace Microsoft::WRL;

void Sound::Initialize(const char* filename)
{

	

	//XAudioエンジンのインデックスを生成

	HRESULT result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	//マスターボイスを生成
	result = xAudio2->CreateMasteringVoice(&masterVoice);
	assert(SUCCEEDED(result));

	//Windows Media Foundationの初期化（ローカルファイル版）
	result = MFStartup(MF_VERSION, MFSTARTUP_NOSOCKET);
	assert(SUCCEEDED(result));

	//音声読み込み
	soundData = SoundLoadFile(filename);

	if (soundData.buffer.empty()) {
    OutputDebugString(L"AACファイルの読み込みに失敗しました！\n");
    
}

}

void Sound::Finalize()
{

	HRESULT result;

	if (sourceVoice) {
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}

	if (masterVoice) {
		masterVoice->DestroyVoice();
		masterVoice = nullptr;
	}

	xAudio2.Reset();


	//WindowsMedia Foundationの終了
	result = MFShutdown();
	assert(SUCCEEDED(result));

}

void Sound::SoundUnload()
{

	soundData.buffer.clear();
	soundData.wfex = {};

}

Sound::SoundData Sound::SoundLoadFile(const std::string& filename)
{

	//フルパスをワイド文字列に変換
	std::wstring filePathW = StringUtility::ConvertString(filename);
	HRESULT result;

	//SourceReader作成
	ComPtr<IMFSourceReader> pReader;
	result = MFCreateSourceReaderFromURL(filePathW.c_str(), nullptr, &pReader);
	assert(SUCCEEDED(result));

	//PCM形式にフォーマット指定する
	ComPtr<IMFMediaType> pPCMType;
	MFCreateMediaType(&pPCMType);
	pPCMType->SetGUID(MF_MT_MAJOR_TYPE, MFMediaType_Audio);
	pPCMType->SetGUID(MF_MT_SUBTYPE, MFAudioFormat_PCM);
	result = pReader->SetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM,nullptr, pPCMType.Get());
	assert(SUCCEEDED(result));

	//実際にセットされたメディアタイプを取得する
	ComPtr<IMFMediaType> pOutType;
	pReader->GetCurrentMediaType((DWORD)MF_SOURCE_READER_FIRST_AUDIO_STREAM, &pOutType);

	//Waveフォーマットを取得する
	WAVEFORMATEX* waveFormat = nullptr;
	MFCreateWaveFormatExFromMFMediaType(pOutType.Get(), &waveFormat, nullptr);

	//コンテナに格納する音声データ
	SoundData soundData = {};
	soundData.wfex = *waveFormat;

	//生成したWaveフォーマットを解放
	CoTaskMemFree(waveFormat);

	//PCMデータのバッファを構築
	while (true) {
		ComPtr<IMFSample> pSample;
		DWORD streamIndex = 0, flags = 0;
		LONGLONG llTimeStamp = 0;
		//サンプルを読み込む
		result = pReader->ReadSample(MF_SOURCE_READER_FIRST_AUDIO_STREAM, 0, &streamIndex, &flags, &llTimeStamp, &pSample);
		//ストリームの末尾に達したら抜ける
		if (flags & MF_SOURCE_READERF_ENDOFSTREAM) break;

		if (pSample) {
			ComPtr<IMFMediaBuffer> pBuffer;
			//サンプルに含まれるサウンドデータのバッファを一繋ぎにして取得
			pSample->ConvertToContiguousBuffer(&pBuffer);

			BYTE* pData = nullptr;//データ読み取り用ポインタ
			DWORD maxLength = 0, currentLength = 0;
			//バッファ読み込み用にロック
			pBuffer->Lock(&pData, &maxLength, &currentLength);
			//バッファの末尾にデータを追加
			soundData.buffer.insert(soundData.buffer.end(), pData, pData + currentLength);
			pBuffer->Unlock();
		}

	}
	return soundData;

	
}

void Sound::SoundPlayWave()
{
	HRESULT result;

	if (sourceVoice) {
		sourceVoice->Stop();
		sourceVoice->FlushSourceBuffers();
		sourceVoice->DestroyVoice();
		sourceVoice = nullptr;
	}


	//波形フォーマットを元にSoundVoiceを生成
	result = xAudio2->CreateSourceVoice(&sourceVoice, &soundData.wfex);
	assert(SUCCEEDED(result));

	//再生する波形データの設定
	XAUDIO2_BUFFER buf{};
	buf.pAudioData = soundData.buffer.data();
	buf.AudioBytes = static_cast<UINT32>(soundData.buffer.size());
	buf.Flags = XAUDIO2_END_OF_STREAM;

	//波形データの再生
	result = sourceVoice->SubmitSourceBuffer(&buf);
	result = sourceVoice->Start();

}


