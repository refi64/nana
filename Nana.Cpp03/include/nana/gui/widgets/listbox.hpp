/*
 *	A List Box Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/listbox.hpp
 *
 */

#ifndef NANA_GUI_WIDGETS_LISTBOX_HPP
#define NANA_GUI_WIDGETS_LISTBOX_HPP
#include "widget.hpp"
#include <nana/any.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace listbox
		{
			typedef std::size_t size_type;
			//struct essence_t
			//@brief:	this struct gives many data for listbox,
			//			the state of the struct does not effect on member funcions, therefore all data members are public.
			struct essence_t;
			class drawer_header_impl;
			class drawer_lister_impl;

			class trigger: public nana::gui::drawer_trigger
			{
			public:
				typedef std::size_t size_type;

				trigger();
				~trigger();
				essence_t& essence();
				essence_t& essence() const;
				void draw();
				void update();
			private:
				void _m_draw_border();
			private:
				void bind_window(widget_reference);

				void attached(graph_reference);
				void detached();
				void refresh(graph_reference);
				void mouse_move(graph_reference, const eventinfo&);
				void mouse_leave(graph_reference, const eventinfo&);
				void mouse_down(graph_reference, const eventinfo&);
				void mouse_up(graph_reference, const eventinfo&);
				void mouse_wheel(graph_reference, const eventinfo&);
				void dbl_click(graph_reference, const eventinfo&);
				void resize(graph_reference, const eventinfo&);
				void key_down(graph_reference, const eventinfo&);
			private:
				essence_t * essence_;
				drawer_header_impl *drawer_header_;
				drawer_lister_impl *drawer_lister_;
			};//end class trigger
		}
	}//end namespace drawerbase

	class listbox
		: public nana::gui::widget_object<category::widget_tag, drawerbase::listbox::trigger>
	{
	public:
		typedef drawerbase::listbox::size_type size_type;
		typedef std::pair<size_type, size_type>	index_pair_t;

		listbox();
		listbox(window, bool visible);
		listbox(window, const rectangle& = rectangle(), bool visible = true);

		template<typename T>
		void anyobj(size_type categ, size_type index, const T& t)
		{
			_m_anyobj(categ, index, t);
		}

		template<typename T>
		T * anyobj(size_type categ, size_type index)
		{
			nana::any * p = _m_anyobj(categ, index);
			return (p ? p->get<T>() : 0);
		}

		template<typename T>
		T * anyobj(size_type categ, size_type index) const
		{
			nana::any * p = _m_anyobj(categ, index);
			return (p ? p->get<T>() : 0);
		}

		void auto_draw(bool);
		void append_categ(const nana::string& text);
		void append_header(const nana::string&, unsigned width = 120);
		void append_item(const nana::string&);
		void append_item(size_type categ, const nana::string&);
		void insert(size_type categ, size_type index, const nana::string&);
		void checkable(bool);
		bool checked(size_type item) const;
		bool checked(size_type categ, size_type item) const;
		void checked(std::vector<std::pair<size_type, size_type> >&);
		void checked(size_type categ, size_type item, bool);

		void clear(size_type categ);
		void clear_all();
		void erase(size_type item);
		void erase(size_type categ, size_type item);
		void erase_categ(size_type categ);
		void erase_all();
		nana::string item_text(size_type categ, size_type index, size_type sub) const;
		void set_item_text(size_type index, size_type sub, const nana::string&);
		void set_item_text(size_type categ, size_type index, size_type sub, const nana::string&);
		void show_header(bool);
		bool visible_header() const;
		bool selected(size_type item) const;
		bool selected(size_type categ, size_type item) const;
		void selected(std::vector<index_pair_t>&);
		void selected(size_type categ, size_type item, bool);
		void move_select(bool upwards);
		void icon(size_type categ, size_type index, const nana::paint::image&);
		nana::paint::image icon(size_type categ, size_type index) const;
		void item_background(size_type categ, size_type index, nana::color_t color);
		nana::color_t item_background(size_type categ, size_type index) const;
		void item_foreground(size_type categ, size_type index, nana::color_t color);
		nana::color_t item_foreground(size_type categ, size_type index) const;
		size_type size_categ() const;
		size_type size_item() const;
		size_type size_item(size_type categ) const;
	private:
		void _m_anyobj(size_type categ, size_type index, const nana::any&);
		nana::any* _m_anyobj(size_type categ, size_type index);
		nana::any* _m_anyobj(size_type categ, size_type index) const;
	};
}//end namespace gui
}//end namespace nana

#endif
