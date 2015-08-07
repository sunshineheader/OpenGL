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

#include <pthread.h>

#include <stdint.h>

#include <time.h>

#include <bcm_host.h>

#include <SDL/SDL.h>

extern GLUSvoid _glusWindowInternalClose(GLUSvoid);

extern GLUSvoid _glusWindowInternalKey(GLUSint key, GLUSint state);

extern GLUSvoid _glusWindowInternalMouse(GLUSint button, GLUSint action);

extern GLUSvoid _glusWindowInternalMouseWheel(GLUSint pos);

extern GLUSvoid _glusWindowInternalMouseMove(GLUSint x, GLUSint y);

// Display resolution changing

static pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;
static pthread_cond_t g_cond = PTHREAD_COND_INITIALIZER;
static GLUSboolean g_resizeDone = GLUS_FALSE;

static void glusOsResizeDone(void *callback_data, uint32_t reason, uint32_t param1, uint32_t param2)
{
	pthread_mutex_lock(&g_mutex);
	g_resizeDone = GLUS_TRUE;
	pthread_cond_signal(&g_cond);
	pthread_mutex_unlock(&g_mutex);
}

static void glusOsWaitResizeDone()
{
	GLUSboolean doBreak = GLUS_FALSE;

	struct timespec breakTime;
	clock_gettime(CLOCK_REALTIME, &breakTime);

	breakTime.tv_sec += 8;

	while (!doBreak)
	{
		pthread_mutex_lock(&g_mutex);
		if (pthread_cond_timedwait(&g_cond, &g_mutex, &breakTime) == 0)
		{
			doBreak = g_resizeDone;
		}
		else
		{
			doBreak = GLUS_TRUE;
		}
		pthread_mutex_unlock(&g_mutex);
	}
}

// Map, if possible, to GLFW keys

