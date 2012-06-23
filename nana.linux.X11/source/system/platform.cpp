/*
 *	A platform API implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/system/platform.cpp
 *	@description:
 *		this implements some API for platform-independent programming
 */
#include <nana/macros.hpp>

#if defined(NANA_WINDOWS)
	#include <windows.h>
#elif defined(NANA_LINUX)
	#include <time.h>
	#include <errno.h>
	#include <sys/time.h>
	#include <sys/syscall.h>
#endif

namespace nana
{
namespace system
{
	/*
	 * sleep
	 *	suspend current thread for a specified milliseconds.
	 *	its precision is depended on hardware.
	 */
	void sleep(unsigned milliseconds)
	{
#if defined(NANA_WINDOWS)
		::Sleep(milliseconds);
#elif defined(NANA_LINUX)
		struct timespec timeOut, remains;
		timeOut.tv_sec = milliseconds / 1000;
		timeOut.tv_nsec = (milliseconds % 1000) * 1000000;
		while(-1 == ::nanosleep(&timeOut, &remains))
		{
			if(errno == EINTR)
				timeOut = remains;
			else
				break;
		}
#endif
	}

	/*
	 *	this_thread_id
	 *	@brief: get the identifier of calling thread.
	 */
	unsigned long this_thread_id()
	{
#if defined(NANA_WINDOWS)
		return ::GetCurrentThreadId();
#elif defined(NANA_LINUX)
		return ::syscall(__NR_gettid);
#endif
	}

	unsigned long timestamp()
	{
#if defined(NANA_WINDOWS)
		return ::GetTickCount();
#elif defined(NANA_LINUX)
		struct timeval tv;
		::gettimeofday(&tv, 0);
		return (tv.tv_sec * 1000 + tv.tv_usec / 1000);
#endif
	}

	bool get_async_mouse_state(int button)
	{
#if defined(NANA_WINDOWS)
		bool swap = (::GetSystemMetrics(SM_SWAPBUTTON) != 0);
		switch(button)
		{
		case 1: //Right
			button = swap ? VK_LBUTTON : VK_RBUTTON;
			break;
		case 2:
			button = VK_MBUTTON;
			break;
		default:
			button = swap ? VK_RBUTTON : VK_LBUTTON;
			break;
		}

		return (::GetAsyncKeyState(button) != 0);
#elif defined(NANA_LINUX)
		return false;
#endif
	}

	//open an url through a default browser
	void open_url(const nana::string& url)
	{
#if defined(NANA_WINDOWS)
		if(::ShellExecute(0, STR("open"), url.c_str(), 0, 0, SW_SHOWNORMAL) < reinterpret_cast<HINSTANCE>(32))
		{
			//Because ShellExecute can delegate execution to Shell extensions (data sources, context menu handlers,
			//verb implementations) that are activated using Component Object Model (COM), COM should be initialized
			//before ShellExecute is called. Some Shell extensions require the COM single-threaded apartment (STA) type.
			//In that case, COM should be initialized under WinXP.
			typedef HRESULT (__stdcall *CoInitializeEx_t)(LPVOID, DWORD);
			typedef void (__stdcall *CoUninitialize_t)(void);
			HMODULE md =::LoadLibrary(STR("Ole32.DLL"));
			if(md == 0) return;

			CoInitializeEx_t fn_init = reinterpret_cast<CoInitializeEx_t>(::GetProcAddress(md, "CoInitializeEx"));
			CoUninitialize_t fn_unin = reinterpret_cast<CoUninitialize_t>(::GetProcAddress(md, "CoUninitialize"));
			if(fn_init && fn_unin)
			{
				fn_init(0, COINIT_APARTMENTTHREADED | /*COINIT_DISABLE_OLE1DDE =*/0x4);
				::ShellExecute(0, STR("open"), url.c_str(), 0, 0, SW_SHOWNORMAL);
				fn_unin();
			}
			::FreeLibrary(md);
		}
#elif defined(NANA_LINUX)
#endif
	}
}//end namespace system
}//end namespace nana
