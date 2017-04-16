#ifndef _SHIP_H
#define _SHIP_H
#define WIN32_LEAN_AND_MEAN

#include "entity.h"
#include "constants.h"

namespace shipNS
{
	const int WIDTH = 32;                  
	const int HEIGHT = 32;                  
	const int X = GAME_WIDTH / 2 - WIDTH / 2;   
	const int Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	const float ROTATION_RATE = (float)PI / 4; 
	const float SPEED = 100;                
	const float MASS = 300.0f;              
	const int   TEXTURE_COLS = 8;          
	const int   SHIP1_START_FRAME = 0;      
	const int   SHIP1_END_FRAME = 3;        
	const int   SHIP2_START_FRAME = 8;      
	const int   SHIP2_END_FRAME = 11;       
	const float SHIP_ANIMATION_DELAY = 0.2f;    
	const int   SHIELD_START_FRAME = 24;    
	const int   SHIELD_END_FRAME = 27;      
	const float SHIELD_ANIMATION_DELAY = 0.1f;
}

class Ship : public Entity
{
private:
	bool shieldOn;
	Image shield;

public:
	Ship();

	virtual void draw();

	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);

	void update(float frameTime);

	void damage(int weapon);
};

#endif