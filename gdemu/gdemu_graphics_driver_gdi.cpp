/**
 * GraphicsDriverClass
 * $Id$
 * \file gdemu_graphics_driver_gdi.cpp
 * \brief GraphicsDriverClass
 * \date 2011-05-26 18:14GMT
 * \author Jan Boon (Kaetemi)
 */

/* 
 * Copyright (C) 2011  Jan Boon (Kaetemi)
 * 
 * This file is part of GAMEDUINO EMULATOR.
 * GAMEDUINO EMULATOR is free software: you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation, either version 2 of the
 * License, or (at your option) any later version.
 * 
 * GAMEDUINO EMULATOR is distributed in the hope that it will be
 * useful, but WITHOUT ANY WARRANTY; without even the implied warranty
 * of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with GAMEDUINO EMULATOR; see the file COPYING.  If not, see
 * <http://www.gnu.org/licenses/>.
 */

// #include <...>
#include "gdemu_graphics_driver.h"

// System includes
#include "gdemu_system_windows.h"
#include "gdemu_system.h"
#include "gdemu_gameduino_spi.h"

// Project includes

using namespace std;

#define GDEMU_WINDOW_TITLE TEXT("Gameduino Emulator")
#define GDEMU_WINDOW_WIDTH 400
#define GDEMU_WINDOW_HEIGHT 300
#define GDEMU_WINDOW_RATIO (4.0f / 3.0f)
#define GDEMU_WINDOW_KEEPRATIO (true)

#define GDEMU_WINDOW_CLASS_NAME TEXT("GDEMUGraphicsDriver")

namespace GDEMU {


GraphicsDriverClass GraphicsDriver;
static argb1555 s_BufferARGB1555[GDEMU_WINDOW_WIDTH * GDEMU_WINDOW_HEIGHT];



static HINSTANCE s_HInstance = NULL;
static HWND s_HWnd = NULL;
//static int s_ViewWidth, s_ViewHeight;
//static float s_AspectRatio;
//static bool s_KeepRatio;
static std::map<UINT, WNDPROC> s_WindowProcedures;
//static ULONG_PTR s_GdiplusToken;
static HBITMAP s_Buffer = NULL;
static HDC s_HDC = NULL;//, m_WindowGraphics;
//static Gdiplus::Graphics *m_GraphicsPtr;



BITMAPINFO s_BitInfo;



argb1555 *GraphicsDriverClass::getBufferARGB1555()
{
	return s_BufferARGB1555;
}

static LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	//int wmId, wmEvent;
	PAINTSTRUCT ps;
	HDC hdc;

	switch (message)
	{
	case WM_PAINT:
		hdc = BeginPaint(hWnd, &ps);
		// TODO: Add any drawing code here...
		/*testpaint(hdc);*/
		EndPaint(hWnd, &ps);
		break;
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_CLOSE:
		s_HWnd = NULL;
		break;
	case WM_ERASEBKGND:
		return (LRESULT)1; // Say we handled it.
	case WM_SYSCOMMAND:
		if (wParam == SC_KEYMENU) return 0;
		return DefWindowProc(hWnd, message, wParam, lParam); 
	default:
		map<UINT, WNDPROC>::iterator it = s_WindowProcedures.find(message);
		if (it != s_WindowProcedures.end())
			return it->second(hWnd, message, wParam, lParam);
		return DefWindowProc(hWnd, message, wParam, lParam);
	}
	return (LRESULT)0;
}

