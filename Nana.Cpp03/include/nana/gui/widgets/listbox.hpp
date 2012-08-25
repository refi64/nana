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
#include <nana/pat/cloneable.hpp>

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
				void typeface_changed(graph_reference);
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

		template<typename T>
		class resolver_interface
		{
		public:
			typedef T target;
			virtual ~resolver_interface(){}
			virtual nana::string decode(std::size_t, const target&) const = 0;
			virtual void encode(target&, std::size_t, const nana::string&) const = 0;
		};
	private:
		template<typename T>
		struct inner_resolver_proxy
		{
			pat::cloneable_interface<resolver_interface<T> > * res;

			inner_resolver_proxy()
				: res(0)
			{}

			inner_resolver_proxy(const inner_resolver_proxy& rhs)
				: res(rhs.res ? rhs.res->clone() : 0)
			{}

			~inner_resolver_proxy()
			{
				if(res)
					res->self_delete();
			}
		};
	public:
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

		template<typename T>
		void append(size_type categ, const T& t)
		{
			inner_resolver_proxy<T> * proxy = _m_resolver().get<inner_resolver_proxy<T> >();
			if(proxy)
			{
				resolver_interface<T> & res = proxy->res->refer();
				size_type index = size_item(categ);
				append_item(categ, res.decode(0, t));

				std::size_t headers = _m_headers();
				for(std::size_t i = 1; i < headers; ++i)
					set_item_text(categ, index, i, res.decode(i, t));
			}
		}

		void insert(size_type categ, size_type index, const nana::string&);

		template<typename T>
		void insert(size_type categ, size_type index, const T& t)
		{
			inner_resolver_proxy<T> * proxy = _m_resolver().get<inner_resolver_proxy<T> >();
			if(proxy)
			{
				resolver_interface<T> & res = proxy->res->refer();
				insert(categ, index, res.decode(0, t));

				std::size_t headers = _m_headers();
				for(std::size_t i = 1; i < headers; ++i)
					set_item_text(categ, index, i, res.decode(i, t));
			}		
		}

		void checkable(bool);
		bool checked(size_type item) const;
		bool checked(size_type categ, size_type item) const;
		void checked(std::vector<std::pair<size_type, size_type> >&);
		void checked(size_type categ, size_type item, bool);

		void clear(size_type categ);
		void clear();
		void erase(size_type categ, size_type item);
		void erase(size_type categ);
		void erase();

		template<typename Resolver>
		void resolver(const Resolver & res)
		{
			inner_resolver_proxy<typename Resolver::target> proxy;
			proxy.res = pat::cloneable<Resolver, resolver_interface<typename Resolver::target> >(res).clone();
			_m_resolver(nana::any(proxy));
		}

		template<typename T>
		bool item(size_type categ, size_type index, T & t) const
		{
			inner_resolver_proxy<T> * proxy = _m_resolver().get<inner_resolver_proxy<T> >();
			if(proxy)
			{
				resolver_interface<T> & res = proxy->res->refer();
				std::size_t headers = _m_headers();
				for(std::size_t i = 0; i < headers; ++i)
					res.encode(t, i, item_text(categ, index, i));
				return true;
			}
			return false;
		}

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
		void _m_resolver(const nana::any&);
		const nana::any & _m_resolver() const;
		std::size_t _m_headers() const;
	};
}//end namespace gui
}//end namespace nana

#endif