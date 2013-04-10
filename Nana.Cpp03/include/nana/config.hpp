/*
 *	Nana Configuration
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/config.hpp
 */

#ifndef NANA_CONFIG_HPP
#define NANA_CONFIG_HPP

//There are marcos used for configuring Nana for the target system
//
//USE_NANA_WINDOWS
//		Target for Windows XP and later
//
//USE_NANA_LINUX_X11
//		Target to Linux(X11)
//
//Only one of them can be defined!!!
#define USE_NANA_WINDOWS


#if defined(USE_NANA_WINDOWS)
	#define NANA_WINDOWS 1
	#define PLATFORM_SPEC_HPP <nana/detail/win32/platform_spec.hpp>
	#define GUI_BEDROCK_HPP <nana/gui/detail/bedrock.hpp>
#elif defined(USE_NANA_LINUX_X11)
	#define NANA_LINUX 1
	#define NANA_X11 1
	#define PLATFORM_SPEC_HPP <nana/detail/linux_X11/platform_spec.hpp>
	#define GUI_BEDROCK_HPP <nana/gui/detail/bedrock.hpp>
#endif


#define NANA_UNICODE 1
//Support for PNG
//Comment it to disable the feature of support for PNG.
//#define NANA_ENABLE_PNG 1
#if defined(NANA_ENABLE_PNG)
	//Comment it to use libpng from operating system.
	#define NANA_LIBPNG 1
#endif

//Test whether the compiler is G++/MinGW.
#if defined(__GNUG__) && defined(NANA_WINDOWS)
	#define	NANA_MINGW
#endif


//If Boost C++ Library is installed in current system. Nana recommends
//enabling Boost mutex and condition_variable instead of the mutex
//and condition_variable that provided by Nana. Only for C++03
//#define NANA_USE_BOOST_MUTEX_CONDITION_VARIABLE 1

#endif
