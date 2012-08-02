/*
 *	A Bedrock Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/bedrock.cpp
 */

#include <nana/config.hpp>
#include PLATFORM_SPEC_HPP
#include GUI_BEDROCK_HPP
#include <nana/gui/detail/eventinfo.hpp>
#include <nana/system/platform.hpp>
#include <errno.h>
#include <stdexcept>

namespace nana
{
namespace gui
{
	//class internal_scope_guard
		internal_scope_guard::internal_scope_guard()
		{
			detail::bedrock::instance().wd_manager.internal_lock().lock();
		}
		internal_scope_guard::~internal_scope_guard()
		{
			detail::bedrock::instance().wd_manager.internal_lock().unlock();
		}
	//end class internal_scope_guard
namespace detail
{
#pragma pack(1)
		union event_mask
		{
			struct
			{
				short x;
				short y;
			}pos;

			struct
			{
				short width;
				short height;
			}size;

			struct
			{
				unsigned short vkey;
				short delta;
			}wheel;
		};
#pragma pack()

	struct bedrock::private_impl
	{
		nana::threads::token token;
		context_container tcontexts;

		struct cache_type
		{
			struct thread_context_cache
			{
				unsigned tid;
				thread_context *object;
			}tcontext;

			cache_type()
			{
				tcontext.tid = 0;
				tcontext.object = 0;
			}
		}cache;

		struct menu_tag
		{
			menu_tag()
				:taken_window(0), window(0), owner(0), has_keyboard(false)
			{}

			core_window_t*	taken_window;
			nana::gui::native_window_type window;
			nana::gui::native_window_type owner;
			bool has_keyboard;
		}menu;

		struct keyboard_tracking_state
		{
			keyboard_tracking_state()
				:has_shortkey_occured(false), has_keyup(true), alt(0)
			{}

			bool has_shortkey_occured;
			bool has_keyup;

			unsigned long alt : 2;
		}keyboard_tracking_state;
	};

	void timer_proc(unsigned);
	void window_proc_dispatcher(Display*, nana::detail::msg_packet_tag&);
	void window_proc_for_packet(Display *, nana::detail::msg_packet_tag&);
	void window_proc_for_xevent(Display*, XEvent&);

	bedrock::thread_context::thread_context()
		: event_pump_ref_count(0), window_count(0), event_window(0)
	{
	}

	//class bedrock defines a static object itself to implement a static singleton
	//here is the definition of this object
	bedrock bedrock::bedrock_object;

	inline nana::gui::window mycast(bedrock::core_window_t* wnd)
	{
		return reinterpret_cast<nana::gui::window>(wnd);
	}

	Window event_window(const XEvent& event)
	{
		switch(event.type)
		{
		case MapNotify:
		case UnmapNotify:
		case DestroyNotify:
			return event.xmap.window;
		}
		return event.xkey.window;
	}

	bedrock::bedrock()
		: impl_(new private_impl)
	{
		nana::detail::platform_spec::instance().msg_set(timer_proc, window_proc_dispatcher);
	}

	bedrock::~bedrock()
	{
		delete impl_;
	}

	void bedrock::map_thread_root_buffer(bedrock::core_window_t* wnd)
	{
	//	::PostMessage(reinterpret_cast<HWND>(wnd->root), gui::messages::map_thread_root_buffer, reinterpret_cast<WPARAM>(wnd), 0);
	}

	/*
	 *	inc_window
	 *	@biref: increament the number of windows
	 */
	int bedrock::inc_window(unsigned tid)
	{
		private_impl * impl = instance().impl_;
		nana::threads::scope_guard sg(impl->token);

		int & cnt = (impl->tcontexts[tid ? tid : nana::system::this_thread_id()].window_count);
		return (cnt < 0 ? cnt = 1 : ++cnt);
	}

	bedrock::thread_context* bedrock::open_thread_context(unsigned tid)
	{
		if(0 == tid) tid = nana::system::this_thread_id();
		nana::threads::scope_guard sg(impl_->token);
		if(impl_->cache.tcontext.tid == tid)
			return impl_->cache.tcontext.object;

		bedrock::thread_context* context = 0;

		context_container::iterator i = impl_->tcontexts.find(tid);
		if(i == impl_->tcontexts.end())
			context = &(impl_->tcontexts[tid]);
		else
			context = &(i->second);

		impl_->cache.tcontext.tid = tid;
		impl_->cache.tcontext.object = context;

		return context;
	}

