/*
 *	Window Manager Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/window_manager.hpp
 *
 *	<Knowledge: 1, 2007-8-17, "Difference between destroy and destroy_handle">
 *		destroy method destroys a window handle and the handles of its children, but it doesn't delete the handle which type is a root window or a frame
 *		destroy_handle method just destroys the handle which type is a root window or a frame
 *
 */

#ifndef NANA_GUI_DETAIL_WINDOW_MANAGER_HPP
#define NANA_GUI_DETAIL_WINDOW_MANAGER_HPP
#include <map>
#include <vector>
#include <algorithm>
#include <nana/paint/image.hpp>
#include "basic_window.hpp"
#include "handle_manager.hpp"
#include "window_layout.hpp"
#include "eventinfo.hpp"
#include <nana/functor.hpp>
#include <functional>
#include <mutex>

namespace nana
{
namespace gui
{
namespace detail
{
	template<typename Key, typename Value>
	class cached_map
	{
	public:
		typedef Key key_type;
		typedef Value value_type;
		typedef std::map<key_type, value_type> std_map_type;

		cached_map()
			: key_(key_type()), value_addr_(nullptr)
		{}

		value_type* insert(key_type key, const value_type& value)
		{
			key_ = key;
			std::pair<typename std_map_type::iterator, bool> ret = map_holder_.insert(std::pair<key_type, value_type>(key, value));
			value_addr_ = &(ret.first->second);
			return value_addr_;
		}

		value_type * find(key_type key) const
		{
			if(key_ == key) return value_addr_;

			key_ = key;
			typename std_map_type::const_iterator i = map_holder_.find(key);
			if(i != map_holder_.end())
				value_addr_ = const_cast<value_type*>(&(i->second));
			else
				value_addr_ = 0;

			return value_addr_;
		}

		void erase(key_type key)
		{
			map_holder_.erase(key);
			key_ = key;
			value_addr_ = 0;
		}
	private:
		mutable key_type key_;
		mutable value_type* value_addr_;
		std_map_type map_holder_;
	};

	struct signals
	{
		enum{caption, destroy, size, count};

		union
		{
			const nana::char_t* caption;
			struct
			{
				unsigned width;
				unsigned height;
			}size;
		}info;
	};

	//signal_manager
	class signal_manager
	{
	public:
        typedef void* identifier;

		~signal_manager();

		template<typename Class>
		bool make(identifier id, Class& object, void (Class::* f)(int, const signals&))
		{
			if(id == 0)	return false;

			inner_invoker * & invk = manager_[id];
			end_ = manager_.end();
			if(invk == 0)
			{
				invk = new (std::nothrow) extend_memfun<Class>(object, f);
				if(invk)	return true;
			}

			return false;
		}

		template<typename Function>
		bool make(identifier id, Function f)
		{
			if(id == 0)	return false;

			inner_invoker * & invk = manager_[id];
			end_ = manager_.end();
			if(invk == 0)
			{
				invk = new (std::nothrow) extend<Function>(f);
				if(invk)	return true;
			}

			return false;
		}

		void umake(identifier);

		void fireaway(identifier, int message, const signals&);
	private:
		struct inner_invoker
		{
			virtual ~inner_invoker();
			virtual void fireaway(int, const signals&) = 0;
		};

		template<typename Class>
		struct extend_memfun: inner_invoker
		{
			extend_memfun(Class& object, void(Class::*f)(int, const signals&))
				:obj_(object), f_(f)
			{}

			void fireaway(int message, const signals& info)
			{
				(obj_.*f_)(message, info);
			}
		private:
			Class& obj_;
			void(Class::*f_)(int, const signals&);
		};

		template<typename Function>
		struct extend: inner_invoker
		{
			extend(Function f):fun_(f){}

			void fireaway(int m, const signals& info)
			{
				fun_(m, info);
			}
		private:
			Function fun_;
		};
	private:
		std::map<identifier, inner_invoker*>	manager_;
		std::map<identifier, inner_invoker*>::iterator end_;
	};

	class shortkey_container
	{
		struct item_type
		{
			nana::gui::window window;
			std::vector<unsigned long> keys;
		};
	public:
		void clear();
		bool make(nana::gui::window, unsigned long key);
		void umake(nana::gui::window);
		nana::gui::window find(unsigned long key);
	private:
		std::vector<item_type> keybase_;
	};