static int glusOsTranslateKey(SDLKey key)
{

	switch (key)
	{
		case SDLK_SPACE:
			return GLFW_KEY_SPACE;
		case SDLK_ESCAPE:
			return GLFW_KEY_ESCAPE;
		case SDLK_F1:
			return GLFW_KEY_F1;
		case SDLK_F2:
			return GLFW_KEY_F2;
		case SDLK_F3:
			return GLFW_KEY_F3;
		case SDLK_F4:
			return GLFW_KEY_F4;
		case SDLK_F5:
			return GLFW_KEY_F5;
		case SDLK_F6:
			return GLFW_KEY_F6;
		case SDLK_F7:
			return GLFW_KEY_F7;
		case SDLK_F8:
			return GLFW_KEY_F8;
		case SDLK_F9:
			return GLFW_KEY_F9;
		case SDLK_F10:
			return GLFW_KEY_F10;
		case SDLK_F11:
			return GLFW_KEY_F11;
		case SDLK_F12:
			return GLFW_KEY_F12;
		case SDLK_F13:
			return GLFW_KEY_F13;
		case SDLK_F14:
			return GLFW_KEY_F14;
		case SDLK_F15:
			return GLFW_KEY_F15;
		case SDLK_UP:
			return GLFW_KEY_UP;
		case SDLK_DOWN:
			return GLFW_KEY_DOWN;
		case SDLK_LEFT:
			return GLFW_KEY_LEFT;
		case SDLK_RIGHT:
			return GLFW_KEY_RIGHT;
		case SDLK_LSHIFT:
			return GLFW_KEY_LEFT_SHIFT;
		case SDLK_RSHIFT:
			return GLFW_KEY_RIGHT_SHIFT;
		case SDLK_LCTRL:
			return GLFW_KEY_LEFT_CONTROL;
		case SDLK_RCTRL:
			return GLFW_KEY_RIGHT_CONTROL;
		case SDLK_LALT:
			return GLFW_KEY_LEFT_ALT;
		case SDLK_RALT:
			return GLFW_KEY_RIGHT_ALT;
		case SDLK_TAB:
			return GLFW_KEY_TAB;
		case SDLK_RETURN:
			return GLFW_KEY_ENTER;
		case SDLK_BACKSPACE:
			return GLFW_KEY_BACKSPACE;
		case SDLK_INSERT:
			return GLFW_KEY_INSERT;
		case SDLK_DELETE:
			return GLFW_KEY_DELETE;
		case SDLK_PAGEUP:
			return GLFW_KEY_PAGE_UP;
		case SDLK_PAGEDOWN:
			return GLFW_KEY_PAGE_DOWN;
		case SDLK_HOME:
			return GLFW_KEY_HOME;
		case SDLK_END:
			return GLFW_KEY_END;
		case SDLK_KP0:
			return GLFW_KEY_KP_0;
		case SDLK_KP1:
			return GLFW_KEY_KP_1;
		case SDLK_KP2:
			return GLFW_KEY_KP_2;
		case SDLK_KP3:
			return GLFW_KEY_KP_3;
		case SDLK_KP4:
			return GLFW_KEY_KP_4;
		case SDLK_KP5:
			return GLFW_KEY_KP_5;
		case SDLK_KP6:
			return GLFW_KEY_KP_6;
		case SDLK_KP7:
			return GLFW_KEY_KP_7;
		case SDLK_KP8:
			return GLFW_KEY_KP_8;
		case SDLK_KP9:
			return GLFW_KEY_KP_9;
		case SDLK_KP_DIVIDE:
			return GLFW_KEY_KP_DIVIDE;
		case SDLK_KP_MULTIPLY:
			return GLFW_KEY_KP_MULTIPLY;
		case SDLK_KP_MINUS:
			return GLFW_KEY_KP_SUBTRACT;
		case SDLK_KP_PLUS:
			return GLFW_KEY_KP_ADD;
		case SDLK_KP_PERIOD:
			return GLFW_KEY_KP_DECIMAL;
		case SDLK_KP_EQUALS:
			return GLFW_KEY_KP_EQUAL;
		case SDLK_KP_ENTER:
			return GLFW_KEY_KP_ENTER;
		case SDLK_NUMLOCK:
			return GLFW_KEY_NUM_LOCK;
		case SDLK_CAPSLOCK:
			return GLFW_KEY_CAPS_LOCK;
		case SDLK_SCROLLOCK:
			return GLFW_KEY_SCROLL_LOCK;
		case SDLK_PAUSE:
			return GLFW_KEY_PAUSE;
		case SDLK_LSUPER:
			return GLFW_KEY_LEFT_SUPER;
		case SDLK_RSUPER:
			return GLFW_KEY_RIGHT_SUPER;
		case SDLK_MENU:
			return GLFW_KEY_MENU;
		default:
			return (int)key;
	}

	return GLFW_KEY_UNKNOWN;
}

//

static GLUSboolean gg_nativeWindowCreated = GLUS_FALSE;

static DISPMANX_DISPLAY_HANDLE_T g_nativeDisplay = 0;

static EGL_DISPMANX_WINDOW_T g_nativeWindow;

static GLUSint g_width = -1;

static GLUSint g_height = -1;

static GLUSint g_wheelPos = 0;

static GLUSboolean g_fullscreen = GLUS_FALSE;

