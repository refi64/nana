/*
 *	A Bedrock Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/win32/bedrock.hpp
 */

#ifndef NANA_GUI_DETAIL_BEDROCK_HPP
#define NANA_GUI_DETAIL_BEDROCK_HPP
#include "native_window_interface.hpp"
#include "window_manager.hpp"
#include "event_manager.hpp"
#include "runtime_manager.hpp"

namespace nana
{
namespace gui
{
	class internal_scope_guard
	{
	public:
		internal_scope_guard();
		~internal_scope_guard();
	};

namespace detail
{
	//class bedrock
	//@brief:	bedrock is a fundamental core component, it provides a abstract to the OS platform
	//			and some basic functions.
	class bedrock
	{
		typedef bedrock self_type;

		bedrock();
	public:
		typedef window_manager<nana::gui::native_window_type, self_type, native_interface > window_manager_t;

		typedef window_manager_t::interface_type	interface_type;
		typedef window_manager_t::core_window_t core_window_t;

		struct thread_context;

		~bedrock();
		void pump_event(nana::gui::window);
		void map_thread_root_buffer(core_window_t* );
		static int inc_window(unsigned tid = 0);
		thread_context* open_thread_context(unsigned tid = 0);
		thread_context* get_thread_context(unsigned tid = 0);
		void remove_thread_context(unsigned tid = 0);
		static bedrock& instance();

		unsigned category(core_window_t*);
		core_window_t* focus();
		native_window_type root(core_window_t*);

		void set_menubar_taken(core_window_t*);
		core_window_t* get_menubar_taken();
		bool close_menu_if_focus_other_window(native_window_type focus);
		void set_menu(native_window_type menu_window, bool is_keyboard_condition);
		native_window_type get_menu(native_window_type owner, bool is_keyboard_condition);
		native_window_type get_menu();
		void remove_menu();
		void empty_menu();

		void get_key_state(nana::gui::detail::tag_keyboard&);
		bool set_keyboard_shortkey(bool yes);
		bool whether_keyboard_shortkey() const;
	public:
		void event_expose(core_window_t *, bool exposed);
		void thread_context_destroy(core_window_t*);
		void thread_context_lazy_refresh();
		void update_cursor(core_window_t *);
	public:
		window_manager_t wd_manager;
		event_manager	evt_manager;
		runtime_manager<core_window_t*, self_type>	rt_manager;
		static bool fire_event_for_drawer(unsigned event_id, core_window_t*, const eventinfo&, thread_context*);
		static bool fire_event(unsigned event_id, core_window_t*, const eventinfo&);

		//raise_event
		//@return: Returns true if the window is available, otherwise returns false
		static bool raise_event(unsigned eventid, core_window_t*, const eventinfo&, bool ask_update);
	private:
		void _m_event_filter(unsigned event_id, core_window_t*, thread_context*);
	private:
		static bedrock bedrock_object;

		struct private_impl;
		private_impl *impl_;
	};//end class bedrock
}//end namespace detail
}//end namespace gui
}//end namespace nana

#endif

