/*
 *	A Frame Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/frame.cpp
 *
 *	A frame provides a way to contain the platform window in a stdex GUI Window
 */

#include <nana/gui/widgets/frame.hpp>

namespace nana
{
namespace gui
{

	
	//class frame:: public widget_object<category::frame_tag>
		frame::frame(){}

		frame::frame(window wd)
		{
			create(wd, 0, 0, 0, 0);
		}

		frame::frame(window wd, const nana::rectangle& r)
		{
			this->create(wd, r.x, r.y, r.width, r.height);
		}

		frame::frame(window wd, int x, int y, unsigned w, unsigned h)
		{
			this->create(wd, x, y, w, h);
		}

		bool frame::insert(native_window_type wd)
		{
			return API::insert_frame(this->handle(), wd);
		}

		native_window_type frame::element(unsigned index)
		{
			return API::frame_element(this->handle(), index);
		}

		native_window_type frame::container() const
		{
			return API::frame_container(this->handle());
		}

	//end class frame
}//end namespace gui
}//end namespace nana

