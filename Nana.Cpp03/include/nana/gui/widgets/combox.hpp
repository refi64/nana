/*
 *	A Combox Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/combox.hpp
 */

#ifndef NANA_GUI_WIDGETS_COMBOX_HPP
#define NANA_GUI_WIDGETS_COMBOX_HPP
#include "widget.hpp"
#include "float_listbox.hpp"
#include <nana/concepts.hpp>
#include <nana/key_type.hpp>
#include <nana/memory.hpp>
#include <functional>

namespace nana{ namespace gui
{
	class combox;
	namespace drawerbase
	{
		namespace combox
		{
			struct extra_events
			{
				nana::fn_group<void(nana::gui::combox&)> selected;
			};

			class drawer_impl;
			
			class trigger
				: public nana::gui::drawer_trigger
			{
			public:
				typedef extra_events ext_event_type;

				trigger();
				~trigger();

				void set_accept(nana::functor<bool(nana::char_t)> &);

				drawer_impl& get_drawer_impl();
				const drawer_impl& get_drawer_impl() const;
			private:
				void attached(widget_reference, graph_reference);
				void detached();
				void refresh(graph_reference);
				void focus(graph_reference, const nana::gui::eventinfo&);
				void mouse_enter(graph_reference, const nana::gui::eventinfo&);
				void mouse_leave(graph_reference, const nana::gui::eventinfo&);
				void mouse_down(graph_reference, const nana::gui::eventinfo&);
				void mouse_up(graph_reference, const nana::gui::eventinfo&);
				void mouse_move(graph_reference, const nana::gui::eventinfo&);
				void mouse_wheel(graph_reference, const nana::gui::eventinfo&);
				void key_down(graph_reference, const nana::gui::eventinfo&);
				void key_char(graph_reference, const nana::gui::eventinfo&);
			private:
				nana::functor<bool(nana::char_t)> pred_acceptive_;
				drawer_impl * drawer_;
			};

			class item_proxy
				: public std::iterator<std::input_iterator_tag, item_proxy>
			{
			public:
				item_proxy(drawer_impl*, std::size_t pos);
				item_proxy&		text(const nana::string&);
				nana::string	text() const;
				item_proxy&	select();
				bool		selected() const;

			public:
				/// Behavior of Iterator's value_type
				bool operator==(const nana::string& s) const;
				bool operator==(const char * s) const;
				bool operator==(const wchar_t * s) const;

				/// Behavior of Iterator
				item_proxy & operator=(const item_proxy&);

				/// Behavior of Iterator
				item_proxy & operator++();

				/// Behavior of Iterator
				item_proxy	operator++(int);

				/// Behavior of Iterator
				item_proxy& operator*();

				/// Behavior of Iterator
				const item_proxy& operator*() const;

				/// Behavior of Iterator
				item_proxy* operator->();

				/// Behavior of Iterator
				const item_proxy* operator->() const;

				/// Behavior of Iterator
				bool operator==(const item_proxy&) const;

				/// Behavior of Iterator
				bool operator!=(const item_proxy&) const;
			private:
				drawer_impl * impl_;
				std::size_t pos_;
			};//end class item_proxy
		}
	}//end namespace drawerbase

	class combox
		:	public widget_object<category::widget_tag, drawerbase::combox::trigger>,
			public concepts::any_objective<std::size_t, 1>
	{
	public:
		typedef float_listbox::item_renderer item_renderer;
		typedef drawerbase::combox::extra_events ext_event_type;
		typedef drawerbase::combox::item_proxy item_proxy;

		combox();
		combox(window, bool visible);
		combox(window, const nana::string& text, bool visible = true);
		combox(window, const nana::char_t* text, bool visible = true);
		combox(window, const rectangle& = rectangle(), bool visible = true);
		
		void clear();
		void editable(bool);
		bool editable() const;
		void set_accept(nana::functor<bool(nana::char_t)>);
		combox& push_back(const nana::string&);
		std::size_t the_number_of_options() const;
		std::size_t option() const;
		void option(std::size_t);
		nana::string text(std::size_t) const;
		void erase(std::size_t pos);

		template<typename Key>
		item_proxy operator[](const Key& kv)
		{
			struct deleter
			{
				void operator()(nana::detail::key_interface * p)
				{
					delete p;
				}
			};
			typedef typename nana::detail::type_escape<Key>::type key_t;
			nana::shared_ptr<nana::detail::key_interface> p(new nana::key<key_t, std::less<key_t> >(kv), deleter());
			return _m_at_key(p);
		}

		template<typename Key>
		void erase_key(const Key& kv)
		{
			typedef typename nana::detail::type_escape<Key>::type key_t;
			nana::key<key_t, std::less<key_t> > key(kv);
			_m_erase(&key);
		}

		ext_event_type& ext_event() const;
		void renderer(item_renderer*);

		void image(std::size_t, const nana::paint::image&);
		nana::paint::image image(std::size_t) const;
		void image_pixels(unsigned);
	private:
		//Override _m_caption for caption()
		nana::string _m_caption() const;
		void _m_caption(const nana::string&);
		nana::any* _m_anyobj(std::size_t, bool allocate_if_empty) const;
		item_proxy _m_at_key(nana::shared_ptr<nana::detail::key_interface>&);
		void _m_erase(nana::detail::key_interface*);
	};
}//end namespace gui
}
#endif
