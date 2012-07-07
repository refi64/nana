/*
 *	A Progress Indicator Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/progress.hpp
 */

#ifndef NANA_GUI_WIDGET_PROGRESS_HPP
#define NANA_GUI_WIDGET_PROGRESS_HPP
#include "widget.hpp"

namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		namespace progress
		{
			class trigger: public nana::gui::drawer_trigger
			{
				struct style
				{
					enum
					{
						begin,
						known,	//It is a style that knows the progress
						unknown,//It is a style that does not know the progress
						end
					};
				};
			public:
				trigger();
				void bind_window(nana::gui::widget&);
				void attached(graph_reference);
				unsigned value() const;
				unsigned value(unsigned);
				unsigned inc();
				unsigned Max() const;
				unsigned Max(unsigned);
				void Style(bool);
			private:
				void refresh(graph_reference);
			private:
				void _m_draw();
				void _m_draw_box(graph_reference);
				void _m_draw_progress(graph_reference);
				bool _m_check_changing(unsigned) const;
			private:
				static const unsigned border = 2;
				nana::gui::widget * widget_;
				nana::paint::graphics* graph_;
				unsigned draw_width_;
				bool has_value_;
				int style_;
				unsigned max_;
				unsigned value_;
			}; //end class drawer
		}
	}//end namespace drawerbase

	class progress
		: public widget_object<category::widget_tag, drawerbase::progress::trigger>
	{
	public:
		progress();
		progress(window);
		progress(window, const nana::rectangle & r);
		progress(window, int x, int y, unsigned width, unsigned height);

		unsigned value() const;
		unsigned value(unsigned val);
		unsigned inc();
		unsigned amount() const;
		unsigned amount(unsigned value);
		void style(bool known);
	};
}//end namespace gui
}//end namespace nana
#endif
