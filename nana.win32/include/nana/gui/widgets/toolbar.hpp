/*
 *	A Toolbar Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/toolbar.hpp
 */

#ifndef NANA_GUI_WIDGET_TOOLBAR_HPP
#define NANA_GUI_WIDGET_TOOLBAR_HPP

#include "widget.hpp"
#include <vector>

namespace nana{ namespace gui{
	class toolbar;

	namespace drawerbase
	{
		namespace toolbar
		{
			struct item_type;

			struct ext_event_tag
			{
				nana::functor_group<void(nana::gui::toolbar&, size_t)> selected;
				nana::functor_group<void(nana::gui::toolbar&, size_t)> enter;
				nana::functor_group<void(nana::gui::toolbar&, size_t)> leave;
			};

			class drawer
				: public nana::gui::drawer_trigger
			{
				struct drawer_impl_type;

			public:
				typedef size_t size_type;
				const static size_type npos = static_cast<size_type>(-1);

				mutable ext_event_tag ext_event;

				drawer();
				~drawer();

				void append(const nana::string&, const nana::paint::image&);
				void append();
				bool enable(size_type) const;
				bool enable(size_type, bool);
				void scale(unsigned);
			private:
				void bind_window(widget_reference);
				void refresh(graph_reference);
				void attached(graph_reference);
				void detached();
				void mouse_move(graph_reference, const nana::gui::eventinfo&);
				void mouse_leave(graph_reference, const nana::gui::eventinfo&);
				void mouse_down(graph_reference, const nana::gui::eventinfo&);
				void mouse_up(graph_reference, const nana::gui::eventinfo&);
			private:
				size_type _m_which(int x, int y, bool want_if_disabled) const;
				void _m_draw_background(nana::color_t color);
				void _m_draw();
				void _m_owner_sized(const nana::gui::eventinfo& ei);
			private:
				void _m_fill_pixels(item_type*, bool force);
			private:
				nana::gui::widget*	widget_;
				nana::paint::graphics* graph_;
				drawer_impl_type	*impl_;
			};
		
		}//end namespace toolbar
	}//end namespace drawerbase

	class toolbar
		: public nana::gui::widget_object<category::widget_tag, drawerbase::toolbar::drawer>
	{
	public:
		typedef size_t size_type;
		typedef drawerbase::toolbar::ext_event_tag ext_event_type;

		toolbar();
		toolbar(window);
		toolbar(window, const nana::rectangle& r);
		toolbar(window, int x, int y, unsigned width, unsigned height);

		ext_event_type& ext_event() const;
		void append();
		void append(const nana::string& text, const nana::paint::image& img);
		void append(const nana::string&);
		bool enable(size_type) const;
		void enable(size_type n, bool eb);
		void scale(unsigned s);
	};
}//end namespace gui
}//end namespace nana


#endif
