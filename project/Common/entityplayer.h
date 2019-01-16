#pragma once

#include "entity.h"
#include "eventmanager.h"
#include "windowmanager.h"

class EntityPlayer : public Entity, public IEventManager::IListener {
public:
	EntityPlayer(int id, float posX = 0.f, float posY = 0.f, float radius = 50.f, float speed = 100.f, float red = 1.f, float green = 1.0, float blue = 0.f, int priority = 1);
	EntityPlayer(float posX = 0.f, float posY = 0.f, float radius = 50.f, float speed = 100.f, float red = 1.f, float green = 1.0, float blue = 0.f, int priority = 1);
	
	~EntityPlayer();

	bool init(bool registerInputEvents = false);

	virtual bool processEvent(const CEvent &event);

	void update(float deltaTime);
	void render();

	float getRadius ()  const;
	void  setRadius (float radius);
	float getSpeed  ()  const;
	void  setSpeed  (float speed);

	float getRed   ()   const;
	float getGreen () const;
	float getBlue  ()  const;

	float getTargetPosX ()  const;
	void  setTargetPosX (float targetPosX);
	float getTargetPosY ()  const;
	void  setTargetPosY (float targetPosY);

	Vec2 getSize() const;

	size_t serialize(char* outBuff);

private:
	float mRadius;
	float mSpeed;

	float mRed;
	float mGreen;
	float mBlue;

	int   mPriority;

	float mTargetPosX;
	float mTargetPosY;

	bool mRegisteredInputEvents;
	bool mWindowFocused;

	IWindowManager * m_pWindowManager;
};