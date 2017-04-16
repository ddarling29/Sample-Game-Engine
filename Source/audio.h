#ifndef _AUDIO_H
#define _AUDIO_H
#define WIN32_LEAN_AND_MEAN

#include <xact3.h>
#include "constants.h"

class Audio
{
private:
	IXACT3Engine *xactEngine;
	IXACT3WaveBank* waveBank;  
	IXACT3SoundBank* soundBank; 
	XACTINDEX cueI;             
	void* mapWaveBank;          
	void* soundBankData;
	bool coInitialized;

public:
	Audio();

	virtual ~Audio();

	HRESULT initialize();

	void run();

	void playCue(const char cue[]);

	void stopCue(const char cue[]);
};
#endif