#ifndef _SPACEWAR_H
#define _SPACEWAR_H
#define WIN32_LEAN_AND_MEAN
#include "game.h"
#include "textureManager.h"
#include "image.h"
#include "ship.h"
#include "planet.h"

class Spacewar : public Game
{
private:
	TextureManager nebulaTexture;
	TextureManager gameTextures;
	
	Image nebula;
	Ship ship1;
	Planet planet;
public:
	Spacewar();
	virtual ~Spacewar();
	void initialize(HWND hwnd);
	void update();
	void ai();
	void collisions();
	void render();
	void releaseAll();
	void resetAll();
};

#endif