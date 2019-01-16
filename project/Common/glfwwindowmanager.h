#pragma once

#include "windowmanager.h"
#include <glfw3.h>


class GlfwWindowManager : public IWindowManager {

public:

	static GlfwWindowManager * Instance();
	~GlfwWindowManager();

	virtual WM_Err init ();
	virtual WM_Err end  ();

	virtual double        getTime           () const;
	virtual bool          windowShouldClose ();
	virtual bool          getWindowAttrib   (WM_Attrib attrib) const;
	virtual void          getWindowSize     (int& screenWidth, int& screenHeight);
	virtual void          setViewport       (int x, int y, int width, int height);
	virtual void          setResolution     (int width, int height);
	virtual void          setColor          (float r, float g, float b, float a);
	virtual void          swapBuffers       ();
	virtual void          pollEvents        ();
	virtual void          waitEvents        ();
	virtual void          drawRect          (float x, float y, float width, float height);
	virtual void          drawOval          (float x, float y, float width, float height);
	virtual void          clearColorBuffer  (float r, float g, float b);
	
	virtual void setMouseMoveCallback          (GLFWcursorposfun fun);
	virtual void setMouseClickCallback         (GLFWmousebuttonfun fun);
	virtual void setKeyPressedCallback         (GLFWkeyfun fun);
	virtual void setWindowFocusChangedCallback (GLFWwindowfocusfun fun);

private:
	GlfwWindowManager();

	static GlfwWindowManager *mInstance;
	Window                    mWindow;

	bool mInitialized;
};