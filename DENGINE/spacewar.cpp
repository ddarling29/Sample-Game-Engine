#include "spacewar.h"

Spacewar::Spacewar()
{}

Spacewar::~Spacewar()
{
	releaseAll();
}

void Spacewar::initialize(HWND hwnd)
{
	Game::initialize(hwnd);
	if (!nebulaTexture.initialize(graphics, NEBULA_IMAGE))
		throw (GameError(gameErrorNS::FATAL_ERROR, "Error initializing nebula texture"));

	if (!gameTextures.initialize(graphics, TEXTURES_IMAGE))
		throw (GameError(gameErrorNS::FATAL_ERROR, "Error initializing game textures"));

	if (!nebula.initialize(graphics, 0, 0, 0, &nebulaTexture))
		throw (GameError(gameErrorNS::FATAL_ERROR, "Error initializing nebula"));

	if (!planet.initialize(this, planetNS::WIDTH, planetNS::HEIGHT, planetNS::TEXTURE_COLS, &gameTextures))
		throw (GameError(gameErrorNS::FATAL_ERROR, "Error initializing planet"));

	planet.setX(GAME_WIDTH*0.5f - planet.getWidth()*0.5f);
	planet.setY(GAME_HEIGHT*0.5f - planet.getHeight()*0.5f);

	if (!ship1.initialize(this, shipNS::WIDTH, shipNS::HEIGHT, shipNS::TEXTURE_COLS, &gameTextures))
		throw (GameError(gameErrorNS::FATAL_ERROR, "Error initializing ship"));

	ship1.setFrames(shipNS::SHIP1_START_FRAME, shipNS::SHIP1_END_FRAME);
	ship1.setCurrentFrame(shipNS::SHIP1_START_FRAME);
	ship1.setX(GAME_WIDTH / 4 - shipNS::WIDTH);
	ship1.setY(GAME_HEIGHT / 2 - shipNS::HEIGHT);
	ship1.setVelocity(VECTOR2(shipNS::SPEED, -shipNS::SPEED));

	return;
}

void Spacewar::update()
{

	planet.update(frameTime);
	ship1.gravityForce(&planet, frameTime);
	ship1.update(frameTime);
}

void Spacewar::ai()
{
}

void Spacewar::collisions()
{
	VECTOR2 collisionVector;

	if (ship1.collidesWith(planet, collisionVector))
	{
		ship1.bounce(collisionVector, planet);
		ship1.damage(PLANET);
		audio->playCue(HIT);
	}
}

void Spacewar::render()
{
	graphics->spriteBegin();
	nebula.draw();
	planet.draw();
	ship1.draw();
	graphics->spriteEnd();
}

void Spacewar::releaseAll()
{
	
	gameTextures.onLostDevice();
	nebulaTexture.onLostDevice();
	Game::releaseAll();
	return;
}

void Spacewar::resetAll()
{
	gameTextures.onResetDevice();
	nebulaTexture.onResetDevice();
	Game::resetAll();
	return;
}
