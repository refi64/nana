/*
 *	A Combox Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/combox.hpp
 */

#ifndef NANA_GUI_WIDGETS_COMBOX_HPP
#define NANA_GUI_WIDGETS_COMBOX_HPP
#include "widget.hpp"
#include "float_listbox.hpp"

namespace nana{ namespace gui
{
	class combox;
	namespace drawerbase
	{
		namespace combox
		{
			struct ext_event_tag
			{
				nana::fn_group<void(nana::gui::combox&)> selected;
			};

			class drawer_impl;
			
			class trigger
				: public nana::gui::drawer_trigger
			{
			public:
				typedef ext_event_tag ext_event_type;

				trigger();
				~trigger();

				drawer_impl& get_drawer_impl();
				const drawer_impl& get_drawer_impl() const;
			private:
				void bind_window(widget_reference);
				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void focus(graph_reference, const nana::gui::eventinfo&);
				void mouse_enter(graph_reference, const nana::gui::eventinfo&);
				void mouse_leave(graph_reference, const nana::gui::eventinfo&);
				void mouse_down(graph_reference, const nana::gui::eventinfo&);
				void mouse_up(graph_reference, const nana::gui::eventinfo&);
				void mouse_move(graph_reference, const nana::gui::eventinfo&);
				void mouse_wheel(graph_reference, const nana::gui::eventinfo&);
				void key_down(graph_reference, const nana::gui::eventinfo&);
				void key_char(graph_reference, const nana::gui::eventinfo&);
			private:
				drawer_impl * drawer_;
			};
		}
	}//end namespace drawerbase

	class combox
		: public widget_object<category::widget_tag, drawerbase::combox::trigger>
	{
	public:
		typedef float_listbox::item_renderer item_renderer;
		typedef drawerbase::combox::ext_event_tag ext_event_type;

		combox();
		combox(window);
		combox(window, const nana::rectangle& r);
		combox(window, int x, int y, unsigned width, unsigned height);
		
		void clear();
		void editable(bool);
		bool editable() const;
		combox& push_back(const nana::string&);
		unsigned option() const;
		void option(unsigned i);
		nana::string text(unsigned) const;

		ext_event_type& ext_event() const;
		void renderer(item_renderer*);
	private:
		void _m_caption(const nana::string&);
	};
}//end namespace gui
}
#endif
