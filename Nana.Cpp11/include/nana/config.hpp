/*
 *	Nana Configuration
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://nanapro.sourceforge.net/LICENSE_1_0.txt)
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

//NANA_MINGW
//	Enable this flags when you compile Nana with MinGW
//#define NANA_MINGW

#define NANA_UNICODE 1

//Support for PNG
//Comment it to disable the feature of support for PNG.
//#define NANA_ENABLE_PNG 1
#if defined(NANA_ENABLE_PNG)
	//Comment it to use libpng from operating system.
	#define NANA_LIBPNG 1
#endif


//Here defines some flags that tell Nana what features of compiler envoriment are not conformed with ISO C++.

//STD_CODECVT_NOT_SUPPORTED
//	Enable this flag when compiling Nana with MinGW.
//#define STD_CODECVT_NOT_SUPPORTED

//Don't modify the following lines
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

#if defined(NANA_LINUX)
	#ifndef STD_CODECVT_NOT_SUPPORTED
		#define STD_CODECVT_NOT_SUPPORTED
	#endif
#endif

#if defined(NANA_MINGW)
	#ifndef STD_CODECVT_NOT_SUPPORTED
		#define STD_CODECVT_NOT_SUPPORTED
	#endif
#endif

#endif	//NANA_CONFIG_HPP
