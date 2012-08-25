/*
 *	A Drawer Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/drawer.hpp
 */

#ifndef NANA_GUI_DETAIL_DRAWER_HPP
#define NANA_GUI_DETAIL_DRAWER_HPP

#include <vector>
#include "eventinfo.hpp"
#include <nana/paint/graphics.hpp>
#include <nana/paint/image.hpp>

namespace nana
{
namespace gui
{
	class widget;

	class drawer_trigger
		: nana::noncopyable, nana::nonmovable
	{
	public:
		typedef gui::widget&		widget_reference;
		typedef paint::graphics&	graph_reference;

		virtual ~drawer_trigger();
		virtual void bind_window(widget_reference);
		virtual void attached(graph_reference);	//none-const
		virtual void detached();	//none-const

		//notify_background_change:
		//@brief: If the widget is a glass window, and the widgets under the glass widget are changed, nana.gui will invoke a notify_background_change
		virtual void notify_background_change(graph_reference);
		virtual void typeface_changed(graph_reference);
		virtual void refresh(graph_reference);

		virtual void resize(graph_reference, const eventinfo&);
		virtual void click(graph_reference, const eventinfo&);
		virtual void dbl_click(graph_reference, const eventinfo&);
		virtual void mouse_enter(graph_reference, const eventinfo&);
		virtual void mouse_move(graph_reference, const eventinfo&);
		virtual void mouse_leave(graph_reference, const eventinfo&);
		virtual void mouse_down(graph_reference, const eventinfo&);
		virtual void mouse_up(graph_reference, const eventinfo&);
		virtual void mouse_wheel(graph_reference, const eventinfo&);
		virtual void mouse_drop(graph_reference, const eventinfo&);

		virtual void focus(graph_reference, const eventinfo&);
		virtual void key_down(graph_reference, const eventinfo&);
		virtual void key_char(graph_reference, const eventinfo&);
		virtual void key_up(graph_reference, const eventinfo&);
		virtual void shortkey(graph_reference, const eventinfo&);
	};

	namespace detail
	{
		namespace dynamic_drawing
		{
			//declaration
			class object;
		}

		//class drawer
		//@brief:	Every window has a drawer, the drawer holds a drawer_trigger for
		//			a widget.
		class drawer
			: nana::noncopyable, nana::nonmovable
		{
			typedef drawer this_type;
		public:

			drawer();
			~drawer();
			// the event is fired by window_layout
			void notify_background_change();
			void typeface_changed();
			void click(const eventinfo&);
			void dbl_click(const eventinfo&);
			void mouse_enter(const eventinfo&);
			void mouse_move(const eventinfo&);
			void mouse_leave(const eventinfo&);
			void mouse_down(const eventinfo&);
			void mouse_up(const eventinfo&);
			void mouse_wheel(const eventinfo&);
			void mouse_drop(const eventinfo&);
			void resize(const eventinfo&);
			void focus(const eventinfo&);
			void key_down(const eventinfo&);
			void key_char(const eventinfo&);
			void key_up(const eventinfo&);
			void shortkey(const eventinfo&);
			void map(window, const nana::rectangle& vr);	//Copy the root buffer to screen
			void refresh();
			drawer_trigger* realizer() const;
			void attached(drawer_trigger&);
			drawer_trigger* detached();
		public:
			void clear();
			void string(int x, int y, unsigned color, const nana::char_t*);
			void line(int x, int y, int x2, int y2, unsigned color);
			void rectangle(int x, int y, unsigned width, unsigned height, unsigned color, bool issolid);
			void shadow_rectangle(int x, int y, unsigned width, unsigned height, nana::color_t beg, nana::color_t end, bool vertical);
			void bitblt(int x, int y, unsigned width, unsigned height, const nana::paint::graphics& graph, int srcx, int srcy);
			void bitblt(int x, int y, unsigned width, unsigned height, const nana::paint::image& img, int srcx, int srcy);
			void stretch(const nana::rectangle& r_dst, const nana::paint::graphics& graph, const nana::rectangle& r_src);
			void stretch(const nana::rectangle& r_dst, const nana::paint::image& img, const nana::rectangle& r_src);
			nana::gui::event_handle make_event(int event_id, nana::gui::window trigger, nana::gui::window listener = 0);
		private:
			void _m_draw_dynamic_drawing_object();
		public:
			nana::paint::graphics graphics;
		private:
			std::vector<nana::gui::detail::dynamic_drawing::object*>	dynamic_drawing_objects_;
			drawer_trigger* realizer_;
			bool refreshing_;
		};
	}//end namespace detail
}//end namespace gui
}//end namespace nana

#endif