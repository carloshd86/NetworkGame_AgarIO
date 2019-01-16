#include "application.h"
#include "glfwwindowmanager.h"
#include "glfwinputmanager.h"
#include "asserts.h"


Application * Application::mInstance;

// *************************************************

Application::Application() :
	mInitialized   (false),
	mWindowManager (nullptr),
	mEventManager  (nullptr) {}

// *************************************************

Application::~Application() {
	if (mInitialized)
		end();
}

// *************************************************

Application * Application::Instance() {
	if (!mInstance) {
		mInstance = new Application();
		mInstance->init();
	}

	return mInstance;
}

// *************************************************

bool Application::init() {
	if (mInitialized)
		return true;

	mWindowManager = GlfwWindowManager::Instance();
	ASSERT(mWindowManager)

	mEventManager  = GlfwInputManager::Instance();
	ASSERT(mEventManager)

	mInitialized  = true;

	return true;
}

// *************************************************

bool Application::end() {

	delete mWindowManager;
	mWindowManager = nullptr;
	delete mEventManager;
	mEventManager = nullptr;

	mInitialized = false;

	return true;
}

// *************************************************

IWindowManager * Application::getWindowManager() {
	return mWindowManager;
}

// *************************************************

IEventManager * Application::getEventManager() {
	return mEventManager;
}

