/*
 *	A Menu implementation
 *	Copyright(C) 2009 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/menu.hpp
 */

#ifndef NANA_GUI_WIDGETS_MENU_HPP
#define NANA_GUI_WIDGETS_MENU_HPP
#include "widget.hpp"
#include <vector>
#include <nana/gui/timer.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace menu
		{
			struct menu_type; //declaration

			struct menu_item_type
			{
				//class item_proxy
				//@brief: this class is used as parameter of menu event function.
				class item_proxy
					: nana::noncopyable
				{
				public:
					item_proxy(unsigned long id, menu_item_type &);
					void enabled(bool v);
					bool enabled() const;
					unsigned long index() const;
				private:
					unsigned long index_;
					menu_item_type &item_;
				};

				typedef nana::functor<void(item_proxy&)> functor_type;

				//Default constructor initializes the item as a splitter
				menu_item_type();
				menu_item_type(const nana::string& text, const functor_type& f);

				struct
				{
					bool enabled:1;
					bool splitter:1;
					bool checked:1;
				}flags;

				menu_type		*sub_menu;
				nana::string	text;
				functor_type	functor;
				int				style;
				paint::image	image;
				mutable nana::char_t	hotkey;
			};

			struct menu_type
			{
				typedef std::vector<menu_item_type> item_container;
				typedef item_container::iterator iterator;
				typedef item_container::const_iterator const_iterator;

				menu_type();

				std::vector<menu_type*>		owner;
				std::vector<menu_item_type>	items;
				unsigned max_pixels;
			};

			class menu_builder
			{
			public:
				typedef menu_item_type item_type;

				typedef menu_type::item_container::value_type::functor_type functor_type;
				typedef menu_type::item_container::iterator iterator;
				typedef menu_type::item_container::const_iterator const_iterator;

				~menu_builder();
				void append(const nana::string&, const functor_type&);
				void append_splitter();
				void clear();
				void check_style(std::size_t index, int style);
				void checked(std::size_t index, bool check);
				menu_type& get_root();
				void insert(std::size_t index, const nana::string&, const functor_type&);
				item_type& at(std::size_t index);
				const item_type& at(std::size_t index) const;
				void erase(std::size_t index);
				bool set_sub_menu(std::size_t index, menu_type &);
				void remove_sub_menu(std::size_t index);
				void destroy();
				unsigned size() const;
				void max_pixels(unsigned);
				unsigned max_pixels() const;
			private:
				menu_type root_;
			};//end class menu_builder


			class item_renderer
			{
			public:
				typedef nana::paint::graphics& graph_reference;

				enum{STATE_NORMAL, STATE_ACTIVE};
				struct state_type
				{
					int state;
					bool enabled;
					bool checked;
					int check_style;
				};

				item_renderer(graph_reference);
				virtual ~item_renderer();
				graph_reference graph() const;
				virtual void background(const nana::point&, const nana::size&, const state_type&);
				virtual void image(int x, int y, const paint::image&);
				virtual void caption(int x, int y, const nana::string &, const state_type&, unsigned strpixels);
			private:
				graph_reference graph_;
			};

			class menu_drawer
				: public nana::gui::drawer_trigger
			{
				typedef menu_drawer self_type;
			public:
				unsigned static const npos = static_cast<unsigned>(-1);
				typedef menu_item_type::item_proxy item_proxy;

				menu_drawer();
				void bind_window(widget_reference widget);
				void attached(graph_reference graph);
				void detached();
				void mouse_move(graph_reference, const nana::gui::eventinfo& ei);
				void mouse_leave(graph_reference, const nana::gui::eventinfo& ei);
				void mouse_down(graph_reference graph, const nana::gui::eventinfo& ei);
				void refresh(graph_reference);
			public:
				unsigned long active() const;
				bool goto_next(bool forword);
				bool track_mouse(int x, int y);
				void set_menu_drawer(menu_type &);
				menu_type* get_menu() const;
				void set_sub_window(bool subw);
				menu_type* retrive_sub_menu(nana::point& pos, unsigned long interval) const;
				//send_shortkey has 3 states, 0 = UNKNOWN KEY, 1 = ITEM, 2 = GOTO SUBMENU
				int send_shortkey(nana::char_t key);
				void draw() const;
			private:
				static item_renderer::state_type _m_make_renderer_state(bool active, const menu_item_type &);
				unsigned long _m_get_index_by_pos(int x, int y) const;
				unsigned _m_item_height() const;
				nana::size _m_client_size() const;
				void _m_adjust_window_size() const;
			private:
				nana::gui::widget		*widget_;
				nana::paint::graphics	*graph_;
				menu_type	*menu_;

				struct state
				{
					state();

					unsigned long active;
					unsigned long active_timestamp;
					unsigned long sub_window: 1;
					unsigned long nullify_mouse: 1;
				}state_;

				struct widget_detail
				{
					widget_detail();
					nana::upoint border;
				}detail_;
			};

			class menu_window
				:	public nana::gui::widget_object<nana::gui::category::root_tag, menu_drawer>
			{
				typedef menu_window self_type;
				typedef menu_drawer drawer_type;
				typedef nana::gui::widget_object<nana::gui::category::root_tag, menu_drawer> base_type;
			public:
				static const unsigned long npos = drawer_type::npos;
				typedef menu_builder::item_type item_type;

				menu_window(window, const point&);
				void popup(menu_type&, bool owner_menubar);
				void goto_next(bool forward);
				bool goto_submenu();
				bool exit_submenu();
				int send_shortkey(nana::char_t key);
			private:
				//_m_destroy just destroys the children windows.
				//If the all window including parent windows want to be closed, call the _m_close_all() instead of close()
				void _m_close_all();
				void _m_destroy();
				void _m_strike();
				void _m_key_down(const nana::gui::eventinfo& ei);
				void _m_make_mouse_event();
				void _m_mouse_event();
				bool _m_show_submenu(menu_type* sbm, nana::point pos, bool forced);
				void _m_check_repeatly();
			private:
				nana::gui::timer timer_;
				struct state_type
				{
					bool self_submenu; //Indicates whether the menu window is used for a submenu
					bool owner_menubar;
					bool auto_popup_submenu;
					nana::point mouse_pos;
				}state_;

				struct submenu_type
				{
					menu_window*	parent;
					menu_window*	child;
					const menu_type *object;
				}submenu_;
			};

		}//end namespace menu
	}//end namespace drawerbase

	class menu
		: private nana::noncopyable
	{
		typedef menu self_type;

		struct menu_info
		{
			menu * handle;
			bool kill;
		};
	public:
		enum check_t{check_none, check_option, check_highlight};

		typedef drawerbase::menu::menu_builder::item_type item_type;
		typedef item_type::item_proxy item_proxy;
		typedef item_type::functor_type event_fn_t;
		
		menu();
		~menu();
		void append(const nana::string& text, const event_fn_t& = event_fn_t());
		void append_splitter();
		void clear();
		void close();
		void image(std::size_t index, const paint::image&);
		void check_style(std::size_t index, check_t style);
		void checked(std::size_t index, bool check);
		bool checked(std::size_t index) const;
		void enabled(std::size_t index, bool enable);
		bool enabled(std::size_t index) const;
		void erase(std::size_t index);
		bool link(std::size_t index, menu& menu_obj);
		menu * link(std::size_t index);
		menu *create_sub_menu(std::size_t index);
		void popup(window, int x, int y, bool owner_menubar);
		void answerer(std::size_t index, const event_fn_t&);
		void destroy_answer(const nana::functor<void()>&);
		void goto_next(bool forward);
		bool goto_submen();
		bool exit_submenu();
		unsigned size() const;
		int send_shortkey(nana::char_t key);
		menu& max_pixels(unsigned);
		unsigned max_pixels() const;
	private:
		void _m_destroy_menu_window();
	private:
		drawerbase::menu::menu_builder mbuilder_;
		drawerbase::menu::menu_window *window_;
		nana::functor<void()> functor_;
		std::map<std::size_t, menu_info> sub_container_;
	};

	namespace detail
	{
		class popuper
		{
		public:
			popuper(menu&, mouse::t);
			popuper(menu&, window owner, const point&, mouse::t);
			void operator()(const eventinfo&);
		private:
			menu & mobj_;
			nana::gui::window owner_;
			bool take_mouse_pos_;
			nana::point pos_;
			mouse::t mouse_;
		};
	}

	detail::popuper menu_popuper(menu&, mouse::t = mouse::right_button);
	detail::popuper menu_popuper(menu&, window owner, const point&, mouse::t = mouse::right_button);
}//end namespace gui
}//end namespace nana
#endif