	bedrock::thread_context* bedrock::get_thread_context(unsigned tid)
	{
		if(0 == tid) tid = nana::system::this_thread_id();

		nana::threads::scope_guard sg(impl_->token);

		if(impl_->cache.tcontext.tid == tid)
			return impl_->cache.tcontext.object;

		context_container::iterator i = impl_->tcontexts.find(tid);
		if(i != impl_->tcontexts.end())
		{
			impl_->cache.tcontext.tid = tid;
			return (impl_->cache.tcontext.object = &(i->second));
		}

		impl_->cache.tcontext.tid = 0;
		return 0;
	}

	void bedrock::remove_thread_context(unsigned tid)
	{
		if(0 == tid) tid = nana::system::this_thread_id();

		nana::threads::scope_guard sg(impl_->token);

		if(impl_->cache.tcontext.tid == tid)
		{
			impl_->cache.tcontext.tid = 0;
			impl_->cache.tcontext.object = 0;
		}

		impl_->tcontexts.erase(tid);
	}

	bedrock& bedrock::instance()
	{
		return bedrock_object;
	}

	unsigned bedrock::category(bedrock::core_window_t* wd)
	{
		if(wd)
		{
			internal_scope_guard isg;
			if(this->wd_manager.available(wd))
				return wd->other.category;
		}
		return 0;
	}

	bedrock::core_window_t* bedrock::focus()
	{
		core_window_t* wd = wd_manager.root(native_interface::get_focus_window());
		return (wd ? wd->other.attribute.root->focus : 0);
	}

	native_window_type bedrock::root(core_window_t* wd)
	{
		if(wd)
		{
			internal_scope_guard isg;
			if(this->wd_manager.available(wd))
				return wd->root;
		}
		return 0;
	}

	void bedrock::set_menubar_taken(core_window_t* wd)
	{
		this->impl_->menu.taken_window = wd;
	}

	bedrock::core_window_t* bedrock::get_menubar_taken()
	{
		core_window_t* wd = this->impl_->menu.taken_window;
		this->impl_->menu.taken_window = 0;
		return wd;
	}

	bool bedrock::close_menu_if_focus_other_window(nana::gui::native_window_type wd)
	{
		if(this->impl_->menu.window && (this->impl_->menu.window != wd))
		{
			wd = native_interface::get_owner_window(wd);
			while(wd)
			{
				if(wd != this->impl_->menu.window)
					wd = native_interface::get_owner_window(wd);
				else
					return false;
			}
			this->remove_menu();
			return true;
		}
		return false;
	}

	void bedrock::set_menu(nana::gui::native_window_type menu_window, bool has_keyboard)
	{
		if(menu_window && this->impl_->menu.window != menu_window)
		{
			remove_menu();
			this->impl_->menu.window = menu_window;
			this->impl_->menu.owner = native_interface::get_owner_window(menu_window);
			this->impl_->menu.has_keyboard = has_keyboard;
		}
	}

	nana::gui::native_window_type bedrock::get_menu(nana::gui::native_window_type owner, bool is_keyboard_condition)
	{
		if(	(this->impl_->menu.owner == 0) ||
			(owner && (this->impl_->menu.owner == owner))
			)
		{
			return ( is_keyboard_condition ? (this->impl_->menu.has_keyboard ? this->impl_->menu.window : 0) : this->impl_->menu.window);
		}

		return 0;
	}

	nana::gui::native_window_type bedrock::get_menu()
	{
		return this->impl_->menu.window;
	}

	void bedrock::remove_menu()
	{
		if(this->impl_->menu.window)
		{
			nana::gui::native_window_type delwin = this->impl_->menu.window;
			this->impl_->menu.window = this->impl_->menu.owner = 0;
			this->impl_->menu.has_keyboard = false;
			native_interface::close_window(delwin);
		}
	}

	void bedrock::empty_menu()
	{
		if(impl_->menu.window)
		{
			impl_->menu.window = impl_->menu.owner = 0;
			impl_->menu.has_keyboard = false;
		}
	}

	void bedrock::get_key_state(nana::gui::detail::tag_keyboard& kb)
	{
		XKeyEvent xkey;
		nana::detail::platform_spec::instance().read_keystate(xkey);
		kb.ctrl = (xkey.state & ControlMask);
	}

