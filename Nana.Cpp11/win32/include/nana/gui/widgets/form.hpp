/*
 *	A Form Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/form.hpp
 */

#ifndef NANA_GUI_WIDGET_FORM_HPP
#define NANA_GUI_WIDGET_FORM_HPP

#include "widget.hpp"

namespace nana{namespace gui{
	namespace drawerbase
	{
		namespace form
		{
			class trigger: public nana::gui::drawer_trigger
			{
			public:
				trigger();
				void bind_window(nana::gui::widget& widget);
				void attached(nana::paint::graphics&);
				void detached();
				void refresh(nana::paint::graphics&);
				void resize(nana::paint::graphics&, const nana::gui::eventinfo&);
			private:
				nana::gui::event_handle event_size_;

				nana::paint::graphics* graph_;
				nana::gui::widget*	widget_;
			};
		}//end namespace form
	}//end namespace drawerbase

	class form: public widget_object<category::root_tag, drawerbase::form::trigger>
	{
	public:
		typedef nana::gui::appear appear;

		form();
		form(int x, int y, unsigned width, unsigned height);
		form(int x, int y, unsigned width, unsigned height, const nana::gui::appearance&);
		form(const nana::rectangle&);
		form(const nana::rectangle& rect, const nana::gui::appearance&);

		form(window);
		form(window, int x, int y, unsigned width, unsigned height);
		form(window, int x, int y, unsigned width, unsigned height, const nana::gui::appearance&);
		form(window, const nana::rectangle&);
		form(window, const nana::rectangle& rect, const nana::gui::appearance&);
	};

	class nested_form: public widget_object<category::root_tag, drawerbase::form::trigger>
	{
	public:
		typedef nana::gui::appear appear;

		nested_form(window);
		nested_form(window, int x, int y, unsigned width, unsigned height);
		nested_form(window, int x, int y, unsigned width, unsigned height, const nana::gui::appearance&);
		nested_form(window, const nana::rectangle&);
		nested_form(window, const nana::rectangle& rect, const nana::gui::appearance&);
	};
}//end namespace gui
}//end namespace nana
#endif