	class tray_event_manager
	{
		typedef std::function<void(const eventinfo&)> function_type;
		typedef std::vector<function_type> fvec_t;
		typedef std::map<unsigned, fvec_t> event_maptable_type;
		typedef std::map<native_window_type, event_maptable_type> maptable_type;
	public:
		void fire(native_window_type, unsigned identifier, const eventinfo&);
		bool make(native_window_type, unsigned identifier, const function_type&);
		void umake(native_window_type);
	private:
		maptable_type maptable_;
	};

	struct root_window_runtime
	{
		typedef basic_window core_window_t;

		core_window_t*			window;
		nana::paint::graphics	root_graph_object;
		shortkey_container		shortkeys;

		struct condition_tag
		{
			core_window_t*	mouse_window;		//the latest window that mouse pressed
			core_window_t*	mousemove_window;	//the latest window that mouse moved
			bool		tabstop_focus_changed;	//KeyDown may set it true, if it is true KeyChar will ignore the message
			condition_tag();
		}condition;

		root_window_runtime(core_window_t* wd, unsigned width, unsigned height);
	};
	
	//class window_manager
	class window_manager
	{
		typedef window_manager self_type;
	public:
		typedef nana::gui::native_window_type	native_window;

		typedef basic_window core_window_t;
		typedef std::vector<core_window_t*> cont_type;

		typedef window_layout<core_window_t>	wndlayout_type;
		typedef cached_map<native_window, root_window_runtime> root_table_type;
		typedef std::function<void(const eventinfo&)> event_fn_t;

		window_manager();

		static bool is_queue(core_window_t*);
		std::size_t number_of_core_window() const;
		std::recursive_mutex& internal_lock() const;
		void all_handles(std::vector<core_window_t*> &v) const;

		template<typename Class>
		bool attach_signal(core_window_t* wd, Class& object, void (Class::*answer)(int, const detail::signals&))
		{
			return signal_manager_.make(wd, object, answer);
		}

		template<typename Function>
		bool attach_signal(core_window_t* wd, Function function)
		{
			return signal_manager_.template make<Function>(wd, function);
		}

		void detach_signal(core_window_t*);
		void signal_fire_caption(core_window_t*, const nana::char_t*);
		void event_filter(core_window_t*, bool is_make, unsigned eventid);
		void default_icon(const nana::paint::image&);

		bool available(core_window_t*);
		bool available(core_window_t *, core_window_t*);
		bool available(nana::gui::native_window_type);

		core_window_t* create_root(core_window_t* owner, bool nested, rectangle, const appearance&);
		core_window_t* create_widget(core_window_t* parent, const rectangle&, bool is_lite);
		core_window_t* create_frame(core_window_t* parent, const rectangle&);
		bool insert_frame(core_window_t* frame, native_window wd);
		bool insert_frame(core_window_t* frame, core_window_t* wd);
		void close(core_window_t*);

		//destroy
		//@brief:	Delete the window handle
		void destroy(core_window_t*);

		//destroy_handle
		//@brief:	Delete window handle, the handle type must be a root and a frame.
		void destroy_handle(core_window_t*);

		void icon(core_window_t*, const paint::image&);

		//show
		//@brief: show or hide a window
		bool show(core_window_t* wd, bool visible);

		core_window_t* find_window(nana::gui::native_window_type root, int x, int y);

		//move the wnd and its all children window, x and y is a relatively coordinate for wnd's parent window
		bool move(core_window_t*, int x, int y);
		bool move(core_window_t*, int x, int y, unsigned width, unsigned height);

		//size
		//@brief: Size a window
		//@param: passive, if it is true, the function would not change the size if wd is a root_widget.
		//			e.g, when the size of window is changed by OS/user, the function should not resize the
		//			window again, otherwise, it causes an infinite loop, because when a root_widget is resized,
		//			window_manager will call the function.
		//
		bool size(core_window_t*, unsigned width, unsigned height, bool passive, bool ask_update);

		core_window_t* root(nana::gui::native_window_type) const;

