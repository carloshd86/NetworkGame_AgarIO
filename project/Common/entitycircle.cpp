#include "entitycircle.h"
#include "application.h"
#include "asserts.h"
#include "events.h"
#include "vec2.h"
#include "EntityCircleSerializer.h"


EntityCircle::EntityCircle(int id, float posX, float posY, float radius, float red, float green, float blue) : 
	Entity     (E_Type::Circle, id, posX, posY),
	mRadius    (radius),
	mRed       (red),
	mGreen     (green),
	mBlue      (blue) {

	updateCollisionInfo();
	setCollisionType(Collider::CollisionType::COLLISION_CIRCLE);
}

// *************************************************

EntityCircle::EntityCircle(float posX, float posY, float radius, float red, float green, float blue) : 
	EntityCircle (-1, posX, posY, radius, red, green, blue) {}

// *************************************************

bool EntityCircle::init(bool registerInputEvents) { 
	Application * pApplication   = Application::Instance();
	ASSERT(pApplication);
	m_pWindowManager = pApplication->getWindowManager();
	ASSERT(m_pWindowManager);

	return true;
}

// *************************************************

void EntityCircle::update(float deltaTime) {
}

// *************************************************

void EntityCircle::render() {
	m_pWindowManager->setColor(mRed, mGreen, mBlue, 1.0f);
	m_pWindowManager->drawOval(mPosX, mPosY, mRadius, mRadius);
}

// *************************************************

float EntityCircle::getRadius() const { return mRadius; }

// *************************************************

void EntityCircle::setRadius(float radius) { 
	mRadius = radius; 
	updateCollisionInfo();
}

// *************************************************

float EntityCircle::getRed() const { return mRed; }

// *************************************************

float EntityCircle::getGreen() const { return mGreen; }

// *************************************************

float EntityCircle::getBlue() const { return mBlue; }

// *************************************************

Vec2 EntityCircle::getSize() const {
	return Vec2(mRadius, mRadius);
}

// *************************************************

size_t EntityCircle::serialize(char* outBuff) {
	EntityCircleSerializer serializer(*this);
	return serializer.serialize(outBuff);
}