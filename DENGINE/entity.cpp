#include "entity.h"

Entity::Entity() : Image()
{
	radius = 1.0;
	edge.left = -1;
	edge.top = -1;
	edge.right = 1;
	edge.bottom = 1;
	mass = 1.0;
	velocity.x = 0.0;
	velocity.y = 0.0;
	deltaV.x = 0.0;
	deltaV.y = 0.0;
	active = true;                  
	rotatedBoxReady = false;
	collisionType = entityNS::CIRCLE;
	health = 100;
	gravity = entityNS::GRAVITY;
}

Entity::~Entity()
{

}

bool Entity::collideCircle(Entity &ent, VECTOR2 &collisionVector)
{
	distSquared = *getCenter() - *ent.getCenter();
	distSquared.x = distSquared.x * distSquared.x;
	distSquared.y = distSquared.y * distSquared.y;
	sumRadiiSquared = (radius*getScale()) + (ent.radius*ent.getScale());
	sumRadiiSquared *= (sumRadiiSquared);

	if (distSquared.x + distSquared.y <= sumRadiiSquared)
	{
		collisionVector = *ent.getCenter() - *getCenter();
		return true;
	}
	return false;
}

bool Entity::collideBox(Entity &ent, VECTOR2 &collisionVector)
{
	if (!active || !ent.getActive())
		return false;

	if ((getCenterX() + edge.right*getScale() >= ent.getCenterX() + ent.getEdge().left*ent.getScale()) &&
		(getCenterX() + edge.left*getScale() <= ent.getCenterX() + ent.getEdge().right*ent.getScale()) &&
		(getCenterY() + edge.bottom*getScale() >= ent.getCenterY() + ent.getEdge().top*ent.getScale()) &&
		(getCenterY() + edge.top*getScale() <= ent.getCenterY() + ent.getEdge().bottom*ent.getScale()))
	{
		collisionVector = *ent.getCenter() - *getCenter();
		return true;
	}
	return false;
}

bool Entity::collideRotatedBox(Entity &ent, VECTOR2 &collisionVector)
{
	computeRotatedBox();
	ent.computeRotatedBox();

	if (projectionsOverlap(ent) && ent.projectionsOverlap(*this))
	{
		collisionVector = *ent.getCenter() - *getCenter();
		return true;
	}
	return false;
}

void Entity::computeRotatedBox()
{
	if (rotatedBoxReady)
		return;

	float projection;

	VECTOR2 rotatedX(cos(spriteData.angle), sin(spriteData.angle));
	VECTOR2 rotatedY(-sin(spriteData.angle), cos(spriteData.angle));

	const VECTOR2 *center = getCenter();

	corners[0] = *center + rotatedX * ((float)edge.left*getScale()) + rotatedY * ((float)edge.top*getScale());
	corners[1] = *center + rotatedX * ((float)edge.right*getScale()) + rotatedY * ((float)edge.top*getScale());
	corners[2] = *center + rotatedX * ((float)edge.right*getScale()) + rotatedY * ((float)edge.bottom*getScale());
	corners[3] = *center + rotatedX * ((float)edge.left*getScale()) + rotatedY * ((float)edge.bottom*getScale());

	edge01 = VECTOR2(corners[1].x - corners[0].x, corners[1].y - corners[0].y);
	graphics->Vector2Normalize(&edge01);
	edge03 = VECTOR2(corners[3].x - corners[0].x, corners[3].y - corners[0].y);
	graphics->Vector2Normalize(&edge03);

	projection = graphics->Vector2Dot(&edge01, &corners[0]);
	edge01Min = projection;
	edge01Max = projection;

	projection = graphics->Vector2Dot(&edge01, &corners[1]);
	if (projection < edge01Min)
		edge01Min = projection;
	else if (projection > edge01Max)
		edge01Max = projection;

	projection = graphics->Vector2Dot(&edge03, &corners[0]);
	edge03Min = projection;
	edge03Max = projection;

	projection = graphics->Vector2Dot(&edge03, &corners[3]);
	if (projection < edge03Min)
		edge03Min = projection;
	else if (projection > edge03Max)
		edge03Max = projection;

	rotatedBoxReady = true;
}

bool Entity::projectionsOverlap(Entity &ent)
{
	float projection, min01, max01, min03, max03;
	projection = graphics->Vector2Dot(&edge01, ent.getCorner(0));
	min01 = projection;
	max01 = projection;

	for (int c = 1; c < 4; c++)
	{
		projection = graphics->Vector2Dot(&edge01, ent.getCorner(c));
		if (projection < min01)
			min01 = projection;
		else if (projection > max01)
			max01 = projection;
	}

	if (min01>edge01Max || max01 < edge01Min)
		return false;

	projection = graphics->Vector2Dot(&edge03, ent.getCorner(0));
	min03 = projection;
	max03 = projection;

	for (int c = 1; c < 4; c++)
	{
		projection = graphics->Vector2Dot(&edge03, ent.getCorner(c));
		if (projection < min03)
			min03 = projection;
		else if (projection > max03)
			max03 = projection;
	}

	if (min03 > edge03Max || max03 < edge03Max)
		return false;

	return true;
}