void GraphicsDriverClass::begin()
{
	// Save params
	s_HInstance = GetModuleHandle(NULL);

	// Register Display Class
	WNDCLASSEX wcex;
	wcex.cbSize = sizeof(WNDCLASSEX);
	wcex.style = CS_HREDRAW | CS_VREDRAW;
	wcex.lpszClassName = GDEMU_WINDOW_CLASS_NAME;
	wcex.lpfnWndProc = WndProc;
	wcex.hInstance = s_HInstance;
	wcex.cbClsExtra = 0;
	wcex.cbWndExtra = 0;
	wcex.lpszMenuName = NULL;
	wcex.hCursor = LoadCursor(NULL, IDC_ARROW);
	wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
	//wcex.hIcon = LoadIcon(m_HInstance, MAKEINTRESOURCE(IDI_JAZZ));
	//wcex.hIconSm = LoadIcon(m_HInstance, MAKEINTRESOURCE(IDI_SMALL));
	wcex.hIcon = NULL;
	wcex.hIconSm = NULL;
	if (!RegisterClassEx(&wcex)) SystemWindows.ErrorWin32();

	// Initialize and Show Display Instance
	DWORD dw_style = WS_OVERLAPPEDWINDOW;
/*#if GDEMUWIN_DISPLAY_ASPECT_RATIO
	RECT r; r.top = 0; r.left = 0; r.bottom = (LONG)(((float)GDEMU_WINDOW_WIDTH / aspectRatio) * 2); r.right = GDEMU_WINDOW_WIDTH * 2; // window size
#else*/
	RECT r; r.top = 0; r.left = 0; r.bottom = GDEMU_WINDOW_HEIGHT * 2; r.right = GDEMU_WINDOW_WIDTH * 2; // window size
/*#endif*/
	AdjustWindowRect(&r, dw_style, FALSE);
	if (s_HWnd) SystemWindows.Error(TEXT("GraphicsDriver.begin()  s_HWnd != NULL"));
	if (!(s_HWnd = CreateWindow(GDEMU_WINDOW_CLASS_NAME, 
		/*(LPCTSTR)title.c_str()*/GDEMU_WINDOW_TITLE, dw_style, 
		CW_USEDEFAULT, 0, r.right - r.left, r.bottom - r.top, // x y w h
		NULL, NULL, s_HInstance, NULL))) 
		SystemWindows.ErrorWin32();
	ShowWindow(s_HWnd, /*nCmdShow*/ true); // If the window was previously visible, the return value is nonzero.
	if (!UpdateWindow(s_HWnd)) SystemWindows.ErrorWin32();

	// Create GDI32 Buffer and Device Context
	HDC hdc = GetDC(s_HWnd);
	if (s_HDC) SystemWindows.Error(TEXT("GraphicsDriver.begin()  s_HDC != NULL"));
	s_HDC = CreateCompatibleDC(hdc);
	if (!s_HDC) SystemWindows.Error(TEXT("GraphicsDriver.begin()  s_HDC == NULL\r\n") + SystemWindows.GetWin32LastErrorString());
	if (s_Buffer) SystemWindows.Error(TEXT("GraphicsDriver.begin()  s_Buffer != NULL"));
	s_Buffer = CreateCompatibleBitmap(hdc, GDEMU_WINDOW_WIDTH, GDEMU_WINDOW_HEIGHT);
	if (!s_Buffer) SystemWindows.Error(TEXT("GraphicsDriver.begin()  s_Buffer == NULL\r\n") + SystemWindows.GetWin32LastErrorString());
	SelectObject(s_HDC, s_Buffer);
	ReleaseDC(s_HWnd, hdc);

	// Verify Maps
	//if (!Bitmap::m_HBitmaps.empty()) SystemWindows.Error(TEXT("GraphicsDriver.begin()  Bitmap::m_HBitmaps.empty() == false"));
	if (!s_WindowProcedures.empty()) SystemWindows.Error(TEXT("GraphicsDriver.begin()  s_WindowProcedures.empty() == false"));

	// Initialize Bitmap Buffer
	ZeroMemory(&s_BitInfo, sizeof(s_BitInfo));
    s_BitInfo.bmiHeader.biSize = sizeof(s_BitInfo.bmiHeader);
	s_BitInfo.bmiHeader.biWidth = GDEMU_WINDOW_WIDTH;
    s_BitInfo.bmiHeader.biHeight = GDEMU_WINDOW_HEIGHT;
    s_BitInfo.bmiHeader.biPlanes = 1;
    s_BitInfo.bmiHeader.biBitCount = 16; // was 24
    s_BitInfo.bmiHeader.biCompression = BI_RGB;

	// Meh
	SystemWindows.setHWnd(s_HWnd);
}

