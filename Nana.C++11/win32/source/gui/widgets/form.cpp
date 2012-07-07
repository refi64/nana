/*
 *	A Form Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/form.cpp
 */

#include <nana/gui/widgets/form.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace form
		{
		/*
		 * class trigger
		 */

			trigger::trigger():graph_(nullptr){}

			void trigger::bind_window(nana::gui::widget& widget)
			{
				widget_ = & widget;
			}

			void trigger::attached(nana::paint::graphics& graph)
			{
				graph_ = &graph;
				event_size_ = API::dev::make_drawer_event<events::size>(widget_->handle());
			}

			void trigger::detached()
			{
				API::umake_event(widget_->handle());
			}

			void trigger::refresh(nana::paint::graphics& graph)
			{
				graph.rectangle(API::background(widget_->handle()), true);
			}

			void trigger::resize(nana::paint::graphics& graph, const nana::gui::eventinfo& ei)
			{
				graph.rectangle(0, 0, ei.size.width, ei.size.height, API::background(widget_->handle()), true);
				API::lazy_refresh();
			}
		}//end namespace form
	}//end namespace drawerbase

	/*
	 * class form
	 */
	typedef widget_object<category::root_tag, drawerbase::form::trigger> form_base_t;

		form::form()
		{
			get_drawer_trigger();
		}

		form::form(int x, int y, unsigned width, unsigned height)
			:form_base_t(0, false, x, y, width, height, appearance())
		{}

		form::form(int x, int y, unsigned width, unsigned height, const nana::gui::appearance& app)
			:form_base_t(0, false, x, y, width, height, app)
		{}

		form::form(const nana::rectangle& r)
			:form_base_t(0, false, r.x, r.y, r.width, r.height, appearance())
		{}

		form::form(const nana::rectangle& r, const nana::gui::appearance& app)
			:form_base_t(0, false, r.x, r.y, r.width, r.height, app)
		{}

		form::form(window wd)
			: form_base_t(wd, false)
		{}

		form::form(window owner, int x, int y, unsigned width, unsigned height)
			:form_base_t(owner, false, x, y, width, height, appearance())
		{}

		form::form(window owner, int x, int y, unsigned width, unsigned height, const nana::gui::appearance& app)
			:form_base_t(owner, false, x, y, width, height, app)
		{}

		form::form(window owner, const nana::rectangle& r)
			:form_base_t(owner, false, r.x, r.y, r.width, r.height, appearance())
		{}

		form::form(window owner, const nana::rectangle& r, const nana::gui::appearance& app)
			:form_base_t(owner, false, r.x, r.y, r.width, r.height, app)
		{}


		nested_form::nested_form(window owner)
			: form_base_t(owner, true)
		{}

		nested_form::nested_form(window owner, int x, int y, unsigned width, unsigned height)
			: form_base_t(owner, true, x, y, width, height, gui::appearance())
		{}

		nested_form::nested_form(window owner, int x, int y, unsigned width, unsigned height, const nana::gui::appearance& app)
			: form_base_t(owner, true, x, y, width, height, app)
		{}

		nested_form::nested_form(window owner, const nana::rectangle& r)
			: form_base_t(owner, true, r.x, r.y, r.width, r.height, appearance())
		{}

		nested_form::nested_form(window owner, const nana::rectangle& r, const nana::gui::appearance& app)
			: form_base_t(owner, true, r.x, r.y, r.width, r.height, app)
		{}
}//end namespace gui
}//end namespace nana