		nana::gui::event_handle make_drawer_event(int event_id, core_window_t* wd, core_window_t* listener = 0);

		//Copy the root buffer that wnd specified into DeviceContext
		void map(core_window_t*);

		bool belong_to_lazy(core_window_t *) const;

		//update
		//@brief:	update is used for displaying the screen-off buffer.
		//			Because of a good efficiency, if it is called in an event procedure and the event procedure window is the
		//			same as update's, update would not map the screen-off buffer and just set the window for lazy refresh
		bool update(core_window_t*, bool redraw, bool force);

		void refresh(core_window_t*);

		//do_lazy_refresh
		//@brief: defined a behavior of flush the screen
		//@return: it returns true if the wnd is available
		bool do_lazy_refresh(core_window_t*, bool force_copy_to_screen);

		//get_graphics
		//@brief: Get a copy of the graphics object of a window.
		//	the copy of the graphics object has a same buf handle with the graphics object's, they are count-refered
		//	here returns a reference that because the framework does not guarantee the wnd's
		//	graphics object available after a get_graphics call.
		bool get_graphics(core_window_t*, nana::paint::graphics&);
		bool get_visual_rectangle(core_window_t*, nana::rectangle&);

		bool tray_make_event(native_window_type, unsigned identifier, const event_fn_t& f);
		void tray_umake_event(native_window_type);
		void tray_fire(native_window_type, unsigned identifier, const eventinfo&);

		//set_focus
		//@brief: Set a keyboard focus to a window. this may fire a focus event.
		core_window_t* set_focus(core_window_t*);

		core_window_t* capture_redirect(core_window_t*);
		void capture_ignore_children(bool ignore);
		bool capture_window_entered(int root_x, int root_y, bool& prev);
		core_window_t * capture_window() const;

		//capture_window
		//@brief:	set a window that always captures the mouse event if it is not in the range of window
		//@return:	this function dose return the previous captured window. If the wnd set captured twice,
		//			the return value is NULL
		core_window_t* capture_window(core_window_t*, bool value);

		//tabstop
		//@brief: when users press a TAB, the focus should move to the next widget.
		//	this method insert a window which catchs an user TAB into a TAB window container
		//	the TAB window container is held by a wnd's root widget. Not every widget has a TAB window container,
		//	the container is created while a first Tab Window is setting
		void tabstop(core_window_t*);
		core_window_t* tabstop_prev(core_window_t*) const;
		core_window_t* tabstop_next(core_window_t*) const;

		void remove_trash_handle(unsigned tid);

		bool glass_window(core_window_t*, bool isglass);
		void make_glass_background(core_window_t *);

		bool calc_window_point(core_window_t*, nana::point& pos);

		root_table_type::value_type* root_runtime(nana::gui::native_window_type root) const;

		bool register_shortkey(core_window_t*, unsigned long key);
		void unregister_shortkey(core_window_t*);

		core_window_t* find_shortkey(nana::gui::native_window_type, unsigned long key);
	private:
		void _m_destroy(core_window_t*);
		void _m_move_core(core_window_t*, int delta_x, int delta_y);

		//_m_find
		//@brief: find a window on root window through a given root coordinate.
		//		the given root coordinate must be in the rectangle of wnd.
		core_window_t* _m_find(core_window_t*, int x, int y);

		//_m_effective, test if the window is a handle of window that specified by (root_x, root_y)
		static bool _m_effective(core_window_t*, int root_x, int root_y);
	private:
		mutable std::recursive_mutex mutex_;
		handle_manager<core_window_t*, self_type>	handle_manager_;
		root_table_type			root_table_;
		signals					signals_;
		signal_manager	signal_manager_;
		tray_event_manager	tray_event_manager_;

		nana::paint::image default_icon_;

		struct attribute
		{
			struct captured
			{
				core_window_t	*window;
				bool		inside;
				bool		ignore_children;
				std::vector<std::pair<core_window_t*, bool> > history;
			}capture;
		}attr_;

		struct menu_tag
		{
			nana::gui::native_window_type window;
			nana::gui::native_window_type owner;
			bool has_keyboard;
		}menu_;
	};//end class window_manager
}//end namespace detail
}//end namespace gui
}//end namespace nana
#endif