bool GraphicsDriverClass::update()
{
	// Update Window Messages
	MSG msg;
	while (PeekMessage(&msg, /*m_HWnd*/ NULL, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return s_HWnd != NULL;
}

void GraphicsDriverClass::end()
{
	SystemWindows.setHWnd(NULL);

	if (!s_WindowProcedures.empty()) 
	{ 
		SystemWindows.Debug(TEXT("GraphicsDriver.end() s_WindowProcedures.empty() == false"));
		s_WindowProcedures.clear();
	}

	if (s_HDC) { DeleteDC(s_HDC); s_HDC = NULL; }
	else SystemWindows.Debug(TEXT("GraphicsDriver.end() s_HDC == NULL"));
	if (s_Buffer) { DeleteObject(s_Buffer); s_Buffer = NULL; }
	else SystemWindows.Debug(TEXT("GraphicsDriver.end() s_Buffer == NULL"));

	if (s_HWnd) { DestroyWindow(s_HWnd); s_HWnd = NULL; }
	else SystemWindows.Debug(TEXT("GraphicsDriver.end() s_HWnd == NULL"));

	UnregisterClass(GDEMU_WINDOW_CLASS_NAME, s_HInstance);
}

void GraphicsDriverClass::renderBuffer()
{
	// Render bitmap to buffer
	if (!SetDIBitsToDevice(s_HDC, 0, 0, 
		GDEMU_WINDOW_WIDTH, GDEMU_WINDOW_HEIGHT, 
		0, 0, 0, GDEMU_WINDOW_HEIGHT, s_BufferARGB1555, &s_BitInfo, DIB_RGB_COLORS))
		SystemWindows.Error(TEXT("SetDIBitsToDevice  FAILED"));

	// Draw buffer to screen
	RECT r;
	GetClientRect(s_HWnd, &r);
	if (GDEMU_WINDOW_KEEPRATIO)
	{
		argb1555 bgC16 = ((argb1555 *)(void *)(&GameduinoSPI.getRam()[0x280e]))[0];
		COLORREF bgC32 = RGB((((bgC16) & 0x1F) * 255 / 31),
			(((bgC16 >> 5) & 0x1F) * 255 / 31),
			(((bgC16 >> 10) & 0x1F) * 255 / 31));
		HBRUSH bgBrush = CreateSolidBrush(bgC32);
		if (bgBrush == NULL) SystemWindows.ErrorWin32();
		int width_r = (int)((float)r.bottom * GDEMU_WINDOW_RATIO); int height_r;
		if (width_r > r.right) { width_r = r.right; height_r = (int)((float)r.right / GDEMU_WINDOW_RATIO); }
		else height_r = r.bottom;
		int x_r = (r.right - width_r) / 2;
		int y_r = (r.bottom - height_r) / 2;
		HDC hdc = GetDC(s_HWnd);		
		StretchBlt(hdc, x_r, y_r, width_r, height_r, s_HDC, 0, 0, GDEMU_WINDOW_WIDTH, GDEMU_WINDOW_HEIGHT, SRCCOPY);
		RECT rect;
		if (x_r > 0)
		{
			rect.top = 0; rect.left = 0;
			rect.top = 0; rect.left = 0;
			rect.right = (r.right - width_r) / 2;
			rect.bottom = r.bottom;
			FillRect(hdc, &rect, bgBrush); // (HBRUSH)(COLOR_WINDOW + 1));
			rect.left = rect.right + width_r;
			rect.right += rect.left;
			FillRect(hdc, &rect, bgBrush); // (HBRUSH)(COLOR_WINDOW + 1));
		}
		if (y_r > 0)
		{
			rect.top = 0; rect.left = 0;
			rect.right = r.right;
			rect.bottom = (r.bottom - height_r) / 2;
			FillRect(hdc, &rect, bgBrush); // (HBRUSH)(COLOR_WINDOW + 1));
			rect.top = rect.bottom + height_r;
			rect.bottom += rect.top;
			FillRect(hdc, &rect, bgBrush); // (HBRUSH)(COLOR_WINDOW + 1));
		}
		ReleaseDC(s_HWnd, hdc);
		if (!DeleteObject(bgBrush)) SystemWindows.ErrorWin32();
	}
	else
	{
		HDC hdc = GetDC(s_HWnd);		
		StretchBlt(hdc, 0, 0, r.right, r.bottom, s_HDC, 0, 0, GDEMU_WINDOW_WIDTH, GDEMU_WINDOW_HEIGHT, SRCCOPY);
		ReleaseDC(s_HWnd, hdc);
	}

	// Update title
	tstringstream newTitle;
	newTitle << GDEMU_WINDOW_TITLE;
	if (GameduinoSPI.getRam()[0x2809] == 0)
		newTitle << TEXT(" [+J1]");
	newTitle << TEXT(" [FPS: ");
	newTitle << System.getFPSSmooth();
	newTitle << TEXT(" (");
	newTitle << System.getFPS();
	newTitle << TEXT(")]");
	SetWindowText(s_HWnd, (LPCTSTR)newTitle.str().c_str());
}

/*
// SET TO NULL WHEN NO LONGER NEEDED
void setWindowProcedure(UINT message, WNDPROC procedure)
{
	map<UINT, WNDPROC>::iterator it = s_WindowProcedures.find(message);
	if (procedure)
	{
		if (it != s_WindowProcedures.end())
			SystemWindows.Debug(TEXT("setWindowProcedure  WindowProcedure already exists, replacing"));
		s_WindowProcedures[message] = procedure;
	}
	else
	{
		if (it == s_WindowProcedures.end())
			SystemWindows.Debug(TEXT("setWindowProcedure  WindowProcedure does not exist"));
		else s_WindowProcedures.erase(it);
	}
}
*/

} /* namespace GDEMU */

/* end of file */
