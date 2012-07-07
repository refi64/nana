/*
 *	A Button Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/caret.cpp
 */

#include <nana/gui/widgets/caret.hpp>

namespace nana{ namespace gui{ namespace widget_impl{

	//class caret
		caret::caret()
			:window_(nullptr)
		{}

		caret::caret(gui::window window, scalar_t width, scalar_t height)
			:window_(window)
		{
			API::create_caret(window, width, height);
		}

		bool caret::create(gui::window window, scalar_t width, scalar_t height)
		{
			if(window_)
				return false;
			
			API::create_caret(window, width, height);
			window_ = window;
			return true;
		}

		void caret::destroy()
		{
			API::destroy_caret(window_);
			window_ = 0;
		}

		void caret::pos(int x, int y)
		{
			API::caret_pos(window_, x, y);
		}

		nana::point caret::pos() const
		{
			return API::caret_pos(window_);
		}

		void caret::size(scalar_t width, scalar_t height)
		{
			API::caret_size(window_, nana::size(width, height));
		}

		nana::size caret::size() const
		{
			return gui::API::caret_size(window_);
		}

		void caret::visible(bool value)
		{
			API::caret_visible(window_, value);
		}

		bool caret::visible() const
		{
			return API::caret_visible(window_);
		}

		void caret::effective_range(const nana::rectangle& rect)
		{
			API::caret_effective_range(window_, rect);
		}

}//end namespace detail
} 
}
