#ifndef _ENTITY_H
#define _ENTITY_H
#define WIN32_LEAN_AND_MEAN

#include "image.h"
#include "input.h"
#include "game.h"
#include "audio.h"

namespace entityNS
{
	enum COLLISION_TYPE { NONE, CIRCLE, BOX, ROTATED_BOX };
	const float GRAVITY = 6.67428e-11f;
}

class Entity : public Image
{
protected:
	Audio *audio;
	entityNS::COLLISION_TYPE collisionType;
	VECTOR2 center;
	float radius;
	VECTOR2 distSquared;
	float sumRadiiSquared;
	RECT edge;
	VECTOR2 corners[4];
	VECTOR2 edge01, edge03;
	float edge01Min, edge01Max, edge03Min, edge03Max;
	VECTOR2 velocity;
	VECTOR2 deltaV;
	float mass;
	float health;
	float rr;
	float force;
	float gravity;
	Input *input; 
	HRESULT hr;
	bool active;
	bool rotatedBoxReady;

	virtual bool collideCircle(Entity &ent, VECTOR2 &collisionVector);

	virtual bool collideBox(Entity &ent, VECTOR2 &collisionVector);

	virtual bool collideRotatedBox(Entity &ent, VECTOR2 &collisionVector);

	virtual bool collideRotatedBoxCircle(Entity &ent, VECTOR2 &collisionVector);

	void computeRotatedBox();

	bool projectionsOverlap(Entity &ent);

	bool collideCornerCircle(VECTOR2 corner, Entity &ent, VECTOR2 &collisionVector);

public:
	
	Entity();
	
	virtual ~Entity();

	virtual const VECTOR2* getCenter()
	{
		center = VECTOR2(getCenterX(), getCenterY());
		return &center;
	}

	virtual float getRadius() const { return radius; }

	virtual const RECT& getEdge() const { return edge; }

	virtual const VECTOR2* getCorner(UINT c) const
	{
		if (c >= 4)
			c = 0;
		return &corners[c];
	}

	virtual const VECTOR2 getVelocity() const { return velocity; }

	virtual bool  getActive() const { return active; }

	virtual float getMass() const { return mass; }

	virtual float getGravity() const { return gravity; }

	virtual float getHealth() const { return health; }

	virtual entityNS::COLLISION_TYPE getCollisionType() { return collisionType; }

	virtual void  setVelocity(VECTOR2 v) { velocity = v; }

	virtual void  setDeltaV(VECTOR2 dv) { deltaV = dv; }

	virtual void  setActive(bool a) { active = a; }

	virtual void setHealth(float h) { health = h; }

	virtual void  setMass(float m) { mass = m; }

	virtual void  setGravity(float g) { gravity = g; }

	virtual void setCollisionRadius(float r) { radius = r; }

	virtual void update(float frameTime);

	virtual bool initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM);

	virtual void activate();

	virtual void ai(float frameTime, Entity &ent);

	virtual bool outsideRect(RECT rect);

	virtual bool collidesWith(Entity &ent, VECTOR2 &collisionVector);

	virtual void damage(int weapon);

	void bounce(VECTOR2 &collisionVector, Entity &ent);

	void gravityForce(Entity *other, float frameTime);
};

#endif