#pragma once

#include "eventmanager.h"
#include <map>
#include <vector>
#include "glfwwindowmanager.h"

class GlfwInputManager : public IEventManager {
	
public:

	typedef std::map<TEvent, std::multimap<int, IListener *>> ListenerMap;

	static GlfwInputManager * Instance();
	~GlfwInputManager();

	EM_Err init();
	EM_Err end();

	EM_Err registerEvent   (IListener * listener, TEvent e, int priority);
	EM_Err unregisterEvent (IListener * listener, TEvent e = TEvent::EAll);

	ListenerMap& getListenerMap();

	static void mouseMove          (GLFWwindow * window, double xpos, double ypos);
	static void mouseClick         (GLFWwindow * window, int button, int action, int mods);
	static void keyPressed         (GLFWwindow * window, int key, int scancode, int action, int mods);
	static void windowFocusChanged (GLFWwindow * window, int focused);

private:
	GlfwInputManager();

	static GlfwInputManager *mInstance;

	ListenerMap         mListeners;
	Window             *m_pWindow;
	GlfwWindowManager  *m_pWindowManager;
	bool                mInitialized;

	void removeListenerForEvent(IListener * listener, TEvent e);
};