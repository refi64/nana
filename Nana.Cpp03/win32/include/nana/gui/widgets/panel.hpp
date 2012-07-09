/*
 *	A Panel Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/panel.hpp
 *
 *	@brief: panel is a widget used for placing some widgets.
 */

#ifndef NANA_GUI_WIDGETS_PANEL_HPP
#define NANA_GUI_WIDGETS_PANEL_HPP
#include "widget.hpp"

namespace nana
{
namespace gui
{
namespace xpanel
{
	class drawer: public nana::gui::drawer_trigger
	{
	public:
		drawer();
	private:
		void bind_window(widget_reference widget);
		void refresh(graph_reference graph);
	private:
		nana::gui::window window_;
	};

}//end namespace xpanel

	template<bool HasBackground>
	class panel
		: public gui::widget_object<typename metacomp::static_if<metacomp::bool_type<HasBackground>,
												 gui::category::widget_tag, gui::category::lite_widget_tag>::value_type,
							 xpanel::drawer>
	{
	public:
		panel(){}

		panel(window wd)
		{
			this->create(wd, 0, 0, 0, 0);
		}

		panel(window wd, const nana::rectangle& r)
		{
			this->create(wd, r.x, r.y, r.width, r.height);
		}

		panel(window wd, int x, int y, unsigned width, unsigned height)
		{
			this->create(wd, x, y, width, height);
		}

		bool transparent() const
		{
			return API::glass_window(*this);
		}

		void transparent(bool tr)
		{
			API::glass_window(*this, tr);
		}
	};

}//end namespace gui
}//end namespace nana
#endif
