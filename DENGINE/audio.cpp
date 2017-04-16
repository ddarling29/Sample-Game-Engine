#include "audio.h"

Audio::Audio()
{
	xactEngine = NULL;
	waveBank = NULL;
	soundBank = NULL;
	cueI = 0;
	mapWaveBank = NULL;         
	soundBankData = NULL;

	HRESULT hr = CoInitializeEx(NULL, COINIT_MULTITHREADED);
	if (SUCCEEDED(hr))
		coInitialized = true;
	else
		coInitialized = false;
}

Audio::~Audio()
{
	if (xactEngine)
	{
		xactEngine->ShutDown(); 
		xactEngine->Release();
	}

	if (soundBankData)
		delete[] soundBankData;
	soundBankData = NULL;

	if (mapWaveBank)
		UnmapViewOfFile(mapWaveBank);
	mapWaveBank = NULL;

	if (coInitialized)        
		CoUninitialize();
}

HRESULT Audio::initialize()
{
	HRESULT result = E_FAIL;
	HANDLE hFile;
	DWORD fileSize;
	DWORD bytesRead;
	HANDLE hMapFile;

	if (coInitialized == false)
		return E_FAIL;

	result = XACT3CreateEngine(0, &xactEngine);
	if (FAILED(result) || xactEngine == NULL)
		return E_FAIL;

	XACT_RUNTIME_PARAMETERS xactParams = { 0 };
	xactParams.lookAheadTime = XACT_ENGINE_LOOKAHEAD_DEFAULT;
	result = xactEngine->Initialize(&xactParams);
	if (FAILED(result))
		return result;

	result = E_FAIL;

	hFile = CreateFile(WAVE_BANK, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);
	if (hFile != INVALID_HANDLE_VALUE)
	{
		fileSize = GetFileSize(hFile, NULL);
		if (fileSize != -1)
		{
			hMapFile = CreateFileMapping(hFile, NULL, PAGE_READONLY, 0, fileSize, NULL);
			if (hMapFile)
			{
				mapWaveBank = MapViewOfFile(hMapFile, FILE_MAP_READ, 0, 0, 0);

				if (mapWaveBank)
					result = xactEngine->CreateInMemoryWaveBank(mapWaveBank, fileSize, 0, 0, &waveBank);
				CloseHandle(hMapFile);
			}
		}
	}
	
	if (FAILED(result))
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);

	result = E_FAIL;
	hFile = CreateFile(SOUND_BANK, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 0, NULL);

	if (hFile != INVALID_HANDLE_VALUE)
	{
		fileSize = GetFileSize(hFile, NULL);
		if (fileSize != -1)
		{
			soundBankData = new BYTE[fileSize];

			if (soundBankData)
			{
				if (0 != ReadFile(hFile, soundBankData, fileSize, &bytesRead, NULL))
					result = xactEngine->CreateSoundBank(soundBankData, fileSize, 0, 0, &soundBank);
			}
		}
		CloseHandle(hFile);
	}
	if (FAILED(result))
		return HRESULT_FROM_WIN32(ERROR_FILE_NOT_FOUND);
	return S_OK;
}

void Audio::playCue(const char cue[])
{
	if (soundBank == NULL)
		return;
	cueI = soundBank->GetCueIndex(cue);
	soundBank->Play(cueI, 0, 0, NULL);
}

void Audio::stopCue(const char cue[])
{
	if (soundBank == NULL)
		return;
	cueI = soundBank->GetCueIndex(cue);
	soundBank->Stop(cueI, XACT_FLAG_SOUNDBANK_STOP_IMMEDIATE);
}

void Audio::run()
{
	if (xactEngine == NULL)
		return;
	xactEngine->DoWork();
}