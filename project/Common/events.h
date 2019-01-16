#pragma once

#include "eventmanager.h"

class CEvent {

public:
	CEvent(IEventManager::TEvent type) : mType(type) {}
	virtual ~CEvent() {}

	IEventManager::TEvent GetType() const { return mType; }

private:
	IEventManager::TEvent mType;
};

// *************************************************

class CEventKeyPressed : public CEvent {

public:
	CEventKeyPressed(int key) :
		CEvent(IEventManager::TEvent::EKeyPressed),
		mKey(key) {}

	int GetKey() const { return mKey; }

private:
	int mKey;
};

// *************************************************

class CEventMouseClick : public CEvent {

public:
	enum EMouseButton {
		Left,
		Right,
		Middle,
		NotSupported
	};

	CEventMouseClick(EMouseButton mouseButton) :
		CEvent(IEventManager::TEvent::EMouseClick),
		mMouseButton(mouseButton) {}

	EMouseButton GetMouseButton() const { return mMouseButton; }

private:
	EMouseButton mMouseButton;
};

// *************************************************

class CEventMouseMove : public CEvent {

public:
	CEventMouseMove(float posX, float posY) :
		CEvent(IEventManager::TEvent::EMouseMove),
		mPosX(posX),
		mPosY(posY) {}

	float GetPosX() const { return mPosX; }
	float GetPosY() const { return mPosY; }

private:
	float mPosX;
	float mPosY;
};

// *************************************************

class CEventWindowFocusChanged : public CEvent {

public:
	CEventWindowFocusChanged(bool focused) :
		CEvent(IEventManager::TEvent::EWindowFocusChanged),
		mFocused(focused) {}

	bool GetFocused() const { return mFocused; }

private:
	bool mFocused;
};