GLUSvoid _glusOsPollEvents()
{
	SDL_Event event;

	while (SDL_PollEvent(&event))
	{
		switch (event.type)
		{
			case SDL_KEYDOWN:
			{
				// CTRL-C
				if (event.key.keysym.sym == 99 && (event.key.keysym.mod == KMOD_LCTRL || event.key.keysym.mod == KMOD_RCTRL))
				{
					_glusWindowInternalClose();

					return;
				}

				_glusWindowInternalKey(glusOsTranslateKey(event.key.keysym.sym), GLFW_PRESS);
			}
			break;

			case SDL_KEYUP:
			{
				_glusWindowInternalKey(glusOsTranslateKey(event.key.keysym.sym), GLFW_RELEASE);
			}
			break;

			case SDL_MOUSEMOTION:
			{
				if (event.motion.x <= g_width && event.motion.y <= g_height)
				{
					_glusWindowInternalMouseMove(event.motion.x, event.motion.y);
				}
			}
			break;

			case SDL_MOUSEBUTTONDOWN:
			{
				if (event.button.x <= g_width && event.button.y <= g_height)
				{
					switch (event.button.button)
					{
						case 1:
							_glusWindowInternalMouse(GLFW_MOUSE_BUTTON_LEFT, GLFW_PRESS);
						break;
						case 2:
							_glusWindowInternalMouse(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_PRESS);
						break;
						case 3:
							_glusWindowInternalMouse(GLFW_MOUSE_BUTTON_RIGHT, GLFW_PRESS);
						break;
						case 4:
							g_wheelPos += 1;
							_glusWindowInternalMouseWheel(g_wheelPos);
						break;
						case 5:
							g_wheelPos -= 1;
							_glusWindowInternalMouseWheel(g_wheelPos);
						break;
					}
				}
			}
			break;

			case SDL_MOUSEBUTTONUP:
			{
				if (event.button.x <= g_width && event.button.y <= g_height)
				{
					switch (event.button.button)
					{
						case 1:
							_glusWindowInternalMouse(GLFW_MOUSE_BUTTON_LEFT, GLFW_RELEASE);
						break;
						case 2:
							_glusWindowInternalMouse(GLFW_MOUSE_BUTTON_MIDDLE, GLFW_RELEASE);
						break;
						case 3:
							_glusWindowInternalMouse(GLFW_MOUSE_BUTTON_RIGHT, GLFW_RELEASE);
						break;
						case 4:
							g_wheelPos += 1;
							_glusWindowInternalMouseWheel(g_wheelPos);
						break;
						case 5:
							g_wheelPos -= 1;
							_glusWindowInternalMouseWheel(g_wheelPos);
						break;
					}
				}
			}
			break;
		}
	}
}

EGLNativeDisplayType _glusOsGetNativeDisplayType()
{
	return EGL_DEFAULT_DISPLAY ;
}

