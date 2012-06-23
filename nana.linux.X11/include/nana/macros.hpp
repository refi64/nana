/*
 *	Marco Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/marcos.hpp
 *
 *	What follow are dependented on what defined in nana/config.hpp
 */

#ifndef NANA_MACROS_HPP
#define NANA_MACROS_HPP

#include "config.hpp"

#if defined(NANA_LINUX)
#undef NANA_WINDOWS
#endif

#include <sstream>
#ifndef NANA_UNICODE
	namespace nana
	{
		typedef char		char_t;
		typedef std::string string;
	}

	#define STR(string)	string
#else
	#ifndef UNICODE
		#define UNICODE
	#endif
	namespace nana
	{
		typedef wchar_t			char_t;
		typedef std::wstring	string;
	}
	#define STR(string) L##string
#endif

#if defined(NANA_CONCURRENT)
	#include "threads/locks.hpp"
	#define NANA_MAKE_LOCK(Lock)	(Lock).lock();
	#define NANA_MAKE_UNLOCK(Lock)	(Lock).unlock();
	#define NANA_SCOPE_GUARD(Lock)	nana::threads::scope_guard scope_guard##Lock(Lock);

	#define NANA_CONCURRENT_EXEC(Expression)	Expression;
	#define NANA_CONCURRENT_DECLARE(Type, Object)	Type	Object;
	#define NANA_CONCURRENT_STATIC_DECLARE(Type, Object)	static	Type	Object;
	#define NANA_CONCURRENT_STATIC_DEFINE(Type, Parent, Object)	Type Parent::Object;
#else
	#define NANA_MAKE_LOCK(Lock)
	#define NANA_MAKE_UNLOCK(Lock)
	#define NANA_SCOPE_GUARD(Lock)

	#define NANA_CONCURRENT_EXEC(Expression)
	#define NANA_CONCURRENT_DECLARE(Type, Object)
	#define NANA_CONCURRENT_STATIC_DECLARE(Type, Object)
	#define NANA_CONCURRENT_STATIC_DEFINE(Type, Parent, Object)
#endif

namespace nana
{
	size_t strlen(const char_t* str);
	char_t* strcpy(char_t* dest, const char_t* source);

#if defined(NANA_WINDOWS)
	typedef __int64 wint_t;
#elif defined(NANA_LINUX)
	typedef long long wint_t;
#endif
}

#if defined(NANA_WINDOWS)
	#define NANA_SHARED_EXPORT	extern "C" _declspec(dllexport)
#elif defined(NANA_LINUX)
	#define NANA_SHARED_EXPORT	extern "C"
#endif

namespace nana
{
	nana::string stringset_cast(const std::wstring& text);
	nana::string stringset_cast(const std::string& text);
	
	void stringset_cast(std::string& to, const nana::string& from);
	void stringset_cast(std::wstring& to, const nana::string& from);
	bool is_incomplete(const nana::string& str, unsigned pos);

	struct state
	{
#if defined(NANA_CONCURRENT)
		static const bool concurrent = true;
#else
		static const bool concurrent = false;
#endif

#if defined(NANA_UNICODE)
		static const bool unicode = true;
#else
		static const bool unicode = false;
#endif
	};

	inline unsigned make_rgb(unsigned char red, unsigned char green, unsigned char blue)
	{

		return ((unsigned(red) << 16)|((unsigned(green)<<8))|blue);
	}
}

#define NANA_RGB(a)	(((DWORD)(a) & 0xFF)<<16) |  ((DWORD)(a) & 0xFF00) | (((DWORD)(a) & 0xFF0000) >> 16 )


#endif //NANA_MACROS_HPP
