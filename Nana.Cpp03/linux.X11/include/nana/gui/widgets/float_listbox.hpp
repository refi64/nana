/*
 *	A float_listbox Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/float_listbox.hpp
 */

#ifndef NANA_GUI_WIDGETS_FLOAT_LISTBOX_HPP
#define NANA_GUI_WIDGETS_FLOAT_LISTBOX_HPP

#include "widget.hpp"
#include <vector>

namespace nana{ namespace gui{
	namespace drawerbase{
		namespace float_listbox
		{
			class item_renderer
			{
			public:
				typedef widget&	widget_reference;
				typedef paint::graphics& graph_reference;
				enum state_t{StateNone, StateHighlighted};

				virtual ~item_renderer();
				virtual void render(widget_reference, graph_reference, const nana::rectangle&, const nana::string&, state_t state);
				virtual unsigned item_pixels(graph_reference) const;
			};

			//struct module_def
			//@brief: This defines a data structure used for float_listbox
			struct module_def
			{
				std::vector<nana::string> strings;
				unsigned max_items;	//the number of items display.
				mutable unsigned index;		//the result of the selection.

				static const unsigned npos = static_cast<unsigned>(-1);
				module_def();
			};

			class drawer_impl;

			class trigger
				: public nana::gui::drawer_trigger
			{
			public:
				trigger();
				drawer_impl& get_drawer_impl();
				const drawer_impl& get_drawer_impl() const;
			private:
				void bind_window(widget_reference);
				void attached(graph_reference graph);
				void detached();
				void refresh(graph_reference);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
			private:
				class drawer_impl *drawer_;
			};
		}
	}//end namespace drawerbase

	class float_listbox
		: public widget_object<category::root_tag, drawerbase::float_listbox::trigger>
	{
		typedef widget_object<category::root_tag, drawerbase::float_listbox::trigger> base_type;
	public:
		typedef drawerbase::float_listbox::item_renderer item_renderer;
		typedef drawerbase::float_listbox::module_def module_type;

		float_listbox();
		float_listbox(window, int x, int y, unsigned width, unsigned height);

		void set_module(const module_type&);
		void scroll_items(bool upwards);
		void move_items(bool upwards, bool recycle);
		void renderer(item_renderer*);
		std::size_t index() const;
	};
}//end namespace gui
}

#endif
