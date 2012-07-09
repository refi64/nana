/*
 *	A Label Control Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/label.hpp
 */

#ifndef NANA_GUI_WIDGET_LABEL_HPP
#define NANA_GUI_WIDGET_LABEL_HPP
#include "widget.hpp"


namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace label
		{
			//class trigger
			//@brief:	draw the label
			class trigger: public nana::gui::drawer_trigger
			{
				struct impl_t;
			public:
				trigger();
				~trigger();
				void bind_window(nana::gui::widget&);
				impl_t * impl() const;
			private:
				void attached(graph_reference);
				void detached();
				void refresh(paint::graphics&);
				void notify_background_change(nana::paint::graphics&);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
				void click(graph_reference, const eventinfo&);
			private:
				impl_t * impl_;
			};

		}//end namespace label
	}//end namespace drawerbase

	//class label
	//@brief: defaine a label widget and it provides the interfaces to be operationa
	class label
		: public widget_object<category::widget_tag, drawerbase::label::trigger>
	{
	public:
		label();
		label(window);
		label(window, const nana::rectangle& r);
		label(window, int x, int y, unsigned width, unsigned height);
		void transparent(bool);
		bool transparent() const;
		void format(bool);
		nana::size measure() const;
		unsigned extent_size() const;
	};
}//end namespace gui
}//end namespace nana
#endif
