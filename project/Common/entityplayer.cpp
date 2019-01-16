#include "entityplayer.h"
#include "application.h"
#include "asserts.h"
#include "events.h"
#include "vec2.h"
#include "EntityPlayerSerializer.h"


EntityPlayer::EntityPlayer(int id, float posX, float posY, float radius, float speed, float red, float green, float blue, int priority) : 
	Entity                 (E_Type::Player, id, posX, posY),
	mRadius                (radius),
	mSpeed                 (speed),
	mRed                   (red),
	mGreen                 (green),
	mBlue                  (blue),
	mPriority              (priority),
    mTargetPosX            (0.f),
    mTargetPosY            (0.f),
	mRegisteredInputEvents (false),
	mWindowFocused(true) {

	updateCollisionInfo();
	setCollisionType(Collider::CollisionType::COLLISION_CIRCLE);
}

// *************************************************

EntityPlayer::EntityPlayer(float posX, float posY, float radius, float speed, float red, float green, float blue, int priority) : 
	EntityPlayer (-1, posX, posY, radius, speed, red, green, blue, priority) {}

// *************************************************

EntityPlayer::~EntityPlayer() {
	if (mRegisteredInputEvents) {
		Application * pApplication   = Application::Instance();
		ASSERT(pApplication);
		IEventManager * pInputManager  = pApplication->getEventManager();
		ASSERT(pInputManager);
		pInputManager->unregisterEvent(this);
	}
}

// *************************************************

bool EntityPlayer::init(bool registerInputEvents) { 
	Application * pApplication   = Application::Instance();
	ASSERT(pApplication);
	m_pWindowManager = pApplication->getWindowManager();
	ASSERT(m_pWindowManager);

	mWindowFocused = m_pWindowManager->getWindowAttrib(IWindowManager::WM_Attrib::HAS_FOCUS);
	mRegisteredInputEvents = registerInputEvents;

	if (registerInputEvents) {
		IEventManager * pInputManager  = pApplication->getEventManager();
		ASSERT(pInputManager);
		pInputManager->registerEvent(this, IEventManager::TEvent::EMouseMove, mPriority);
		pInputManager->registerEvent(this, IEventManager::TEvent::EMouseClick, mPriority);
		pInputManager->registerEvent(this, IEventManager::TEvent::EKeyPressed, mPriority);
		pInputManager->registerEvent(this, IEventManager::TEvent::EWindowFocusChanged, mPriority);
	}

	setCollisionType(Collider::CollisionType::COLLISION_CIRCLE);

	return true;
}

// *************************************************

bool EntityPlayer::processEvent(const CEvent &event) {
	switch (event.GetType()) {

		case IEventManager::TEvent::EMouseMove: {
			if (mWindowFocused) {
				const CEventMouseMove * eMouseMovement = dynamic_cast<const CEventMouseMove *>(&event);
				mTargetPosX = eMouseMovement->GetPosX();
				mTargetPosY = eMouseMovement->GetPosY();
			}

			break;
		}

		case IEventManager::TEvent::EWindowFocusChanged: {
			const CEventWindowFocusChanged * eWindowFocusChanged = dynamic_cast<const CEventWindowFocusChanged *>(&event);
			mWindowFocused = eWindowFocusChanged->GetFocused();
			if (!mWindowFocused) {
				// Si la ventana no tiene foco, el movimiento se para
				mTargetPosX = mPosX;
				mTargetPosY = mPosY;
			}

			break;
		}

		default:
			return false;
	}
	
	return true;
}

// *************************************************

void EntityPlayer::update(float deltaTime) {
	Vec2 distance(mTargetPosX - mPosX, mTargetPosY - mPosY);
	if (abs(distance.x) > EPSILON || abs(distance.y) > EPSILON) {
		Vec2 norm = distance.norm();
		Vec2 movement = norm * mSpeed;
		if (abs(distance.x) > EPSILON) {
			setPosX(mPosX + movement.x * deltaTime);
		}
		if (abs(distance.y) > EPSILON) {
			setPosY(mPosY + movement.y * deltaTime);
		}
	}
}

// *************************************************

void EntityPlayer::render() {
	m_pWindowManager->setColor(mRed, mGreen, mBlue, 1.0f);
	m_pWindowManager->drawOval(mPosX, mPosY, mRadius, mRadius);
}

// *************************************************

float EntityPlayer::getRadius() const { return mRadius; }

// *************************************************

void EntityPlayer::setRadius(float radius) { 
	mRadius = radius; 
	updateCollisionInfo();
}

// *************************************************

float EntityPlayer::getSpeed() const { return mSpeed; }

// *************************************************

void EntityPlayer::setSpeed(float speed) { mSpeed = speed; }

// *************************************************

float EntityPlayer::getRed() const { return mRed; }

// *************************************************

float EntityPlayer::getGreen() const { return mGreen; }

// *************************************************

float EntityPlayer::getBlue() const { return mBlue; }

// *************************************************

float EntityPlayer::getTargetPosX() const { return mTargetPosX; }

// *************************************************

void EntityPlayer::setTargetPosX(float targetPosX) { mTargetPosX = targetPosX; }

// *************************************************

float EntityPlayer::getTargetPosY() const { return mTargetPosY; }

// *************************************************

void EntityPlayer::setTargetPosY(float targetPosY) { mTargetPosY = targetPosY; }

// *************************************************

Vec2 EntityPlayer::getSize() const {
	return Vec2(mRadius, mRadius);
}

// *************************************************

size_t EntityPlayer::serialize(char* outBuff) {
	EntityPlayerSerializer serializer(*this);
	return serializer.serialize(outBuff);
}