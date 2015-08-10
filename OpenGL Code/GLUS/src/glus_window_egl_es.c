/*
 * GLUS - Modern OpenGL, OpenGL ES and OpenVG Utilities. Copyright (C) since 2010 Norbert Nopper
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "GL/glus.h"

extern GLUSvoid _glusOsPollEvents(GLUSvoid);

extern EGLNativeDisplayType _glusOsGetNativeDisplayType(GLUSvoid);

extern EGLNativeWindowType _glusOsCreateNativeWindowType(const char* title, const GLUSint width, const GLUSint height, const GLUSboolean fullscreen, const GLUSboolean noResize, const GLUSint nativeVisualID);

extern GLUSvoid _glusOsDestroyNativeWindowDisplay(GLUSvoid);

extern double _glusOsGetRawTime(GLUSvoid);

extern GLUSvoid _glusOsGetWindowSize(GLUSint* width, GLUSint* height);

extern GLUSint _glusWindowGetCurrentRecordingFrame(GLUSvoid);

extern GLUSint _glusWindowGetCurrentAndIncreaseRecordingFrame(GLUSvoid);

extern GLUSint _glusWindowGetRecordingFrames(GLUSvoid);

extern GLUSfloat _glusWindowGetRecordingTime(GLUSvoid);

extern const GLUStgaimage* _glusWindowGetRecordingImageTga(GLUSvoid);

static EGLDisplay g_eglDisplay = EGL_NO_DISPLAY;
static EGLDisplay g_eglSurface = EGL_NO_SURFACE;
static EGLDisplay g_eglContext = EGL_NO_CONTEXT;

static GLUSboolean g_windowCreated = GLUS_FALSE;
static GLUSboolean g_initdone = GLUS_FALSE;
static GLUSboolean g_done = GLUS_FALSE;
static GLUSint g_buttons = 0;
static GLUSint g_mouseX = 0;
static GLUSint g_mouseY = 0;

static GLUSint g_width = 640;
static GLUSint g_height = 480;

static GLUSboolean (*glusInit)(GLUSvoid) = 0;
static GLUSvoid (*glusReshape)(GLUSint width, GLUSint height) = 0;
static GLUSboolean (*glusUpdate)(GLUSfloat time) = 0;
static GLUSvoid (*glusTerminate)(GLUSvoid) = 0;

static GLUSvoid (*glusKey)(GLUSboolean pressed, GLUSint key) = 0;
static GLUSvoid (*glusMouse)(GLUSboolean pressed, GLUSint button, GLUSint xPos, GLUSint yPos) = 0;
static GLUSvoid (*glusMouseWheel)(GLUSint buttons, GLUSint ticks, GLUSint xPos, GLUSint yPos) = 0;
static GLUSvoid (*glusMouseMove)(GLUSint buttons, GLUSint xPos, GLUSint yPos) = 0;

GLUSvoid GLUSAPIENTRY glusWindowSetKeyFunc(GLUSvoid (*glusNewKey)(GLUSboolean pressed, GLUSint key))
{
	glusKey = glusNewKey;
}

GLUSvoid GLUSAPIENTRY glusWindowSetMouseFunc(GLUSvoid (*glusNewMouse)(GLUSboolean pressed, GLUSint button, GLUSint xPos, GLUSint yPos))
{
	glusMouse = glusNewMouse;
}

GLUSvoid GLUSAPIENTRY glusWindowSetMouseWheelFunc(GLUSvoid (*glusNewMouseWheel)(GLUSint buttons, GLUSint ticks, GLUSint xPos, GLUSint yPos))
{
	glusMouseWheel = glusNewMouseWheel;
}

GLUSvoid GLUSAPIENTRY glusWindowSetMouseMoveFunc(GLUSvoid (*glusNewMouseMove)(GLUSint buttons, GLUSint xPos, GLUSint yPos))
{
	glusMouseMove = glusNewMouseMove;
}

GLUSvoid GLUSAPIENTRY glusWindowSetInitFunc(GLUSboolean (*glusNewInit)(GLUSvoid))
{
	glusInit = glusNewInit;
}

GLUSvoid GLUSAPIENTRY glusWindowSetReshapeFunc(GLUSvoid (*glusNewReshape)(GLUSint width, GLUSint height))
{
	glusReshape = glusNewReshape;
}

GLUSvoid GLUSAPIENTRY glusWindowSetUpdateFunc(GLUSboolean (*glusNewUpdate)(GLUSfloat time))
{
	glusUpdate = glusNewUpdate;
}

GLUSvoid GLUSAPIENTRY glusWindowSetTerminateFunc(GLUSvoid (*glusNewTerminate)(GLUSvoid))
{
	glusTerminate = glusNewTerminate;
}

static GLUSfloat glusWindowGetElapsedTime(GLUSvoid)
{
	static GLUSboolean init = GLUS_FALSE;
	static GLUSfloat lastTime;
	static GLUSfloat currentTime;

	GLUSfloat measuredTime;

	measuredTime = (GLUSfloat)_glusOsGetRawTime();

	if (!init)
	{
		lastTime = measuredTime;

		currentTime = measuredTime;

		init = GLUS_TRUE;
	}
	else
	{
		lastTime = currentTime;

		currentTime = measuredTime;
	}

	return currentTime - lastTime;
}

GLUSvoid _glusWindowInternalReshape(GLUSint width, GLUSint height)
{
	if (width < 1)
	{
		width = 1;
	}
	if (height < 1)
	{
		height = 1;
	}

	if (glusReshape && g_initdone)
	{
		glusReshape(width, height);
	}
}

GLUSvoid _glusWindowInternalClose(GLUSvoid)
{
	g_done = GLUS_TRUE;
}

GLUSvoid _glusWindowInternalKey(GLUSint key, GLUSint state)
{
	if (state == GLFW_RELEASE)
	{
		if (key == GLFW_KEY_ESCAPE)
		{
			g_done = GLUS_TRUE;

			return;
		}

		if (glusKey)
		{
			glusKey(GLUS_FALSE, tolower(key));
		}
	}
	else
	{
		if (glusKey)
		{
			glusKey(GLUS_TRUE, tolower(key));
		}
	}
}

GLUSvoid _glusWindowInternalMouse(GLUSint button, GLUSint action)
{
	GLUSint usedButton = 0;

	if (button == GLFW_MOUSE_BUTTON_LEFT)
	{
		usedButton = 1;
	}
	else if (button == GLFW_MOUSE_BUTTON_MIDDLE)
	{
		usedButton = 2;
	}
	else if (button == GLFW_MOUSE_BUTTON_RIGHT)
	{
		usedButton = 4;
	}

	if (action == GLFW_PRESS)
	{
		g_buttons |= usedButton;
	}
	else
	{
		g_buttons ^= usedButton;
	}

	if (glusMouse)
	{
		glusMouse(action == GLFW_PRESS, usedButton, g_mouseX, g_mouseY);
	}
}

GLUSvoid _glusWindowInternalMouseWheel(GLUSint pos)
{
	if (glusMouseWheel)
	{
		glusMouseWheel(g_buttons, pos, g_mouseX, g_mouseY);
	}
}

GLUSvoid _glusWindowInternalMouseMove(GLUSint x, GLUSint y)
{
	g_mouseX = x;
	g_mouseY = y;

	if (glusMouseMove)
	{
		glusMouseMove(g_buttons, g_mouseX, g_mouseY);
	}
}

GLUSvoid GLUSAPIENTRY glusWindowDestroy(GLUSvoid)
{
	glusEGLTerminate(&g_eglDisplay, &g_eglContext, &g_eglSurface);

	_glusOsDestroyNativeWindowDisplay();

	g_windowCreated = GLUS_FALSE;

	g_initdone = GLUS_FALSE;
}

GLUSboolean GLUSAPIENTRY glusWindowCreate(const GLUSchar* title, const GLUSint width, const GLUSint height, const GLUSboolean fullscreen, const GLUSboolean noResize, const EGLint* configAttribList, const EGLint* contextAttribList, const EGLint* surfaceAttribList)
{
	EGLConfig eglConfig;

	EGLNativeWindowType eglNativeWindowType;

	EGLint nativeVisualID = 0;

	if (g_windowCreated)
	{
		glusLogPrint(GLUS_LOG_ERROR, "Window already exists");

		return GLUS_FALSE;
	}

	if (!glusEGLCreateContext(_glusOsGetNativeDisplayType(), &g_eglDisplay, &eglConfig, &g_eglContext, configAttribList, contextAttribList))
	{
		glusWindowDestroy();

		return GLUS_FALSE;
	}

	eglGetConfigAttrib(g_eglDisplay, eglConfig, EGL_NATIVE_VISUAL_ID, &nativeVisualID);

	eglNativeWindowType = _glusOsCreateNativeWindowType(title, width, height, fullscreen, noResize, nativeVisualID);

	if (!eglNativeWindowType)
	{
		glusLogPrint(GLUS_LOG_ERROR, "Could not create native window");

		glusWindowDestroy();

		return GLUS_FALSE;
	}

	if (!glusEGLCreateWindowSurfaceMakeCurrent(eglNativeWindowType, &g_eglDisplay, &eglConfig, &g_eglContext, &g_eglSurface, surfaceAttribList))
	{
		glusWindowDestroy();

		return GLUS_FALSE;
	}

	_glusOsGetWindowSize(&g_width, &g_height);

	g_windowCreated = GLUS_TRUE;

	return GLUS_TRUE; // Success
}

GLUSboolean GLUSAPIENTRY glusWindowRun(GLUSvoid)
{
	GLUSboolean run = GLUS_TRUE;

	if (!glusWindowStartup())
	{
		return GLUS_FALSE;
	}

	while (run)
	{
		if (glusWindowIsRecording())
		{
			run = glusWindowLoopDoRecording();
		}
		else
		{
			run = glusWindowLoop();
		}
	}

	glusWindowShutdown();

	return GLUS_TRUE; // Exit The Program
}

GLUSboolean GLUSAPIENTRY glusWindowStartup(GLUSvoid)
{
	// Init Engine
	if (glusInit)
	{
		if (!glusInit())
		{
			glusWindowShutdown();

			return GLUS_FALSE; // Exit The Program
		}
	}

	g_initdone = GLUS_TRUE;

	// Do the first reshape
	if (glusReshape)
	{
		glusReshape(g_width, g_height);
	}

	return GLUS_TRUE;
}

GLUSboolean GLUSAPIENTRY glusWindowLoop(GLUSvoid)
{
	if (!g_done) // Loop That Runs While done=FALSE
	{
		if (glusUpdate)
		{
			g_done = !glusUpdate(glusWindowGetElapsedTime());
		}

		eglSwapBuffers(g_eglDisplay, g_eglSurface); // Swap Buffers (Double Buffering)

		_glusOsPollEvents();
	}

	return !g_done;
}

GLUSboolean GLUSAPIENTRY glusWindowLoopDoRecording(GLUSvoid)
{
	if (!g_done) // Loop That Runs While done=FALSE
	{
		if (glusUpdate)
		{
			// Still consume and update time, as a fixed recording time is used.
			glusWindowGetElapsedTime();

			g_done = !glusUpdate(_glusWindowGetRecordingTime());

			if (!g_done)
			{
				if (_glusWindowGetCurrentRecordingFrame() < _glusWindowGetRecordingFrames())
				{
					if (glusScreenshotUseTga(0, 0, _glusWindowGetRecordingImageTga()))
					{
						static const GLUSchar* filenameTemplate = "screenshot-%04d.tga";
						static GLUSchar filename[20];

						sprintf(filename, filenameTemplate, _glusWindowGetCurrentAndIncreaseRecordingFrame());

						glusImageSaveTga(filename, _glusWindowGetRecordingImageTga());
					}
				}
				else
				{
					g_done = GLUS_TRUE;
				}
			}
		}

		eglSwapBuffers(g_eglDisplay, g_eglSurface); // Swap Buffers (Double Buffering)

		_glusOsPollEvents();
	}

	return !g_done;
}

GLUSvoid GLUSAPIENTRY glusWindowShutdown(GLUSvoid)
{
	// Terminate Game
	if (glusTerminate)
	{
		glusTerminate();
	}

	if (glusWindowIsRecording())
	{
		glusWindowStopRecording();
	}

	// Shutdown
	glusWindowDestroy(); // Destroy The Window
}

GLUSvoid GLUSAPIENTRY glusWindowSwapInterval(GLUSint interval)
{
	eglSwapInterval(g_eglDisplay, interval);
}

GLUSint GLUSAPIENTRY glusWindowGetWidth(GLUSvoid)
{
	return g_width;
}

GLUSint GLUSAPIENTRY glusWindowGetHeight(GLUSvoid)
{
	return g_height;
}

//

void* GLUSAPIENTRY glusExtensionGetFuncAddress(const GLUSchar* procname)
{
	return eglGetProcAddress(procname);
}

//

EGLDisplay GLUSAPIENTRY glusEGLGetDefaultDisplay(GLUSvoid)
{
	return g_eglDisplay;
}

EGLSurface GLUSAPIENTRY glusEGLGetDefaultSurface(GLUSvoid)
{
	return g_eglSurface;
}

EGLContext GLUSAPIENTRY glusEGLGetDefaultContext(GLUSvoid)
{
	return g_eglContext;
}
