#include "glfwwindowmanager.h"
#include "globals.h"
#include "asserts.h"

#define LITE_GFX_IMPLEMENTATION
#include <litegfx.h>

#define mGlfwWindow static_cast<GLFWwindow * >(mWindow.pWindow)


// *************************************************

GlfwWindowManager * GlfwWindowManager::mInstance;

// *************************************************

GlfwWindowManager::GlfwWindowManager() :
	mInitialized (false) {}

// *************************************************

GlfwWindowManager::~GlfwWindowManager() {
	if (mInitialized)
		end();
}

// *************************************************

GlfwWindowManager * GlfwWindowManager::Instance() {
	if (!mInstance) {
		mInstance = new GlfwWindowManager();
		mInstance->init();
	}
	return mInstance;
}

// *************************************************

IWindowManager::WM_Err GlfwWindowManager::init() {

	// Init glfw
	if (!glfwInit())
		return KO;

	// Creating the window
	glfwWindowHint(GLFW_RESIZABLE, true);
	GLFWwindow* glfwWindow = glfwCreateWindow(GlobalConstants::SCREEN_WIDTH, GlobalConstants::SCREEN_HEIGHT, "Interfaz Usuario | GlfwInputManager", nullptr, nullptr);
	if (!glfwWindow)
		return KO;

	// Activating OpenGL
	glfwMakeContextCurrent(glfwWindow);

	// Initializing LiteGFX
	lgfx_setup2d(GlobalConstants::SCREEN_WIDTH, GlobalConstants::SCREEN_HEIGHT);

	mWindow.pWindow = glfwWindow;

	mInitialized = true;
	return OK;
}

// *************************************************

IWindowManager::WM_Err GlfwWindowManager::end() {
	if (mInitialized)
		glfwTerminate();
	return OK;
}

// *************************************************

double GlfwWindowManager::getTime() const {
	return glfwGetTime();
}

// *************************************************

bool  GlfwWindowManager::windowShouldClose() {

	return glfwWindowShouldClose(mGlfwWindow);
}

// *************************************************

bool GlfwWindowManager::getWindowAttrib(WM_Attrib attrib) const {
	int glfwAttrib = 0;
	switch (attrib) {
		case WM_Attrib::HAS_FOCUS:
			glfwAttrib = GLFW_FOCUSED;
		break;
	}
	
	return glfwGetWindowAttrib(mGlfwWindow, glfwAttrib);
}

// *************************************************

void  GlfwWindowManager::getWindowSize(int& screenWidth, int& screenHeight) {
	glfwGetWindowSize(mGlfwWindow, &screenWidth, &screenHeight);
}

// *************************************************

void  GlfwWindowManager::setViewport(int x, int y, int width, int height) {
	lgfx_setviewport(x, y, width, height);
}

// *************************************************

void  GlfwWindowManager::setResolution(int width, int height) {
	lgfx_setresolution(width, height);
}

// *************************************************

void  GlfwWindowManager::setColor(float r, float g, float b, float a) {
	lgfx_setcolor(r, g, b, a);
}

// *************************************************

void  GlfwWindowManager::swapBuffers() {
	glfwSwapBuffers(mGlfwWindow);	
}

// *************************************************

void  GlfwWindowManager::pollEvents() {
	glfwPollEvents();
}

// *************************************************

void  GlfwWindowManager::waitEvents() {
	glfwWaitEvents();
}

// *************************************************

void GlfwWindowManager::drawRect(float x, float y, float width, float height) {
	lgfx_drawrect(x, y, width, height);
}

// *************************************************

void GlfwWindowManager::drawOval(float x, float y, float width, float height) {
	lgfx_drawoval(x - width/2, y - width/2, width, height);
}

// *************************************************
void GlfwWindowManager::clearColorBuffer(float r, float g, float b) {
	lgfx_clearcolorbuffer(r, g, b);
}

// *************************************************

void GlfwWindowManager::setMouseMoveCallback(GLFWcursorposfun fun) {
	glfwSetCursorPosCallback(mGlfwWindow, fun);
}

// *************************************************

void GlfwWindowManager::setMouseClickCallback(GLFWmousebuttonfun fun) {
	glfwSetMouseButtonCallback(mGlfwWindow, fun);
}

// *************************************************

void GlfwWindowManager::setKeyPressedCallback(GLFWkeyfun fun) {
	glfwSetKeyCallback(mGlfwWindow, fun);
}

// *************************************************

void GlfwWindowManager::setWindowFocusChangedCallback(GLFWwindowfocusfun fun) {
	glfwSetWindowFocusCallback(mGlfwWindow, fun);
}