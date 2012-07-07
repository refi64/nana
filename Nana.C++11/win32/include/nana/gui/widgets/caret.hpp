/*
 *	A Button Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/caret.hpp
 */

#ifndef NANA_GUI_WIDGETS_CARET_HPP
#define NANA_GUI_WIDGETS_CARET_HPP

#include "../programming_interface.hpp"

namespace nana{ namespace gui{ namespace widget_impl{

	class caret
		: noncopyable
	{
	public:
		caret();
		caret(gui::window window, scalar_t width, scalar_t height);
		bool create(gui::window window, scalar_t width, scalar_t height);
		void destroy();
		void pos(int x, int y);
		nana::point pos() const;
		void size(scalar_t width, scalar_t height);
		nana::size size() const;
		void visible(bool value);
		bool visible() const;
		void effective_range(const nana::rectangle& rect);
	private:
		gui::window window_;
		nana::rectangle effective_range_;
	};


}//end namespace detail
	using widget_impl::caret;
} 
}

#endif
