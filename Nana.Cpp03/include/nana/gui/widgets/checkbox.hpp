/*
 *	A CheckBox Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/checkbox.hpp
 */

#ifndef NANA_GUI_WIDGET_CHECKBOX_HPP
#define NANA_GUI_WIDGET_CHECKBOX_HPP
#include "widget.hpp"
#include <nana/paint/gadget.hpp>
#include <vector>

namespace nana
{
namespace gui
{
	namespace xcheckbox
	{

		class drawer
			: public drawer_trigger
		{
		public:
			typedef paint::gadget::check_renderer check_renderer_t;

			drawer();
			void bind_window(nana::gui::widget&);
			void attached(graph_reference);
			void detached();
			void refresh(graph_reference);
			void mouse_enter(graph_reference, const nana::gui::eventinfo&);
			void mouse_leave(graph_reference, const nana::gui::eventinfo&);
			void mouse_down(graph_reference, const nana::gui::eventinfo&);
			void mouse_up(graph_reference, const nana::gui::eventinfo&);
		public:
			paint::gadget::check_renderer& check_renderer();

			void react(bool);
			void checked(bool);
			bool checked() const;
			void radio(bool);
			void style(check_renderer_t::checker_t);
			check_renderer_t::checker_t style() const;
			
		private:
			void _m_draw(graph_reference);
			void _m_draw_background(graph_reference);
			void _m_draw_checkbox(graph_reference, unsigned first_line_height);
			void _m_draw_title(graph_reference);
		private:
			static const int interval = 4;
			nana::gui::widget* widget_;
			unsigned state_;

			struct checker_tag
			{
				bool react;
				bool checked;
				bool radio;
				paint::gadget::check_renderer::checker_t type;
				paint::gadget::check_renderer renderer;

				checker_tag();
			}checker_;
		};
	}//end namespace xcheckbox

	class checkbox
		: public widget_object<category::widget_tag, xcheckbox::drawer>
	{
	public:
		enum checker_t{clasp, blocker};

		checkbox();
		checkbox(window, bool visible);
		checkbox(window, const rectangle& = rectangle(), bool visible = true);

		void react(bool want);
		bool checked() const;
		void check(bool chk);
		void radio(bool);
		void style(checker_t chk);
		checker_t style() const;
		void transparent(bool value);
		bool transparent() const;
		void open_check_image(const nana::paint::image&);
		void set_check_image(mouse_action_t, checker_t, bool checked, const nana::rectangle& r);
	};//end class checkbox

	class radio_group
	{
		struct element_tag
		{
			checkbox * uiobj;
			event_handle eh_checked;
			event_handle eh_destroy;
		};
	public:
		~radio_group();
		void add(checkbox&);
		std::size_t checked() const;
	private:
		void _m_checked(const eventinfo&);
		void _m_destroy(const eventinfo&);
	private:
		std::vector<element_tag> ui_container_;
	};
}//end namespace gui
}//end namespace nana

#endif