	bool bedrock::set_keyboard_shortkey(bool yes)
	{
		bool ret = impl_->keyboard_tracking_state.has_shortkey_occured;
		impl_->keyboard_tracking_state.has_shortkey_occured = yes;
		return ret;
	}

	void make_eventinfo(nana::gui::eventinfo& ei, nana::gui::detail::bedrock::core_window_t* wd, unsigned int msg, const XEvent& event)
	{
		ei.window = reinterpret_cast<nana::gui::window>(wd);
		if(msg == ButtonPress || msg == ButtonRelease)
		{
			if(event.xbutton.button == Button4 || event.xbutton.button == Button5)
			{
				ei.wheel.upwards = (event.xbutton.button == Button4);
				ei.wheel.x = event.xbutton.x - wd->root_x;
				ei.wheel.y = event.xbutton.y - wd->root_y;
			}
			else
			{
				ei.mouse.x = event.xbutton.x - wd->root_x;
				ei.mouse.y = event.xbutton.y - wd->root_y;

				ei.mouse.left_button = ei.mouse.mid_button = ei.mouse.right_button = false;
				ei.mouse.shift = ei.mouse.ctrl = false;
				switch(event.xbutton.button)
				{
				case Button1:
					ei.mouse.left_button = true;
					break;
				case Button2:
					ei.mouse.mid_button = true;
					break;
				case Button3:
					ei.mouse.right_button = true;
					break;
				}
			}
		}
		else if(msg == MotionNotify)
		{
			ei.mouse.x = event.xmotion.x - wd->root_x;
			ei.mouse.y = event.xmotion.y - wd->root_y;
			ei.mouse.left_button = ei.mouse.mid_button = ei.mouse.right_button = false;

			ei.mouse.shift = event.xmotion.state & ShiftMask;
			ei.mouse.ctrl = event.xmotion.state & ControlMask;
			if(event.xmotion.state & Button1Mask)
				ei.mouse.left_button = true;
			else if(event.xmotion.state & Button2Mask)
				ei.mouse.right_button = true;
			else if(event.xmotion.state & Button3Mask)
				ei.mouse.mid_button = true;
		}
	}

	void timer_proc(unsigned tid)
	{
		nana::detail::platform_spec::instance().timer_proc(tid);
	}

	void window_proc_dispatcher(Display* display, nana::detail::msg_packet_tag& msg)
	{
		switch(msg.kind)
		{
		case nana::detail::msg_packet_tag::kind_xevent:
			window_proc_for_xevent(display, msg.u.xevent);
			break;
		case nana::detail::msg_packet_tag::kind_mouse_drop:
			window_proc_for_packet(display, msg);
			break;
		default:	break;
		}
	}

	void window_proc_for_packet(Display * display, nana::detail::msg_packet_tag& msg)
	{
		typedef nana::gui::detail::bedrock::core_window_t core_window_t;
		typedef detail::bedrock::window_manager_t::root_table_type::value_type wm_root_runtime_type;

		static nana::gui::detail::bedrock& bedrock = nana::gui::detail::bedrock::instance();

		nana::gui::native_window_type native_window = reinterpret_cast<nana::gui::native_window_type>(msg.u.packet_window);
		wm_root_runtime_type*	root_runtime = bedrock.wd_manager.root_runtime(native_window);

		if(root_runtime)
		{
			core_window_t* msgwd = root_runtime->window;

			eventinfo ei;
			switch(msg.kind)
			{
			case nana::detail::msg_packet_tag::kind_mouse_drop:
				msgwd = bedrock.wd_manager.find_window(native_window, msg.u.mouse_drop.x, msg.u.mouse_drop.y);
				if(msgwd)
				{
					nana::gui::detail::tag_dropinfo di;
					di.filenames.swap(*msg.u.mouse_drop.files);
					delete msg.u.mouse_drop.files;
					di.pos.x = msg.u.mouse_drop.x - msgwd->root_x;
					di.pos.y = msg.u.mouse_drop.y - msgwd->root_y;
					ei.dropinfo = & di;
					ei.window = reinterpret_cast<nana::gui::window>(msgwd);
					
					bedrock.fire_event(event_tag::mouse_drop, msgwd, ei);
					bedrock.wd_manager.refresh(msgwd);
				}
				break;
			default:
				throw std::runtime_error("Nana.GUI.Bedrock: Undefined message packet");
			}
		}		

	}

