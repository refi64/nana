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
		//class trigger
			trigger::trigger():graph_(0){}

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

	//class form
	typedef widget_object<category::root_tag, drawerbase::form::trigger> form_base_t;

		form::form(const rectangle& r, const appearance& apr)
			: form_base_t(0, false, r, apr)
		{}

		form::form(window owner, const appearance& apr)
			: form_base_t(owner, false, API::make_center(owner, 300, 150), apr)
		{}

		form::form(window owner, const rectangle& r, const appearance& apr)
			: form_base_t(owner, false, r, apr)
		{}
	//end class form

	//class nested_form
		nested_form::nested_form(window owner)
			: form_base_t(0, true, rectangle(), appearance())
		{}

		nested_form::nested_form(window owner, const appearance& apr)
			: form_base_t(owner, true, rectangle(), apr)
		{}

		nested_form::nested_form(window owner, const rectangle& r, const appearance& apr)
			: form_base_t(owner, true, r, apr)
		{}
	//end nested_form
}//end namespace gui
}//end namespace nana
