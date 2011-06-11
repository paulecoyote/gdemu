/**
 * SystemWindowsClass
 * $Id$
 * \file gdemu_system_windows.cpp
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

// #include <...>
#include "gdemu_system_windows.h"
#include "gdemu_system.h"

// Libraries
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "avrt.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "msimg32.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "dinput8.lib")

// System includes

// Project includes

using namespace std;

namespace GDEMU {


SystemClass System;
SystemWindowsClass SystemWindows;


static LARGE_INTEGER s_PerformanceFrequency = { 0 };
static LARGE_INTEGER s_PerformanceCounterBegin = { 0 };


//static HANDLE s_J1Thread = NULL;
static HANDLE s_DuinoThread = NULL;
static HANDLE s_MainThread = NULL;

//static CRITICAL_SECTION s_CriticalSection;


void SystemClass::_begin()
{
	QueryPerformanceFrequency(&s_PerformanceFrequency);
	QueryPerformanceCounter(&s_PerformanceCounterBegin);
	//InitializeCriticalSection(&s_CriticalSection);
}

void SystemClass::_update()
{
	
}

void SystemClass::_end()
{
	//DeleteCriticalSection(&s_CriticalSection);
}
/*
void SystemClass::enterCriticalSection()
{
	//EnterCriticalSection(&s_CriticalSection);
}

void SystemClass::leaveCriticalSection()
{
	//LeaveCriticalSection(&s_CriticalSection);
}
*/
void SystemClass::disableAutomaticPriorityBoost()
{
	SetThreadPriorityBoost(GetCurrentThread(), TRUE);
}
void SystemClass::makeLowPriorityThread()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_LOWEST);
}
void SystemClass::makeNormalPriorityThread()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_NORMAL);
}

void SystemClass::makeHighPriorityThread()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);
}

void SystemClass::makeHighestPriorityThread()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_HIGHEST);
}

void SystemClass::makeRealtimePriorityThread()
{
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_TIME_CRITICAL);
}

void SystemClass::makeMainThread()
{
	if (!DuplicateHandle(
		GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&s_MainThread,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS))
		SystemWindows.ErrorWin32();
}

bool SystemClass::isMainThread()
{
	HANDLE currentThread;
	if (!DuplicateHandle(
		GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&currentThread,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS))
		SystemWindows.ErrorWin32();
	return currentThread == s_MainThread;
}

// Duino thread control
void SystemClass::makeDuinoThread()
{
	if (!DuplicateHandle(
		GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&s_DuinoThread,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS))
		SystemWindows.ErrorWin32();
}

bool SystemClass::isDuinoThread()
{
	HANDLE currentThread;
	if (!DuplicateHandle(
		GetCurrentProcess(),
		GetCurrentThread(),
		GetCurrentProcess(),
		&currentThread,
		0,
		TRUE,
		DUPLICATE_SAME_ACCESS))
		SystemWindows.ErrorWin32();
	return currentThread == s_DuinoThread;
}

void SystemClass::prioritizeDuinoThread()
{
	if (s_DuinoThread != NULL)
		SetThreadPriority(s_DuinoThread, THREAD_PRIORITY_HIGHEST);
}

void SystemClass::unprioritizeDuinoThread()
{
	if (s_DuinoThread != NULL)
		SetThreadPriority(s_DuinoThread, THREAD_PRIORITY_NORMAL);
}

void SystemClass::holdDuinoThread()
{
	if (0 > SuspendThread(s_DuinoThread))
		SystemWindows.Error(TEXT("SuspendThread  FAILED"));
}

void SystemClass::resumeDuinoThread()
{
	if (0 > ResumeThread(s_DuinoThread))
		SystemWindows.Error(TEXT("ResumeThread  FAILED"));
}



/*
void SystemClass::prioritizeJ1Thread()
{
	if (s_J1Thread != NULL)
		SetThreadPriority(s_J1Thread, THREAD_PRIORITY_HIGHEST);
}

void SystemClass::unprioritizeJ1Thread()
{
	if (s_J1Thread != NULL)
		SetThreadPriority(s_J1Thread, THREAD_PRIORITY_NORMAL);
}

void SystemClass::makeJ1Thread()
{
	s_J1Thread = GetCurrentThread();
}
*/

void *SystemClass::setThreadGamesCategory(unsigned long *refId)
{
	HANDLE h = AvSetMmThreadCharacteristics(TEXT("Games"), refId);
	if (!h) SystemWindows.ErrorWin32();
	return h;
}