	void window_proc_for_xevent(Display* display, XEvent& xevent)
	{
		typedef nana::gui::detail::bedrock::core_window_t core_window_t;
		typedef detail::bedrock::window_manager_t::root_table_type::value_type wm_root_runtime_type;

		static nana::gui::detail::bedrock& bedrock = nana::gui::detail::bedrock::instance();
		static unsigned long	last_mouse_down_time;
		static core_window_t*	last_mouse_down_window;

		nana::gui::native_window_type native_window = reinterpret_cast<nana::gui::native_window_type>(event_window(xevent));
		wm_root_runtime_type*	root_runtime = bedrock.wd_manager.root_runtime(native_window);


		if(root_runtime)
		{
			core_window_t* msgwnd = root_runtime->window;
			bedrock::thread_context& context = *bedrock.get_thread_context(msgwnd->thread_id);

			core_window_t* pre_event_window = context.event_window;
			core_window_t* mouse_window = root_runtime->condition.mouse_window;
			core_window_t* mousemove_window = root_runtime->condition.mousemove_window;

			eventinfo ei;

			const int message = xevent.type;
			switch(xevent.type)
			{
			case EnterNotify:
				break;
			case LeaveNotify:
				if(bedrock.wd_manager.available(mousemove_window) && mousemove_window->flags.enabled)
				{
					ei.mouse.x = ei.mouse.y = 0;
					mousemove_window->flags.action = mouse_action_normal;
					ei.window = reinterpret_cast<nana::gui::window>(mousemove_window);
					bedrock.raise_event(event_tag::mouse_leave, mousemove_window, ei, true);
				}
				mousemove_window = 0;
				break;
			case FocusIn:
				if(msgwnd->flags.enabled && msgwnd->flags.take_active)
				{
					core_window_t * focus = msgwnd->other.attribute.root->focus;
					if(focus && focus->together.caret)
						focus->together.caret->set_active(true);
					msgwnd->root_widget->other.attribute.root->context.focus_changed = true;
					ei.focus.getting = true;
					ei.focus.receiver = native_window;
					if(false == bedrock.raise_event(event_tag::focus, focus, ei, true))
						bedrock.wd_manager.set_focus(msgwnd);
				}
				break;
			case FocusOut:
				if(msgwnd->other.attribute.root->focus && native_interface::is_window(msgwnd->root))
				{
					nana::point pos = native_interface::cursor_position();
					native_window_type recv = native_interface::find_window(pos.x, pos.y);

					core_window_t * focus = msgwnd->other.attribute.root->focus;
					ei.focus.getting = false;
					ei.focus.receiver = recv;
					if(bedrock.raise_event(event_tag::focus, focus, ei, true))
					{
						if(focus->together.caret)
							focus->together.caret->set_active(false);
					}
					bedrock.close_menu_if_focus_other_window(recv);
				}
				break;
			case ConfigureNotify:
				if(msgwnd->rect.width != static_cast<unsigned>(xevent.xconfigure.width) || msgwnd->rect.height != static_cast<unsigned>(xevent.xconfigure.height))
				{
					ei.size.width = xevent.xconfigure.width;
					ei.size.height = xevent.xconfigure.height;
					bedrock.wd_manager.size(msgwnd, ei.size.width, ei.size.height, true, true);
				}
				break;
			case ButtonPress:
				msgwnd = bedrock.wd_manager.find_window(native_window, xevent.xbutton.x, xevent.xbutton.y);

				if(msgwnd && (msgwnd == msgwnd->root_widget->other.attribute.root->menubar) && bedrock.get_menu(msgwnd->root, true))
				{
					bedrock.remove_menu();
				}
				else
					bedrock.close_menu_if_focus_other_window(msgwnd->root);

				if(msgwnd && msgwnd->flags.enabled)
				{
					bool dbl_click = (last_mouse_down_window == msgwnd) && (xevent.xbutton.time - last_mouse_down_time <= 400);
					last_mouse_down_time = xevent.xbutton.time;
					last_mouse_down_window = msgwnd;

					mouse_window = msgwnd;
					core_window_t* new_focus = (msgwnd->flags.take_active ? msgwnd : msgwnd->other.active_window);

					if(new_focus)
					{
						context.event_window = new_focus;
						core_window_t* kill_focus = bedrock.wd_manager.set_focus(new_focus);
						if(kill_focus != new_focus)
							bedrock.wd_manager.do_lazy_refresh(kill_focus, false);
					}
					msgwnd->root_widget->other.attribute.root->context.focus_changed = false;

					context.event_window = msgwnd;

					make_eventinfo(ei, msgwnd, message, xevent);
					msgwnd->flags.action = mouse_action_pressed;
					if(bedrock.raise_event(dbl_click ? event_tag::dbl_click : event_tag::mouse_down, msgwnd, ei, true))
					{
						//If a root window is created during the mouse_down event, Nana.GUI will ignore the mouse_up event.
						if(msgwnd->root_widget->other.attribute.root->context.focus_changed)
						{
							//call the drawer mouse up event for restoring the surface graphics
							msgwnd->flags.action = mouse_action_normal;
							bedrock.fire_event_for_drawer(event_tag::mouse_up, msgwnd, ei, &context);
							bedrock.wd_manager.do_lazy_refresh(msgwnd, false);
						}
					}
					else
						mouse_window = 0;
				}
				break;
			case ButtonRelease:
				if(xevent.xbutton.button == Button4 || xevent.xbutton.button == Button5)
				{
					msgwnd = bedrock.focus();
					if(msgwnd && msgwnd->flags.enabled)
					{
						make_eventinfo(ei, msgwnd, message, xevent);
						bedrock.raise_event(event_tag::mouse_wheel, msgwnd, ei, true);
					}
				}
				else if(mouse_window && mouse_window->flags.enabled)
				{
					make_eventinfo(ei, mouse_window, message, xevent);
					msgwnd = bedrock.wd_manager.find_window(native_window, xevent.xbutton.x, xevent.xbutton.y);
					core_window_t* click_window = 0;
					if(msgwnd == mouse_window)
					{
						mouse_window->flags.action = mouse_action_over;
						click_window = mouse_window;
						context.event_window = msgwnd;
						bedrock.fire_event_for_drawer(event_tag::click, msgwnd, ei, &context);
					}
					else
					{
						mouse_window->flags.action = mouse_action_normal;
						msgwnd = mouse_window;
					}
					context.event_window = msgwnd;
					bedrock.fire_event_for_drawer(event_tag::mouse_up, msgwnd, ei, &context);
					bedrock.wd_manager.refresh(msgwnd);

					if(click_window)
					{
						context.event_window = click_window;
						bedrock.fire_event(event_tag::click, click_window, ei);
						if(click_window != msgwnd)
							bedrock.wd_manager.do_lazy_refresh(click_window, false);
					}

					context.event_window = msgwnd;
					bedrock.fire_event(event_tag::mouse_up, msgwnd, ei);
					bedrock.wd_manager.do_lazy_refresh(msgwnd, false);

					mouse_window = 0;
				}
				break;
			case DestroyNotify:
				{
					nana::detail::platform_spec & spec = nana::detail::platform_spec::instance();
					if(bedrock.wd_manager.available(msgwnd))
					{
						//The msgwnd may be destroyed if the window is destroyed by calling native interface of close_window().
						if(msgwnd->root == bedrock.get_menu())
							bedrock.empty_menu();

						spec.remove(native_window);
						bedrock.wd_manager.destroy(msgwnd);
						bedrock.evt_manager.umake(reinterpret_cast<gui::window>(msgwnd));

						bedrock.rt_manager.remove_if_exists(msgwnd);
						bedrock.wd_manager.destroy_handle(msgwnd);
					}
					if(--context.window_count <= 0)
					{

					}
				}
				break;
			case MotionNotify:
				msgwnd = bedrock.wd_manager.find_window(native_window, xevent.xbutton.x, xevent.xbutton.y);
				if(mousemove_window && (msgwnd != mousemove_window))
				{
					//if current window is not the previous mouse event window.
					make_eventinfo(ei, mousemove_window, message, xevent);
					mousemove_window->flags.action = mouse_action_normal;
					bedrock.raise_event(event_tag::mouse_leave, mousemove_window, ei, true);

					//if msgwnd is neither captured window nor the child of captured window,
					//redirect the msgwnd to the captured window.
					core_window_t* wd = bedrock.wd_manager.capture_redirect(msgwnd);
					if(wd)
						msgwnd = wd;
				}
				else
				{
					make_eventinfo(ei, msgwnd, message, xevent);
					bool prev_captured_inside;
					if(bedrock.wd_manager.capture_window_entered(ei.mouse.x, ei.mouse.y, prev_captured_inside))
					{
						unsigned eid;
						if(prev_captured_inside)
						{
							eid = event_tag::mouse_leave;
							msgwnd->flags.action = mouse_action_normal;
						}
						else
						{
							eid = event_tag::mouse_enter;
							msgwnd->flags.action = mouse_action_over;
						}
						bedrock.raise_event(eid, msgwnd, ei, false);
					}
				}

				if(msgwnd)
				{
					make_eventinfo(ei, msgwnd, message, xevent);
					msgwnd->flags.action = mouse_action_over;
					if(mousemove_window != msgwnd)
						bedrock.raise_event(event_tag::mouse_enter, msgwnd, ei, true);
					bedrock.raise_event(event_tag::mouse_move, msgwnd, ei, true);
					mousemove_window = msgwnd;
				}
				break;
			case MapNotify:
			case UnmapNotify:
				bedrock.event_expose(msgwnd, (xevent.type == MapNotify));
				break;
			case Expose:
				if(msgwnd->visible && (msgwnd->root_graph->empty() == false))
				{
					nana::detail::platform_scope_guard psg;
					nana::detail::drawable_impl_type* drawer_impl = msgwnd->root_graph->handle();
					::XCopyArea(display, drawer_impl->pixmap, reinterpret_cast<Window>(native_window), drawer_impl->context,
							xevent.xexpose.x, xevent.xexpose.y,
							xevent.xexpose.width, xevent.xexpose.height,
							xevent.xexpose.x, xevent.xexpose.y);
				}
				break;
			case KeyPress:
				nana::detail::platform_spec::instance().write_keystate(xevent.xkey);
				if(msgwnd->flags.enabled)
				{

					if(msgwnd->root != bedrock.get_menu())
						msgwnd = bedrock.focus();

					if(msgwnd)
					{
						KeySym keysym;
						Status status;
						char fixbuf[33];
						char * keybuf = fixbuf;
						int len = 0;
						XIC input_context = nana::detail::platform_spec::instance().caret_input_context(native_window);
						if(input_context)
						{
							nana::detail::platform_scope_guard psg;
#if defined(NANA_UNICODE)
							len = ::Xutf8LookupString(input_context, &xevent.xkey, keybuf, 32, &keysym, &status);
							if(status == XBufferOverflow)
							{
								keybuf = new char[len + 1];
								len = ::Xutf8LookupString(input_context, &xevent.xkey, keybuf, len, &keysym, &status);
							}
#else
							len = ::XmbLookupString(input_context, &xevent.xkey, keybuf, 32, &keysym, &status);
							if(status == XBufferOverflow)
							{
								keybuf = new char[len + 1];
								len = ::XmbLookupString(input_context, &xevent.xkey, keybuf, len, &keysym, &status);
							}
#endif
						}
						else
						{
							nana::detail::platform_scope_guard psg;
							status = XLookupBoth;
							len = ::XLookupString(&xevent.xkey, keybuf, 32, &keysym, 0);
						}

						keybuf[len] = 0;
						nana::char_t keychar;
						switch(status)
						{
						case XLookupKeySym:
						case XLookupBoth:
							switch(keysym)
							{
							case XK_Alt_L: case XK_Alt_R:
								keychar = keyboard::alt; break;
							case XK_BackSpace:
								keychar = keyboard::backspace; break;
							case XK_Tab:
								keychar = keyboard::tab; break;
							case XK_Escape:
								keychar = keyboard::esc; break;
							case XK_Return:
								keychar = keyboard::enter; break;
							case XK_Cancel:
								keychar = keyboard::cancel; break;	//Ctrl+C
							case XK_Page_Up:
								keychar = keyboard::pageup; break;
							case XK_Page_Down:
								keychar = keyboard::pagedown; break;
							case XK_Left: case XK_Up: case XK_Right: case XK_Down:
								keychar = keyboard::left + (keysym - XK_Left); break;
							case XK_Insert:
								keychar = keyboard::insert; break;
							case XK_Delete:
								keychar = keyboard::del; break;
							default:
								keychar = 0xFF;
							}
							context.platform.keychar = keychar;
							if(keychar == keyboard::tab && (false == (msgwnd->flags.tab & nana::gui::detail::tab_type::eating))) //Tab
							{
								core_window_t* the_next = bedrock.wd_manager.tabstop_next(msgwnd);
								if(the_next)
								{
									bedrock.wd_manager.set_focus(the_next);
									bedrock.wd_manager.do_lazy_refresh(the_next, true);
									root_runtime->condition.tabstop_focus_changed = true;
								}
							}
							else if(keychar != 0xFF)
							{
								ei.keyboard.key = keychar;
								bedrock.get_key_state(ei.keyboard);
								bedrock.raise_event(event_tag::key_down, msgwnd, ei, true);
							}

							if(XLookupKeySym == status)
							{
								bedrock.wd_manager.do_lazy_refresh(msgwnd, false);
								break;
							}
						case XLookupChars:
							{
								const nana::char_t * charbuf;
#if defined(NANA_UNICODE)
								nana::detail::charset_conv charset("UTF-32", "UTF-8");
								const std::string& str = charset.charset(std::string(keybuf, keybuf + len));
								charbuf = reinterpret_cast<const nana::char_t*>(str.c_str()) + 1;
								len = str.size() / sizeof(wchar_t) - 1;
#else
								charbuf = keybuf;
#endif
								for(int i = 0; i < len; ++i)
								{
									ei.keyboard.key = charbuf[i];
									bedrock.get_key_state(ei.keyboard);
									ei.keyboard.ignore = false;

									ei.identifier = event_tag::key_char;
									ei.window = reinterpret_cast<nana::gui::window>(msgwnd);

									bedrock.evt_manager.answer(event_tag::key_char, reinterpret_cast<nana::gui::window>(msgwnd), ei, event_manager::event_kind::user);
									if(ei.keyboard.ignore == false && bedrock.wd_manager.available(msgwnd))
										bedrock.fire_event_for_drawer(event_tag::key_char, msgwnd, ei, &context);
								}
							}
							break;
						}
						bedrock.wd_manager.do_lazy_refresh(msgwnd, false);
						if(keybuf != fixbuf)
							delete [] keybuf;
					}
				}
				break;
			case KeyRelease:
				nana::detail::platform_spec::instance().write_keystate(xevent.xkey);
				if(context.platform.keychar != keyboard::alt) //Must NOT be an ALT
				{
					msgwnd = bedrock.focus();
					if(msgwnd)
					{
						ei.keyboard.key = static_cast<nana::char_t>(context.platform.keychar);
						bedrock.get_key_state(ei.keyboard);
						bedrock.raise_event(event_tag::key_up, msgwnd, ei, true);
					}
				}
				else
					bedrock.set_keyboard_shortkey(false);
				break;
			default:
				if((message == ClientMessage))
				{
					nana::detail::platform_spec & spec = nana::detail::platform_spec::instance();
					if(spec.atombase().wm_protocols == xevent.xclient.message_type)
					{
						if(spec.atombase().wm_delete_window == static_cast<Atom>(xevent.xclient.data.l[0]))
						{
							ei.unload.cancel = false;
							bedrock.raise_event(event_tag::unload, msgwnd, ei, true);
							if(false == ei.unload.cancel)
								native_interface::close_window(native_window);
						}
					}
				}
			}


			root_runtime = bedrock.wd_manager.root_runtime(native_window);
			if(root_runtime)
			{
				context.event_window = pre_event_window;
				root_runtime->condition.mouse_window = mouse_window;
				root_runtime->condition.mousemove_window = mousemove_window;
			}
			else
			{
				bedrock::thread_context *context = bedrock.get_thread_context();
				if(context) context->event_window = pre_event_window;
			}

			if(msgwnd)
			{
				unsigned tid = nana::system::this_thread_id();

				bedrock.wd_manager.remove_trash_handle(tid);
				bedrock.evt_manager.remove_trash_handle(tid);
			}
		}
	}

