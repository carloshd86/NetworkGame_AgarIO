#ifndef __WINDOW_MANAGER_H__
#define __WINDOW_MANAGER_H__

#include <functional>

class IWindowManager {

public:

	enum WM_Err  {
		KO,
		OK
	};

	enum WM_Attrib  {
		HAS_FOCUS
	};

	class Window {
	public:
		void * pWindow;
	};

	virtual WM_Err init ()  = 0;
	virtual WM_Err end  () = 0;

	virtual double        getTime                  () const                                              = 0;
	virtual bool          windowShouldClose        ()                                                    = 0;
	virtual bool          getWindowAttrib          (WM_Attrib attrib) const                              = 0;
	virtual void          getWindowSize            (int& screenWidth, int& screenHeight)                 = 0;
	virtual void          setViewport              (int x, int y, int width, int height)                 = 0;
	virtual void          setResolution            (int width, int height)                               = 0;
	virtual void          setColor                 (float r, float g, float b, float a)                  = 0;
	virtual void          swapBuffers              ()                                                    = 0;
	virtual void          pollEvents               ()                                                    = 0;
	virtual void          waitEvents               ()                                                    = 0;
	virtual void          drawRect                 (float x, float y, float width, float height)         = 0;
	virtual void          drawOval                 (float x, float y, float width, float height)         = 0;
	virtual void          clearColorBuffer         (float r, float g, float b)                           = 0;

};

#endif