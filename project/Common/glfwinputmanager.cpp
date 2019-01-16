#include "glfwinputmanager.h"
#include "globals.h"
#include "glfwwindowmanager.h"
#include "asserts.h"
#include <algorithm>
#include <utility>
#include "events.h"


GlfwInputManager * GlfwInputManager::mInstance;

// *************************************************

GlfwInputManager::GlfwInputManager() :
	mInitialized (false) {}

// *************************************************

GlfwInputManager::~GlfwInputManager() {
	if (mInitialized)
		end();
}

// *************************************************

GlfwInputManager * GlfwInputManager::Instance() {
	if (!mInstance) {
		mInstance = new GlfwInputManager();
		mInstance->init();
	}

	return mInstance;
}

// *************************************************

IEventManager::EM_Err GlfwInputManager::init() {

	if (mInitialized)
		return OK;

	m_pWindowManager = GlfwWindowManager::Instance();

	ASSERT(m_pWindowManager);

	// Setting events
	// Mouse Move
	m_pWindowManager->setMouseMoveCallback(&GlfwInputManager::mouseMove);

	// Mouse Click
	m_pWindowManager->setMouseClickCallback(&GlfwInputManager::mouseClick);
	
	// Key Pressed
	m_pWindowManager->setKeyPressedCallback(&GlfwInputManager::keyPressed);

	// Window focus changed
	m_pWindowManager->setWindowFocusChangedCallback(&GlfwInputManager::windowFocusChanged);

	mInitialized = true;
	return OK;
}

// *************************************************

IEventManager::EM_Err GlfwInputManager::end() {

	return OK;
}

// *************************************************

IEventManager::EM_Err GlfwInputManager::registerEvent(IListener * listener, TEvent e, int priority) {

	if (EAll == e)
		return KO;

	mListeners[e].insert(std::pair<int, IListener *>(priority, listener));

	return OK;
}

// *************************************************

IEventManager::EM_Err GlfwInputManager::unregisterEvent(IListener * listener, TEvent e) {

	if (TEvent::EAll == e) {
		for (auto it = mListeners.begin(); it != mListeners.end(); ++it) {
			TEvent event = it->first;
			removeListenerForEvent(listener, event);
		}
	}
	else {
		removeListenerForEvent(listener, e);
	}

	return OK;
}

// *************************************************

GlfwInputManager::ListenerMap& GlfwInputManager::getListenerMap() {
	return mListeners;
}

// *************************************************

void GlfwInputManager::removeListenerForEvent(IListener * listener, TEvent e) {

	for(auto prioritiesIt = mListeners[e].begin(); prioritiesIt != mListeners[e].end(); ++prioritiesIt) {
		if (prioritiesIt->second == listener) {
			prioritiesIt = mListeners[e].erase(prioritiesIt);
			break;
		}
	}

}

// *************************************************

void GlfwInputManager::mouseMove(GLFWwindow * window, double xpos, double ypos) {
	ListenerMap& listenerMap = mInstance->getListenerMap();

	if (listenerMap[TEvent::EMouseMove].size()) {
		for (auto prioritiesIt = listenerMap[TEvent::EMouseMove].begin(); prioritiesIt != listenerMap[TEvent::EMouseMove].end(); ++prioritiesIt) {
			prioritiesIt->second->processEvent(CEventMouseMove(static_cast<float>(xpos), static_cast<float>(ypos)));
		}
	}
}

// *************************************************

void GlfwInputManager::mouseClick(GLFWwindow * window, int button, int action, int mods) {
	ListenerMap& listenerMap = mInstance->getListenerMap();

	CEventMouseClick::EMouseButton mouseButton = CEventMouseClick::EMouseButton::NotSupported;
	switch (button) {
		case GLFW_MOUSE_BUTTON_LEFT:   mouseButton = CEventMouseClick::EMouseButton::Left;   break;
		case GLFW_MOUSE_BUTTON_MIDDLE: mouseButton = CEventMouseClick::EMouseButton::Middle; break;
		case GLFW_MOUSE_BUTTON_RIGHT:  mouseButton = CEventMouseClick::EMouseButton::Right;  break;
	}

	if (listenerMap[TEvent::EMouseClick].size()) {
		for (auto prioritiesIt = listenerMap[TEvent::EMouseClick].begin(); prioritiesIt != listenerMap[TEvent::EMouseClick].end(); ++prioritiesIt) {
			prioritiesIt->second->processEvent(CEventMouseClick(mouseButton));
		}
	}
}

// *************************************************

void GlfwInputManager::keyPressed(GLFWwindow * window, int key, int scancode, int action, int mods) {
	ListenerMap& listenerMap = mInstance->getListenerMap();
	
	if (listenerMap[TEvent::EKeyPressed].size()) {
		for (auto prioritiesIt = listenerMap[TEvent::EKeyPressed].begin(); prioritiesIt != listenerMap[TEvent::EKeyPressed].end(); ++prioritiesIt) {
			prioritiesIt->second->processEvent(CEventKeyPressed(key));
		}
	}
}

// *************************************************

void GlfwInputManager::windowFocusChanged(GLFWwindow * window, int focused) {
	ListenerMap& listenerMap = mInstance->getListenerMap();
	if (listenerMap[TEvent::EWindowFocusChanged].size()) {
		for (auto prioritiesIt = listenerMap[TEvent::EWindowFocusChanged].begin(); prioritiesIt != listenerMap[TEvent::EWindowFocusChanged].end(); ++prioritiesIt) {
			prioritiesIt->second->processEvent(CEventWindowFocusChanged(static_cast<bool>(focused)));
		}
	}
}