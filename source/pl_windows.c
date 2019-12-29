#ifndef PLATFORM_H
#define PLATFORM_H

#include "types.h"
#include "keys.h"
#include "platform.h"

#include <windows.h>

//Globals
int win_w, win_h, grabbed;

//Windowing
HWND win;
HDC dc;
HGLRC context;
HCURSOR cursor, no_cursor;
//Input
int grabbed;

//Windowing
void pl_create_window(int w, int h, int fullscr)
{
  win = CreateWindowExA(
    0,
    "Coolmaze3D",
    "",
    WS_CLIPCHILDREN | WS_CLIPSIBLINGS | WS_VISIBLE,
    0, 0, w, h,
    NULL,
    NULL,
    GetModuleHandle(NULL),//instance
    NULL
  );
  
  //OpenGL
  dc = GetDC(win);
  PIXELFORMATDESCRIPTOR pfd = {
    .nSize = sizeof(PIXELFORMATDESCRIPTOR),
    .nVersion = 1,
    .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
    .iPixelType = PFD_TYPE_RGBA,
    .cColorBits = 32,
    .cDepthBits = 24,
    .cStencilBits = 8,
    .iLayerType = PFD_MAIN_PLANE
  };
  int pf = ChoosePixelFormat(dc, &pfd);
  SetPixelFormat(dc, pf, &pfd);
  context = wglCreateContext(dc);
}

void pl_destroy_window(void)
{
  DestroyWindow(win);
}

void pl_swap_window(void)
{
  wglSwapLayerBuffers(dc, WGL_SWAP_MAIN_PLANE);
}

//Input
void pl_grab_input(void)
{
  grabbed = 1;
  SetCapture(win);
  SetFocus(win);
  ShowCursor(FALSE);
}

void pl_release_input(void)
{
  grabbed = 0;
  SetCursor(cursor);
  ShowCursor(TRUE);
}


//Events
//TODO factor this out
struct {
  key_callback_t fn;
  void * arg;
} keymap[NUM_KEYS];

void pl_bind_key(keycode_t k, key_callback_t kc, void * arg)
{
  keymap[k].fn = kc;
  keymap[k].arg = arg;
}

void pl_unbind_key(keycode_t k)
{
  keymap[k].fn = NULL;
  keymap[k].arg = NULL;
}

struct {
  mouse_callback_t fn;
  void * arg;
} mouse;

void pl_bind_mouse(mouse_callback_t mc, void * arg)
{
  mouse.fn = mc;
  mouse.arg = arg;
}

void pl_unbind_mouse(mouse_callback_t mc, void * arg)
{
  mouse.fn = NULL;
  mouse.arg = NULL;
}

void pl_poll_events(void)
{
  MSG msg;
  while(PeekMessage(&msg, win, WM_KEYFIRST, WM_MOUSELAST, PM_REMOVE) != 0) {
    int down = 0;
    int key;
    switch(msg.message){
      case WM_KEYDOWN:
      case WM_MBUTTONDOWN:
        down = 1;
      case WM_KEYUP:
      case WM_MBUTTONUP:
        key = keycode_from_keysym(msg.wParam);
        if(key != KEY_INVALID && keymap[key].fn){
          keymap[key].fn(down, keymap[key].arg);
        }
        break;
    }
  }

  //Mouse
  POINT p;
  GetCursorPos(&p);
  ScreenToClient(win, &p);
}

//Timing
float pl_mstime(void)
{
}

//TODO fix this shit win8+ only
float pl_deltat(void)
{
  static long pcval, lastval, freq;
  lastval = pcval;
  QueryPerformanceFrequency((LARGE_INTEGER*)&freq);
  QueryPerformanceCounter((LARGE_INTEGER*)&pcval);
  return 1000.0 * (pcval - lastval) / (float) (freq);
}

void pl_sleep(void)
{
  Sleep(0);
}

//Audio
typedef void(*audio_work_t)(void *arg);

void pl_create_stream(audio_work_t work)
{
}

void pl_send_stream(int nsamples, short *samples)
{
}

#endif
