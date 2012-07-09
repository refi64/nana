/*
 *	A Panel Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: source/gui/widgets/panel.cpp
 *
 *	@brief: panel is a widget used for placing some widgets.
 */

#include <nana/gui/widgets/panel.hpp>

namespace nana
{
namespace gui
{
namespace xpanel
{
	//class drawer
	//public:
		drawer::drawer()
			:window_(nullptr)
		{}
	//private:

		void drawer::bind_window(widget_reference widget)
		{
			widget.caption(STR("Nana Panel"));
			window_ = widget.handle();
		}

		void drawer::refresh(drawer::graph_reference graph)
		{
			if(API::glass_window(window_))
				API::make_glass_background(window_);
			else
				graph.rectangle(0, 0, graph.width(), graph.height(), nana::gui::API::background(window_), true);
		}
	//end class drawer

}//end namespace xpanel
}//end namespace gui
}//end namespace nana