	void bedrock::pump_event(nana::gui::window modal_window)
	{
		thread_context * context = this->open_thread_context();
		if(0 == context->window_count)
		{
			//test if there is not a window
			//GetMessage may block if there is not a window
			this->remove_thread_context();
			return;
		}

		++(context->event_pump_ref_count);

		nana::detail::platform_spec::instance().msg_dispatch(modal_window ? reinterpret_cast<core_window_t*>(modal_window)->root : 0);

		if(0 == --(context->event_pump_ref_count))
		{
			if(0 == modal_window || 0 == context->window_count)
				this->remove_thread_context();
		}

	}//end bedrock::event_loop

	void make_eventinfo_for_mouse(nana::gui::eventinfo& ei, nana::gui::detail::bedrock::core_window_t* wnd, unsigned int msg, const event_mask& lparam)
	{
		ei.window = mycast(wnd);
		ei.mouse.x = lparam.pos.x - wnd->root_x;
		ei.mouse.y = lparam.pos.y - wnd->root_y;
		//ei.mouse.button = (msg - WM_LBUTTONDOWN)/3 + 1;
	}


	bool bedrock::fire_event_for_drawer(unsigned event_id, detail::bedrock::core_window_t* wd, const eventinfo& ei, thread_context* thrd)
	{
		if(bedrock_object.wd_manager.available(wd) == false) return false;
		if(thrd) thrd->event_window = wd;

		if(wd->other.upd_state == core_window_t::update_state::none)
			wd->other.upd_state = core_window_t::update_state::lazy;
		return bedrock_object.evt_manager.answer(event_id, reinterpret_cast<nana::gui::window>(wd), ei, event_manager::event_kind::trigger);
	}

