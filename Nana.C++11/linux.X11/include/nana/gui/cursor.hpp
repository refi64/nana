/*
 *	An Cursor Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/cursor.hpp
 */

#ifndef NANA_GUI_CURSOR_HPP
#define NANA_GUI_CURSOR_HPP
#include <nana/deploy.hpp>
#include <nana/gui/basis.hpp>

namespace nana
{
namespace gui
{

	namespace detail
	{
		struct cursor_impl;
	}

	class cursor
	{
		typedef cursor self_type;
	public:
		struct predef
		{
			//The constants are defined at http://www.tronche.com/gui/x/xlib/appendix/b/
			enum t{
				hand	= 60, 
				arrow	= 68,
				wait	= 150, 
				iterm	= 152, //a text caret.
				size_we = 108,
				size_ns = 116,
				size_top_left = 134,
				size_top_right = 136,
				size_bottom_left = 12,
				size_bottom_right = 14
			};
		};
	public:
		cursor();
		cursor(nana::gui::window);
		cursor(const self_type&);
		~cursor();
		self_type& operator=(const self_type&);
	public:
		bool bind(nana::gui::window);
		cursor::predef::t load(predef::t id);
		cursor::predef::t get() const;
	private:
		void _m_assign(detail::cursor_impl*);
	private:
		detail::cursor_impl* impl_;
	}; //end class cursor

	class auto_cursor
		: nana::noncopyable, nana::nonmovable
	{
		class auto_cursor_impl;
	public:
		auto_cursor();
		~auto_cursor();
		void set(nana::gui::window, cursor::predef::t id);
		void erase(nana::gui::window);
	private:
		auto_cursor_impl * impl_;
	};

}//end namespace gui
}//end namespace nana

#endif