EGLNativeWindowType _glusOsCreateNativeWindowType(const char* title, const GLUSint width, const GLUSint height, const GLUSboolean fullscreen, const GLUSboolean noResize, const GLUSint nativeVisualID)
{
	const SDL_VideoInfo* videoInfo;

	//

	DISPMANX_UPDATE_HANDLE_T dispmanUpdate;
	DISPMANX_ELEMENT_HANDLE_T dispmanElement;
	VC_RECT_T dstRect;
	VC_RECT_T srcRect;
	VC_DISPMANX_ALPHA_T dispmanAlpha;
	int32_t success;
	int32_t windowWidth;
	int32_t windowHeight;

	glusLogPrint(GLUS_LOG_INFO, "Parameters 'title' and 'noResize' are not used");

	// Initialize graphics system

	bcm_host_init();

	// Set fullscreen, if wanted

	if (fullscreen)
	{
		const uint32_t MAX_SUPPORTED_MODES = 128;
		HDMI_RES_GROUP_T group = HDMI_RES_GROUP_DMT;
		TV_SUPPORTED_MODE_NEW_T supportedModes[MAX_SUPPORTED_MODES];
		int32_t i, numberSupportedModes;

		numberSupportedModes = vc_tv_hdmi_get_supported_modes_new(group, supportedModes, MAX_SUPPORTED_MODES, 0, 0);

		for (i = 0; i < numberSupportedModes; i++)
		{
			if (supportedModes[i].width == (uint32_t)width && supportedModes[i].height == (uint32_t)height && supportedModes[i].frame_rate >= 60)
			{
				break;
			}
		}

		if (i == numberSupportedModes)
		{
			glusLogPrint(GLUS_LOG_ERROR, "No matching display resolution found: %dx%d", width, height);

			return 0;
		}

		vc_tv_register_callback(glusOsResizeDone, 0);

		if (vc_tv_hdmi_power_on_explicit_new(group, supportedModes[i].group, supportedModes[i].code) != 0)
		{
			vc_tv_unregister_callback(glusOsResizeDone);

			glusLogPrint(GLUS_LOG_ERROR, "Could not switch to full screen: %dx%d", width, height);

			return 0;
		}

		glusOsWaitResizeDone();

		vc_tv_unregister_callback(glusOsResizeDone);

		windowWidth = width;
		windowHeight = height;

		g_fullscreen = GLUS_TRUE;
	}
	else
	{
		windowWidth = width;
		windowHeight = height;
	}

	//

	g_nativeDisplay = vc_dispmanx_display_open(0 /* LCD */);

	if (!g_nativeDisplay)
	{
		glusLogPrint(GLUS_LOG_ERROR, "Could not open display");

		return 0;
	}

	//

	if (SDL_Init(SDL_INIT_VIDEO) != 0)
	{
		glusLogPrint(GLUS_LOG_ERROR, "Could not initialize SDL");

		return 0;
	}

	videoInfo = SDL_GetVideoInfo();

	if (!videoInfo)
	{
		glusLogPrint(GLUS_LOG_ERROR, "Could not get video info for SDL");

		return 0;
	}

	if (!SDL_SetVideoMode(videoInfo->current_w, videoInfo->current_h, videoInfo->vfmt->BitsPerPixel, SDL_HWSURFACE))
	{
		glusLogPrint(GLUS_LOG_ERROR, "Set video mode for SDL failed");

		return 0;
	}

	SDL_ShowCursor(SDL_DISABLE);

	//

	dstRect.x = 0;
	dstRect.y = 0;
	dstRect.width = windowWidth;
	dstRect.height = windowHeight;

	srcRect.x = 0;
	srcRect.y = 0;
	srcRect.width = windowWidth << 16;
	srcRect.height = windowHeight << 16;

	dispmanAlpha.flags = DISPMANX_FLAGS_ALPHA_FIXED_ALL_PIXELS;
	dispmanAlpha.mask = 0xFFFFFFFF;
	dispmanAlpha.opacity = 255;

	dispmanUpdate = vc_dispmanx_update_start(0);

	dispmanElement = vc_dispmanx_element_add(dispmanUpdate, g_nativeDisplay, 0 /*layer*/, &dstRect, 0 /*src*/, &srcRect, DISPMANX_PROTECTION_NONE, &dispmanAlpha, 0/*clamp*/, 0/*transform*/);

	success = vc_dispmanx_update_submit_sync(dispmanUpdate);

	if (!dispmanElement || success < 0)
	{
		glusLogPrint(GLUS_LOG_ERROR, "Could not add element");

		return 0;
	}

	g_width = windowWidth;
	g_height = windowHeight;

	g_nativeWindow.element = dispmanElement;
	g_nativeWindow.width = windowWidth;
	g_nativeWindow.height = windowHeight;

	gg_nativeWindowCreated = GLUS_TRUE;

	return (EGLNativeWindowType)&g_nativeWindow;
}

GLUSvoid _glusOsDestroyNativeWindowDisplay()
{
	if (gg_nativeWindowCreated)
	{
		DISPMANX_UPDATE_HANDLE_T dispmanUpdate;

		dispmanUpdate = vc_dispmanx_update_start(0);
		vc_dispmanx_element_remove(dispmanUpdate, g_nativeWindow.element);
		vc_dispmanx_update_submit_sync(dispmanUpdate);

		gg_nativeWindowCreated = GLUS_FALSE;
	}
	memset(&g_nativeWindow, 0, sizeof(g_nativeWindow));

	if (g_nativeDisplay)
	{
		vc_dispmanx_display_close(g_nativeDisplay);

		g_nativeDisplay = 0;
	}

	if (g_fullscreen)
	{
		vc_tv_register_callback(glusOsResizeDone, 0);

		if (vc_tv_hdmi_power_on_preferred() == 0)
		{
			glusOsWaitResizeDone();
		}

		vc_tv_unregister_callback(glusOsResizeDone);

		g_fullscreen = GLUS_FALSE;
	}

	SDL_ShowCursor(SDL_ENABLE);

	SDL_Quit();

	bcm_host_deinit();
}

double _glusOsGetRawTime()
{
	struct timespec currentTime;

	clock_gettime(CLOCK_MONOTONIC, &currentTime);

	return (double)currentTime.tv_sec + (double)currentTime.tv_nsec / 1000000000.0;
}

GLUSvoid _glusOsGetWindowSize(GLUSint* width, GLUSint* height)
{
	if (width)
	{
		*width = g_width;
	}

	if (height)
	{
		*height = g_height;
	}
}
