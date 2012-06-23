/*
 *	A Frame Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/frame.hpp
 *
 *	A frame provides a way to contain the platform window in a stdex GUI Window
 */

#ifndef NANA_GUI_WIDGET_FRAME_HPP
#define NANA_GUI_WIDGET_FRAME_HPP

#include "widget.hpp"
namespace nana
{
namespace gui
{
	class frame: public widget_object<category::frame_tag, int>
	{
		typedef widget_object<category::frame_tag, int> base_type;
	public:
		frame();
		frame(window);
		frame(window, const nana::rectangle& r);
		frame(window, int x, int y, unsigned width, unsigned height);
		bool insert(native_window_type);
		native_window_type element(unsigned index);
		native_window_type container() const;
	};
}//end namespace gui
}//end namespace stedex
#endif