	bool bedrock::fire_event(unsigned event_id, detail::bedrock::core_window_t* wd, const eventinfo& ei)
	{
		if(bedrock_object.wd_manager.available(wd) == false) return false;
		return bedrock_object.evt_manager.answer(event_id, reinterpret_cast<nana::gui::window>(wd), ei, event_manager::event_kind::user);
	}

	bool bedrock::raise_event(unsigned eid, core_window_t* wd, const eventinfo& ei, bool ask_update)
	{
		if(bedrock_object.wd_manager.available(wd) == false) return false;
		ei.identifier = eid;
		ei.window = reinterpret_cast<window>(wd);

		thread_context * thrd = bedrock_object.get_thread_context();
		core_window_t * prev_wd;
		if(thrd)
		{
			prev_wd = thrd->event_window;
			thrd->event_window = wd;
		}

		if(wd->other.upd_state == core_window_t::update_state::none)
			wd->other.upd_state = core_window_t::update_state::lazy;

		bedrock_object.evt_manager.answer(eid, reinterpret_cast<window>(wd), ei, event_manager::event_kind::trigger);
		bedrock_object.evt_manager.answer(eid, reinterpret_cast<window>(wd), ei, event_manager::event_kind::user);

		if(ask_update)
			bedrock_object.wd_manager.do_lazy_refresh(wd, false);
		else
			wd->other.upd_state = core_window_t::update_state::none;

		if(thrd) thrd->event_window = prev_wd;
		return true;
	}

