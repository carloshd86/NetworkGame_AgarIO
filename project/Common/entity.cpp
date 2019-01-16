#include "entity.h"
#include "application.h"
#include "asserts.h"
#include "events.h"
#include "vec2.h"


const float Entity::EPSILON = 0.1f;

// *************************************************

Entity::Entity(E_Type type, int id, float posX, float posY) : 
	mType          (type),
	mId            (id),
	mPosX          (posX),
	mPosY          (posY),
    mDead          (false),
	mPivot         (0.5f, 0.5f),
	mCollisionType (Collider::COLLISION_NONE),
	mCollider      (nullptr) {}

// *************************************************

Entity::Entity(E_Type type, float posX, float posY) :
	Entity(type, -1, posX, posY) {}

// *************************************************

Entity::~Entity() {}

// *************************************************

Entity::E_Type Entity::getType() const { return mType; }

// *************************************************

float Entity::getPosX() const { return mPosX; }

// *************************************************

void Entity::setPosX(float posX) { mPosX = posX; updateCollisionInfo(); }

// *************************************************

float Entity::getPosY() const { return mPosY; }

// *************************************************

void Entity::setPosY(float posY) { mPosY = posY; updateCollisionInfo(); }

// *************************************************

int Entity::getId() const { return mId; }

// *************************************************

void Entity::setId(int id) { mId = id; }

// *************************************************

int Entity::getDead() const { return mDead; }

// *************************************************

void Entity::setDead(bool dead) { mDead = dead; }

// *************************************************

void Entity::setCollisionType(Collider::CollisionType type) {
	mCollisionType = type;
	delete mCollider;
	mCollider = nullptr;

	switch ( type ) {
	case Collider::CollisionType::COLLISION_CIRCLE:
		mCollider = new CircleCollider(mCircleCenter, mCircleRadius);
		break;
	}
}

// *************************************************

Collider::CollisionType Entity::getCollisionType() const {
	return mCollisionType;
}

// *************************************************

const Collider* Entity::getCollider() const {
	return mCollider;
}

// *************************************************

bool Entity::collides(const Entity& other) const {
	if (!mCollider || !other.mCollider) return false;
	return mCollider->collides(*other.mCollider);
}

// *************************************************

void Entity::updateCollisionInfo() {
	Vec2 size = getSize();
	mRectOrigin = Vec2(mPosX, mPosY) - mPivot * size;
	mRectSize = size;
	mCircleCenter = mRectOrigin + mRectSize / 2;
	mCircleRadius = std::min(size.x, size.y) / 2;
}