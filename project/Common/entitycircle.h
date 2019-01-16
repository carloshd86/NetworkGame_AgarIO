#pragma once

#include "entity.h"
#include "eventmanager.h"
#include "windowmanager.h"

class EntityCircle : public Entity {
public:
	EntityCircle(int id, float posX = 0.f, float posY = 0.f, float radius = 50.f, float red = 1.f, float green = 1.0, float blue = 0.f);
	EntityCircle(float posX = 0.f, float posY = 0.f, float radius = 50.f, float red = 1.f, float green = 1.0, float blue = 0.f);

	bool init(bool registerInputEvents = false);

	void update(float deltaTime);
	void render();

	float getRadius ()  const;
	void  setRadius (float radius);

	float getRed()   const;
	float getGreen() const;
	float getBlue()  const;

	Vec2 getSize() const;

	size_t serialize(char* outBuff);

private:
	float mRadius;

	float mRed;
	float mGreen;
	float mBlue;

	IWindowManager * m_pWindowManager;
};