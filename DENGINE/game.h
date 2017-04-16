#ifndef _GAME_H
#define _GAME_H
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <Mmsystem.h>
#include "graphics.h"
#include "input.h"
#include "gameError.h"
#include "constants.h"
#include "audio.h"
#include "text.h"
#include "textdx.h"

namespace gameNS
{
	const char FONT[] = "Courier New";
	const int POINT_SIZE = 14;
	const COLOR_ARGB FONT_COLOR = SETCOLOR_ARGB(255, 255, 255, 255);
}

class Game

{
protected:
	Graphics *graphics;
	TextDX dxFont;
	Input *input;
	Audio *audio;
	HWND hwnd;
	HRESULT hr;
	LARGE_INTEGER timeStart;
	LARGE_INTEGER timeEnd;
	LARGE_INTEGER timerFreq;
	float frameTime;
	float fps;
	DWORD sleepTime;
	bool paused;
	bool initialized;
	bool fpsOn;

public:
	Game();

	virtual ~Game();

	LRESULT messageHandler(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

	virtual void initialize(HWND hwnd);

	virtual void run(HWND);

	virtual void releaseAll();

	virtual void resetAll();

	virtual void deleteAll();

	virtual void renderGame();

	virtual void handleLostGraphicsDevice();

	Graphics* getGraphics(){ return graphics; }

	Input* getInput(){ return input; }

	Audio* getAudio(){ return audio; }

	void exitGame();

	void setDisplayMode(graphicsNS::DISPLAY_MODE mode = graphicsNS::TOGGLE);

	virtual void update() = 0;

	virtual void ai() = 0;

	virtual void collisions() = 0;

	virtual void render() = 0;

};

#endif