#pragma once

#include "collider.h"

class Entity {
public:

	enum E_Type  {
		Player,
		Circle
	};

	Entity(E_Type type, int id, float posX = 0.f, float posY = 0.f);
	Entity(E_Type type, float posX = 0.f, float posY = 0.f);

	virtual ~Entity();

	virtual bool init(bool registerInputEvents = false) = 0;

	virtual void update(float deltaTime) = 0;
	virtual void render() = 0;

	E_Type getType() const;

	virtual float getPosX() const;
	virtual void  setPosX(float posX);
	virtual float getPosY() const;
	virtual void  setPosY(float posY);

	int  getId() const;
	void setId(int id);

	int  getDead() const;
	void setDead(bool dead);

	void                    setCollisionType (Collider::CollisionType type);
	Collider::CollisionType getCollisionType ()                    const;
	const Collider*         getCollider      ()                    const;
	bool                    collides         (const Entity& other) const;

	virtual Vec2 getSize() const = 0;

	virtual size_t serialize(char* outBuff) = 0;

protected:
	static const float EPSILON;

	E_Type mType;

	Vec2 mPivot;

	float mPosX;
	float mPosY;

	int mId;

	bool mDead;
	
	void updateCollisionInfo();

private:
	// Collision
	Collider::CollisionType mCollisionType;
	Collider*				mCollider;
	Vec2					mCircleCenter;
	float					mCircleRadius;
	Vec2					mRectOrigin;
	Vec2					mRectSize;
};