	bool bedrock::raise_event_keep_ei(unsigned eid, core_window_t* wd, const eventinfo& ei, thread_context* thrd)
	{
		if(bedrock_object.wd_manager.available(wd) == false) return false;
		if(thrd) thrd->event_window = wd;

		if(wd->other.upd_state == core_window_t::update_state::none)
			wd->other.upd_state = core_window_t::update_state::lazy;

		bedrock_object.evt_manager.answer(eid, reinterpret_cast<window>(wd), ei, event_manager::event_kind::trigger);
		bedrock_object.evt_manager.answer(eid, reinterpret_cast<window>(wd), ei, event_manager::event_kind::user);
		return true;
	}

	void bedrock::event_expose(core_window_t * wd, bool exposed)
	{
		if(wd)
		{
			eventinfo ei;
			ei.exposed = exposed;
			wd->visible = exposed;
			if(raise_event(event_tag::expose, wd, ei, false))
			{
				if(false == exposed)
				{
					if(wd->other.category != category::root_tag::value)
					{
						wd = wd->parent;

						while(wd->other.category == category::lite_widget_tag::value)
							wd = wd->parent;
					}
					else if(wd->other.category == category::frame_tag::value)
						wd = wd_manager.find_window(wd->root, wd->root_x, wd->root_y);
				}

				wd_manager.update(wd, true, true);
			}
		}
	}
}//end namespace detail
}//end namespace gui
}//end namespace nana
