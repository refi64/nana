/*
 *	The Deploy Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/depoly.cpp
 *
 *	What follow are dependented on what defined in nana/config.hpp
 */

#include <nana/deploy.hpp>
#include <nana/auto_buf.hpp>

#if defined(NANA_WINDOWS)
	#include <windows.h>
#elif defined(NANA_LINUX)
	#include <string.h>
	#include <nana/detail/platform_spec.hpp>
#endif

namespace nana
{
	std::size_t strlen(const char_t* str)
	{
#if defined(NANA_UNICODE)
		return ::wcslen(str);
#else
		return ::strlen(str);
#endif
	}

	char_t* strcpy(char_t* dest, const char_t* source)
	{
#if defined(NANA_UNICODE)
		return ::wcscpy(dest, source);
#else
		return ::strcpy(dest, source);
#endif	
	}


	nana::string stringset_cast(const std::wstring& text)
	{
#if defined(NANA_UNICODE)
		return text;
#else
	#if defined(NANA_WINDOWS)
		const int size = static_cast<int>(text.size()) * 2 + 1;
		nana::auto_buf<char> ptr(size);
		memset(ptr.get(), 0, size);
		::WideCharToMultiByte(CP_ACP, 0, text.c_str(), -1, ptr.get(), size, 0, 0);
		return ptr.get();
	#elif defined(NANA_LINUX)
		nana::detail::charset_conv conv("UTF-8", "UTF-32");
		return conv.charset(text);
	#endif
#endif
	}

	nana::string stringset_cast(const std::string& text)
	{
		if(text.size() == 0) return nana::string();
#if defined(NANA_UNICODE)
	#if defined(NANA_WINDOWS)
		const int size = static_cast<int>(text.size()) + 1;
		nana::auto_buf<wchar_t> ptr(size);
		::MultiByteToWideChar(CP_ACP, 0, text.c_str(), -1, ptr.get(), size);
		return ptr.get();
	#elif defined(NANA_LINUX)
		nana::detail::charset_conv conv("UTF-32", "UTF-8");
		std::string str = conv.charset(text);
		const wchar_t * beg = reinterpret_cast<const wchar_t*>(str.c_str());
		std::wstring wstr(beg + 1, beg + str.size() / sizeof(wchar_t));
		return wstr;
	#endif
#else
		return text;
#endif
	}

	void stringset_cast(std::string& to, const nana::string& from)
	{
		if(from.size())
		{
#if defined(NANA_UNICODE)
	#if defined(NANA_WINDOWS)
			const int size = static_cast<int>(from.size()) * 2 + 1;
			nana::auto_buf<char> ptr(size);
			memset(ptr.get(), 0, size);
			::WideCharToMultiByte(CP_ACP, 0, from.c_str(), -1, ptr.get(), size, 0, 0);
			to = ptr.get();
	#elif defined(NANA_LINUX)
			nana::detail::charset_conv conv("UTF-8", "UTF-32");
			const char* beg = reinterpret_cast<const char*>(from.c_str());
			std::string str(beg, beg + from.size() * sizeof(wchar_t));
			to = conv.charset(str);
	#endif
#else
			to = from;
#endif
		}
		else
			to = std::string();
	}

	void stringset_cast(std::wstring& to, const nana::string& from)
	{
		if(from.size() == 0)
		{
			to = std::wstring();
			return;
		}
#if defined(NANA_UNICODE)
		to = from;
#else
	#if defined(NANA_WINDOWS)
		const int size = static_cast<int>(from.size()) + 1;
		nana::auto_buf<wchar_t> ptr(size);
		memset(ptr.get(), 0, sizeof(wchar_t) * size);
		::MultiByteToWideChar(CP_ACP, 0, from.c_str(), -1, ptr.get(), size);
		to = ptr.get();
	#elif defined(NANA_LINUX)
		nana::detail::charset_conv conv("UTF-32", "UTF-8");
		std::string str = conv.charset(from);
		const wchar_t * beg = reinterpret_cast<const wchar_t*>(str.c_str());
		to.clear();
		to.assign(beg + 1, beg + str.size() / sizeof(wchar_t));
	#endif
#endif	
	}

	bool is_incomplete(const nana::string& str, unsigned pos)
	{
#ifndef NANA_UNICODE
		if(pos > str.size())
			pos = static_cast<unsigned>(str.size());
		const nana::char_t * pstr = str.c_str();
		if(pstr[pos] < 0)
		{
			bool incomp = false;
			for(unsigned i = 0; i < pos; ++i)
			{
				if(pstr[i] < 0)
					incomp = !incomp;
				else
					incomp = false;
			}
			return incomp;
		}
#endif
		return false;
	}
}
