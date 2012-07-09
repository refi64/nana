/*
 *	Nana Configuration
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/config.hpp
 */

#ifndef NANA_CONFIG_HPP
#define NANA_CONFIG_HPP

#define NANA_UNICODE 1

#define NANA_WINDOWS 1

//Support for PNG
//Comment it to disable the feature of support for PNG.
//#define NANA_ENABLE_PNG 1
#if defined(NANA_ENABLE_PNG)
	//Comment it to use libpng from operating system.
	#define NANA_LIBPNG 1
#endif


#endif

