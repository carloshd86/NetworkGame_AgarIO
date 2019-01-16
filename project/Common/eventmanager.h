#pragma once

class CEvent;
class Window;

class IEventManager {

public:

	enum EM_Err  {
		KO,
		OK
	};

	enum TEvent {
		EKeyPressed,
		EMouseClick,
		EMouseMove,
		EWindowFocusChanged,
		EAll
	};

	class IListener {

	public:

		virtual bool processEvent(const CEvent &event) = 0;
	};

	virtual EM_Err registerEvent   (IListener * listener, TEvent e, int priority)  = 0;
	virtual EM_Err unregisterEvent (IListener * listener, TEvent e = TEvent::EAll) = 0;
};