void SystemClass::revertThreadCategory(void *taskHandle)
{
	AvRevertMmThreadCharacteristics(taskHandle);
}

void SystemClass::switchThread()
{
	SwitchToThread();
}

double SystemClass::getSeconds()
{
	LARGE_INTEGER counter;
	QueryPerformanceCounter(&counter);
	return (double)(counter.QuadPart - s_PerformanceCounterBegin.QuadPart) / (double)s_PerformanceFrequency.QuadPart;
}

long SystemClass::getMillis()
{
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	c.QuadPart -= s_PerformanceCounterBegin.QuadPart;
	c.QuadPart *= (LONGLONG)1000;
	c.QuadPart /= s_PerformanceFrequency.QuadPart;
	return c.QuadPart;
}

long SystemClass::getMicros()
{
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	c.QuadPart -= s_PerformanceCounterBegin.QuadPart;
	c.QuadPart *= (LONGLONG)1000000;
	c.QuadPart /= s_PerformanceFrequency.QuadPart;
	return c.QuadPart;
}

long SystemClass::getFreqTick(int hz)
{
	LARGE_INTEGER c;
	QueryPerformanceCounter(&c);
	c.QuadPart -= s_PerformanceCounterBegin.QuadPart;
	c.QuadPart *= (LONGLONG)hz;
	c.QuadPart /= s_PerformanceFrequency.QuadPart;
	return c.QuadPart;
}

void SystemClass::delay(int ms)
{
	Sleep(ms);
}

void SystemClass::delayMicros(int us)
{
	long endMicros = getMicros() + (long)us;;
	do
	{
		switchThread();
	} while (getMicros() < endMicros); 
	//Sleep(us / 1000);
}



tstring SystemWindowsClass::GetWin32ErrorString(DWORD dwError)
{
	// convert win32 error number to string

	LPTSTR lpMsgBuf;

    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER | 
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        dwError,
        MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        (LPTSTR) &lpMsgBuf,
        0, NULL );

	tstring result = tstring(lpMsgBuf);

    LocalFree(lpMsgBuf);

	return result;
}

tstring SystemWindowsClass::GetWin32LastErrorString()
{
	// put the last win32 error in a string and add the error number too
	DWORD dwError = GetLastError();
	tstringstream buffer;
	buffer << GetWin32ErrorString(dwError) 
		<< TEXT(" (error: ") << dwError << ")";
	return buffer.str();
}

void SystemWindowsClass::Error(const tstring &message)
{
	// exit with message
	MessageBox(NULL, (LPCTSTR)message.c_str(), TEXT("Error"), MB_OK | MB_ICONERROR);
	tcout << TEXT("Error: ") << message << endl;
	exit(EXIT_FAILURE);
}

void SystemWindowsClass::Warning(const tstring &message)
{
	// show a warning box and send to output
	MessageBox(NULL, (LPCTSTR)message.c_str(), TEXT("Warning"), MB_OK | MB_ICONWARNING);
	tcout << TEXT("Warning: ") << message << endl;
}

void SystemWindowsClass::Debug(const tstring &message)
{
	// send a debug to output
	tcout << TEXT("Debug: ") << message << endl;
}

void SystemWindowsClass::ErrorWin32()
{
	// crash with last win32 error string
	Error(GetWin32LastErrorString());
}

void SystemWindowsClass::ErrorHResult(HRESULT hr)
{
	Error(TEXT("ErrorHResult")); // fixme :p
}

#ifdef _UNICODE
tstring SystemWindowsClass::ToTString(const std::wstring &s) { return s; }
tstring SystemWindowsClass::ToTString(const std::string &s)
#else
tstring SystemWindowsClass::ToTString(const std::string &s) { return s; }
tstring SystemWindowsClass::ToTString(const std::wstring &s)
#endif
{
	tstring result(s.length(), 0);
	// copy from one to another
	std::copy(s.begin(), s.end(), result.begin());
	return result;
}

#ifdef _UNICODE
std::wstring SystemWindowsClass::ToWString(const tstring &s) { return s; }
std::string SystemWindowsClass::ToAString(const tstring &s)
#else
std::string SystemWindowsClass::ToAString(const tstring &s) { return s; }
std::wstring SystemWindowsClass::ToWString(const tstring &s)
#endif
{
#ifdef _UNICODE
	string
#else
	wstring
#endif
		result(s.length(), 0);
	// copy from one to another
	std::copy(s.begin(), s.end(), result.begin());
	return result;
}


} /* namespace GDEMU */

/* end of file */
