/*
 *	An Edge Keeper Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/layout.hpp
 *	@brief: The edge_keeper automatically controls the position and size of a widget.
 */

#ifndef NANA_GUI_LAYOUT_HPP
#define NANA_GUI_LAYOUT_HPP

#include <nana/gui/widgets/widget.hpp>
#include <vector>

namespace nana
{
namespace gui
{
	class gird
		: nana::noncopyable
	{
		enum kind_t{kind_window, kind_gird};
		struct element_tag;
		gird(gird * owner, unsigned scale);
	public:
		gird();
		gird(widget&);
		~gird();
		void bind(widget&);
		gird * push(unsigned blank, unsigned scale);
		void push(window, unsigned blank, unsigned scale);
		gird * add(unsigned blank, unsigned scale);
		void add(window, unsigned blank, unsigned scale);
		void fasten(window);
	private:
		void _m_resize();
		void _m_adjust_children();
		void _m_adjust_elements();
	private:
		struct ref_owner_tag
		{
			kind_t kind;
			union
			{
				widget * ref_widget;
				gird * ref_gird;
			}u;
		}owner_;

		nana::gui::event_handle event_handle_;
		nana::rectangle area_;
		std::vector<element_tag*> child_;
		std::vector<element_tag*> elements_;
		std::vector<window> fasten_elements_;
	};//end class gird
}//end namespace gui
}//end namespace nana

#endif