bool Entity::collideRotatedBoxCircle(Entity &ent, VECTOR2 &collisionVector)
{
	float min01, min03, max01, max03, center01, center03;
	computeRotatedBox();
	center01 = graphics->Vector2Dot(&edge01, ent.getCenter());
	min01 = center01 - ent.getRadius()*ent.getScale();
	max01 = center01 + ent.getRadius()*ent.getScale();

	if (min01 > edge01Max || max01 < edge01Min)
		return false;

	center03 = graphics->Vector2Dot(&edge03, ent.getCenter());
	min03 = center03;
	max03 = center03;

	if (min03 > edge03Max || max03 < edge03Min)
		return false;

	if (center01 < edge01Min && center03 < edge03Min)
		return collideCornerCircle(corners[0], ent, collisionVector);

	if (center01 > edge01Max || center03 < edge03Min)
		return collideCornerCircle(corners[1], ent, collisionVector);

	if (center01 > edge01Max || center03 > edge03Max)
		return collideCornerCircle(corners[2], ent, collisionVector);

	if (center01 < edge01Min || center03 > edge03Max)
		return collideCornerCircle(corners[3], ent, collisionVector);

	collisionVector = *ent.getCenter() - *getCenter();
	return true;
}

bool Entity::collideCornerCircle(VECTOR2 corner, Entity &ent, VECTOR2 &collisionVector)
{
	distSquared = corner - *ent.getCenter();
	distSquared.x = distSquared.x * distSquared.x;
	distSquared.y = distSquared.y * distSquared.y;
	sumRadiiSquared = ent.getRadius()*ent.getScale();
	sumRadiiSquared *= sumRadiiSquared;
	if (distSquared.x + distSquared.y <= sumRadiiSquared)
	{
		collisionVector = *ent.getCenter() - corner;
		return true;
	}
	return false;
}

bool Entity::initialize(Game *gamePtr, int width, int height, int ncols, TextureManager *textureM)
{
	input = gamePtr->getInput();
	audio = gamePtr->getAudio();
	return(Image::initialize(gamePtr->getGraphics(), width, height, ncols, textureM));
}

void Entity::activate()
{
	active = true;
}

void Entity::update(float frameTime)
{
	velocity += deltaV;
	deltaV.x = 0;
	deltaV.y = 0;
	Image::update(frameTime);
	rotatedBoxReady = false;
}

void Entity::ai(float frameTime, Entity &ent)
{

}

bool Entity::collidesWith(Entity &ent, VECTOR2 &collisionVector)
{
	if (!active || !ent.getActive())
		return false;

	if (collisionType == entityNS::CIRCLE && ent.getCollisionType() == entityNS::CIRCLE)
		return collideCircle(ent, collisionVector);

	if (collisionType == entityNS::BOX && ent.getCollisionType() == entityNS::BOX)
		return collideBox(ent, collisionVector);

	if (collisionType != entityNS::CIRCLE && ent.getCollisionType() != entityNS::CIRCLE)
		return collideRotatedBox(ent, collisionVector);

	else
		if (collisionType == entityNS::CIRCLE)
			return ent.collideRotatedBoxCircle(*this, collisionVector);
		else
			return collideRotatedBox(ent, collisionVector);
	return false;
}

bool Entity::outsideRect(RECT rect)
{
	if (spriteData.x + spriteData.width*getScale() < rect.left || spriteData.x > rect.right || spriteData.y + spriteData.height*getScale() < rect.top || spriteData.y > rect.bottom)
		return true;
	return false;
}

void Entity::damage(int weapon)
{
	
}

void Entity::bounce(VECTOR2 &collisionVector, Entity &ent)
{
	VECTOR2 Vdiff = ent.getVelocity() - velocity;
	VECTOR2 cUV = collisionVector;
	Graphics::Vector2Normalize(&cUV);
	float cUVdotVdiff = Graphics::Vector2Dot(&cUV, &Vdiff);
	float massRatio = 2.0f;

	if (getMass() != 0)
		massRatio *= (ent.getMass() / (getMass() + ent.getMass()));

	if (cUVdotVdiff > 0)
	{
		setX(getX() - cUV.x * massRatio);
		setY(getY() - cUV.y * massRatio);
	}
	else
		deltaV += ((massRatio * cUVdotVdiff) * cUV);
}

void Entity::gravityForce(Entity *other, float frameTime)
{
	if (!active || !other->getActive())
		return;
	rr = pow((other->getCenterX() - getCenterX()), 2) + pow((other->getCenterY() - getCenterY()), 2);
	force = gravity * other->getMass() * mass / rr;

	VECTOR2 gravityV(other->getCenterX() - getCenterX(), other->getCenterY() - getCenterY());
	Graphics::Vector2Normalize(&gravityV);
	gravityV *= force * frameTime;
	velocity += gravityV;
}
