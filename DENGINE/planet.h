#ifndef _PANET_H
#define _PLANET_H
#define WIN32_LEAN_AND_MEN

#include "entity.h"
#include "constants.h"

namespace planetNS
{
	const int WIDTH = 128;                
	const int HEIGHT = 128;               
	const int COLLISION_RADIUS = 120 / 2;   
	const int X = GAME_WIDTH / 2 - WIDTH / 2; 
	const int Y = GAME_HEIGHT / 2 - HEIGHT / 2;
	const float MASS = 1.0e14f;             
	const int TEXTURE_COLS = 2;           
	const int START_FRAME = 1;            
	const int END_FRAME = 1;
}

class Planet : public Entity
{
public:
	Planet();
};

#endif