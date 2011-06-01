/**
 * SystemWindowsClass
 * $Id$
 * \file gdemu_system_windows.h
 * \brief SystemWindowsClass
 * \date 2011-05-25 19:28GMT
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

#ifndef GDEMU_SYSTEM_WINDOWS_H
#define GDEMU_SYSTEM_WINDOWS_H
// #include <...>


#ifndef WINVER                  // Specifies that the minimum required platform is Windows XP.
#define WINVER 0x0510           // Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINNT            // Specifies that the minimum required platform is Windows XP.
#define _WIN32_WINNT 0x0510     // Change this to the appropriate value to target other versions of Windows.
#endif
#ifndef _WIN32_WINDOWS          // Specifies that the minimum required platform is Windows 98.
#define _WIN32_WINDOWS 0x0410   // Change this to the appropriate value to target Windows Me or later.
#endif
#ifndef _WIN32_IE               // Specifies that the minimum required platform is Internet Explorer 6.0.
#define _WIN32_IE 0x0600        // Change this to the appropriate value to target other versions of IE.
#endif


// Windows Headers
#include <windows.h>
#include <avrt.h>

// GDI+
#include <gdiplus.h>

// WASAPI
#include <objbase.h>
#include <mmreg.h>
#include <mmdeviceapi.h>
#include <audioclient.h>
#include <audiopolicy.h>

// C Headers
#include <cstdlib>
#include <tchar.h>

// STL Headers
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <fstream>
#include <iostream>
#include <cmath>

// DirectInput Headers
#define DIRECTINPUT_VERSION 0x0800
#include "dinput.h"


// TString
#ifdef _UNICODE
typedef std::wstring tstring;
typedef std::wstringbuf tstringbuf;
typedef std::wstringstream tstringstream;
#define tcout std::wcout
#define tcin std::wcin
#else
typedef std::string tstring;
typedef std::stringbuf tstringbuf;
typedef std::stringstream tstringstream;
#define tcout std::cout
#define tcin std::cin
#endif


namespace GDEMU {

/**
 * SystemWindowsClass
 * \brief SystemWindowsClass
 * \date 2011-05-25 19:28GMT
 * \author Jan Boon (Kaetemi)
 */
class SystemWindowsClass
{
private:
	HWND m_HWnd;

public:
	SystemWindowsClass() : m_HWnd(NULL) { }

	static tstring GetWin32ErrorString(DWORD dwError);
	static tstring GetWin32LastErrorString();
	static void Error(const tstring &message);
	static void Warning(const tstring &message);
	static void Debug(const tstring &message);
	static void ErrorWin32();
	static void ErrorHResult(HRESULT hr);

	inline void setHWnd(HWND hwnd) { m_HWnd = hwnd; }
	inline HWND getHWnd() { return m_HWnd; }

	static tstring ToTString(const std::string &s);
	static tstring ToTString(const std::wstring &s);
	static std::wstring ToWString(const tstring &s);
	static std::string ToAString(const tstring &s);

private:
	SystemWindowsClass(const SystemWindowsClass &);
	SystemWindowsClass &operator=(const SystemWindowsClass &);
	
}; /* class SystemWindowsClass */

extern SystemWindowsClass SystemWindows;

} /* namespace GDEMU */

#endif /* #ifndef GDEMU_SYSTEM_WINDOWS_H */

/* end of file */
