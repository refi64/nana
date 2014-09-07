/*
 *	A Drawer Implementation
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/drawer.hpp
 */

#ifndef NANA_GUI_DETAIL_DRAWER_HPP
#define NANA_GUI_DETAIL_DRAWER_HPP

#include <vector>
#include "general_events.hpp"
#include <nana/paint/graphics.hpp>
#include <nana/paint/image.hpp>
#include <functional>

namespace nana
{
	class widget;

	class drawer_trigger
		: ::nana::noncopyable, ::nana::nonmovable
	{
	public:
		typedef widget&		widget_reference;
		typedef paint::graphics&	graph_reference;

		virtual ~drawer_trigger();
		virtual void attached(widget_reference, graph_reference);	//none-const
		virtual void detached();	//none-const

		virtual void typeface_changed(graph_reference);
		virtual void refresh(graph_reference);

		virtual void resizing(graph_reference, const arg_resizing&);
		virtual void resized(graph_reference, const arg_resized&);
		virtual void move(graph_reference, const arg_move&);
		virtual void click(graph_reference, const arg_mouse&);
		virtual void dbl_click(graph_reference, const arg_mouse&);
		virtual void mouse_enter(graph_reference, const arg_mouse&);
		virtual void mouse_move(graph_reference, const arg_mouse&);
		virtual void mouse_leave(graph_reference, const arg_mouse&);
		virtual void mouse_down(graph_reference, const arg_mouse&);
		virtual void mouse_up(graph_reference, const arg_mouse&);
		virtual void mouse_wheel(graph_reference, const arg_wheel&);
		virtual void mouse_dropfiles(graph_reference, const arg_dropfiles&);

		virtual void focus(graph_reference, const arg_focus&);
		virtual void key_press(graph_reference, const arg_keyboard&);
		virtual void key_char(graph_reference, const arg_keyboard&);
		virtual void key_release(graph_reference, const arg_keyboard&);
		virtual void shortkey(graph_reference, const arg_keyboard&);
	};

	namespace detail
	{
		struct basic_window;

		namespace dynamic_drawing
		{
			//declaration
			class object;
		}

		//@brief:	Every window has a drawer, the drawer holds a drawer_trigger for
		//			a widget.
		class drawer
			: nana::noncopyable, nana::nonmovable
		{
		public:

			drawer();
			~drawer();

			void bind(basic_window*);

			void typeface_changed();
			void click_arg(const arg_mouse&);
			void dbl_click_arg(const arg_mouse&);
			void mouse_enter_arg(const arg_mouse&);
			void mouse_move_arg(const arg_mouse&);
			void mouse_leave_arg(const arg_mouse&);
			void mouse_down_arg(const arg_mouse&);
			void mouse_up_arg(const arg_mouse&);
			void mouse_wheel_arg(const arg_wheel&);
			void mouse_dropfiles(const arg_dropfiles&);
			void resizing_arg(const arg_resizing&);
			void resized_arg(const arg_resized&);
			void move_arg(const arg_move&);
			void focus_arg(const arg_focus&);
			void key_press_arg(const arg_keyboard&);
			void key_char_arg(const arg_keyboard&);
			void key_release_arg(const arg_keyboard&);
			void shortkey_arg(const arg_keyboard&);
			void map(window);	//Copy the root buffer to screen
			void refresh();
			drawer_trigger* realizer() const;
			void attached(widget&, drawer_trigger&);
			drawer_trigger* detached();
		public:
			void clear();
			void* draw(std::function<void(paint::graphics&)> &&, bool diehard);
			void erase(void* diehard);
		private:
			void _m_bground_pre();
			void _m_bground_end();
			void _m_draw_dynamic_drawing_object();
		public:
			nana::paint::graphics graphics;
		private:
			basic_window*	core_window_;
			drawer_trigger*	realizer_;
			std::vector<dynamic_drawing::object*>	dynamic_drawing_objects_;
			bool refreshing_;
		};
	}//end namespace detail
}//end namespace nana

#endif
