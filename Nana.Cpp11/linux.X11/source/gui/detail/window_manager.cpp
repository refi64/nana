/*
 *	Window Manager Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/window_manager.cpp
 *
 */

#include <nana/gui/detail/window_manager.hpp>
#include <nana/gui/detail/bedrock.hpp>
#include <nana/gui/detail/native_window_interface.hpp>

namespace nana
{
namespace gui
{
namespace detail
{

	//class signal_manager

		//begin struct inner_invoker
			signal_manager::inner_invoker::~inner_invoker(){}
		//end struct inner_invoker

		signal_manager::~signal_manager()
		{
			manager_.clear();
		}

		void signal_manager::umake(identifier id)
		{
			if(id)
			{
				manager_.erase(id);
				end_ = manager_.end();
			}
		}

		void signal_manager::fireaway(signal_manager::identifier wd, int message, const signals& info)
		{
			auto i = manager_.find(wd);
			if((i != end_) && i->second)
				i->second->fireaway(message, info);
		}
	//end class signal_manager


	//class shortkey_container::
		bool shortkey_container::make(nana::gui::window wd, unsigned long key)
		{
			if(wd == nullptr) return false;
			if(key < 0x61) key += (0x61 - 0x41);

			for(auto & m : keybase_)
			{
				if(m.window == wd)
				{
					m.keys.push_back(key);
					return true;
				}
			}

			item_type m;
			m.window = wd;
			m.keys.push_back(key);
			keybase_.push_back(m);

			return true;
		}

		void shortkey_container::clear()
		{
			keybase_.clear();
		}

		void shortkey_container::umake(nana::gui::window wd)
		{
			if(wd == nullptr) return;
			auto i = std::find_if(keybase_.begin(), keybase_.end(), [wd](const item_type& m){
				return (m.window ==wd);});

			if(i != keybase_.end())
				keybase_.erase(i);
		}

		nana::gui::window shortkey_container::find(unsigned long key)
		{
			if(key < 0x61) key += (0x61 - 0x41);

			for(auto & m: keybase_)
			{
				for(auto n : m.keys)
				{
					if(key == n)
						return m.window;
				}
			}
			return nullptr;
		}
	//end class shortkey_container


	//class tray_event_manager
		void tray_event_manager::fire(native_window_type wd, unsigned identifier, const eventinfo& ei)
		{
			auto i = maptable_.find(wd);
			if(i == maptable_.end()) return;
			
			auto u = i->second.find(identifier);
			if(u == i->second.end()) return;
			
			const fvec_t & fvec = u->second;
			for(fvec_t::const_iterator j = fvec.begin(); j != fvec.end(); ++j)
				(*j)(ei);
		}

		bool tray_event_manager::make(native_window_type wd, unsigned identifier, const function_type & f)
		{
			if(wd)
			{
				maptable_[wd][identifier].push_back(f);
				return true;
			}
			return false;
		}

		void tray_event_manager::umake(native_window_type wd)
		{
			maptable_.erase(wd);
		}
	//end class tray_event_manager

	//class root_window_runtime
		//struct condition_tag
			root_window_runtime::condition_tag::condition_tag()
				: mouse_window(nullptr), mousemove_window(nullptr), tabstop_focus_changed(false)
			{}
		//end struct condition_tag
		root_window_runtime::root_window_runtime(core_window_t * wd, unsigned width, unsigned height)
			: window(wd), root_graph_object(width, height)
		{}
	//end class root_window_runtime

	//class window_manager
		window_manager::window_manager()
		{
			attr_.capture.window = nullptr;
			attr_.capture.ignore_children = true;

			menu_.window = nullptr;
			menu_.owner = nullptr;
			menu_.has_keyboard = false;
		}

		bool window_manager::is_queue(core_window_t* wd)
		{
			return (wd && wd->other.category == category::root_tag::value);
		}

		std::size_t window_manager::number_of_core_window() const
		{
			return this->handle_manager_.size();
		}

		std::recursive_mutex& window_manager::internal_lock() const
		{
			return mutex_;
		}

		void window_manager::all_handles(std::vector<core_window_t*> &v) const
		{
			handle_manager_.all(v);
		}

		void window_manager::detach_signal(core_window_t* wd)
		{
			signal_manager_.umake(wd);
		}

		void window_manager::signal_fire_caption(core_window_t* wd, const nana::char_t* str)
		{
			detail::signals sig;
			sig.info.caption = str;
			signal_manager_.fireaway(wd, detail::signals::caption, sig);
		}

		void window_manager::event_filter(core_window_t* wd, bool is_make, unsigned eventid)
		{
			switch(eventid)
			{
			case gui::events::mouse_drop::identifier:
				wd->flags.dropable = (is_make ? true : (bedrock::instance().evt_manager.the_number_of_handles(reinterpret_cast<nana::gui::window>(wd), eventid, false) != 0));
				break;
			}
		}

		void window_manager::default_icon(const nana::paint::image& img)
		{
			default_icon_ = img;
		}

		bool window_manager::available(core_window_t* wd)
		{
			return handle_manager_.available(wd);
		}

		bool window_manager::available(core_window_t * a, core_window_t* b)
		{
			return (handle_manager_.available(a) && handle_manager_.available(b));
		}

		bool window_manager::available(nana::gui::native_window_type wd)
		{
			if(wd)
			{
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				return (root_table_.find(wd) != 0);
			}
			return false;
		}

		window_manager::core_window_t* window_manager::create_root(core_window_t* owner, bool nested, int x, int y, unsigned width, unsigned height, const appearance& app)
		{
			nana::gui::native_window_type ownerWnd = nullptr;

			if(owner)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);

				if(handle_manager_.available(owner))
				{
					ownerWnd = (owner->other.category == category::frame_tag::value ?
										owner->other.attribute.frame->container : owner->root_widget->root);
					x += owner->root_x;
					y += owner->root_y;
				}
				else
					owner = nullptr;
			}

			native_interface::window_result result = native_interface::create_window(ownerWnd, nested, x, y, width, height, app);
			if(result.handle)
			{
				core_window_t* wd = new core_window_t(owner, (nana::gui::category::root_tag**)0);
				wd->flags.take_active = !app.no_activate;
				wd->title = native_interface::window_caption(result.handle);

				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);

				//create Root graphics Buffer and manage it
				root_table_type::value_type rt_window_data(wd, result.width, result.height);
				root_table_type::value_type* value = root_table_.insert(result.handle, rt_window_data);
				if(value)
				{
					value->root_graph_object = rt_window_data.root_graph_object;
					value->window = rt_window_data.window;
				}

				wd->bind_native_window(result.handle, result.width, result.height, result.extra_width, result.extra_height, value->root_graph_object);

				handle_manager_.insert(wd, wd->thread_id);

				if(owner && owner->other.category == category::frame_tag::value)
					insert_frame(owner, wd);

				bedrock::inc_window(wd->thread_id);
				this->icon(wd, default_icon_);
				return wd;
			}
			return nullptr;
		}

		window_manager::core_window_t* window_manager::create_frame(core_window_t* parent, int x, int y, unsigned width, unsigned height)
		{
			if(parent == nullptr) return nullptr;
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);

			if(handle_manager_.available(parent) == false)	return nullptr;

			core_window_t * wd = new core_window_t(parent, x, y, width, height, (nana::gui::category::frame_tag**)nullptr);
			wd->frame_window(native_interface::create_child_window(parent->root, wd->root_x, wd->root_y, width, height));
			handle_manager_.insert(wd, wd->thread_id);

			//Insert the frame_widget into its root frames container.
			wd->root_widget->other.attribute.root->frames.push_back(wd);
			return (wd);
		}

		bool window_manager::insert_frame(core_window_t* frame, native_window wd)
		{
			if(frame)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(frame->other.category == category::frame_tag::value)
					frame->other.attribute.frame->attach.push_back(wd);
				return true;
			}
			return false;
		}

		bool window_manager::insert_frame(core_window_t* frame, core_window_t* wd)
		{
			if(frame)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(frame->other.category == category::frame_tag::value)
				{
					if(handle_manager_.available(wd) && wd->other.category == category::root_tag::value && wd->root != frame->root)
					{
						frame->other.attribute.frame->attach.push_back(wd->root);
						return true;
					}
				}
			}
			return false;
		}

		window_manager::core_window_t* window_manager::create_widget(core_window_t* parent, int x, int y, unsigned width, unsigned height)
		{
			if(parent == nullptr)	return nullptr;
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(parent) == false)	return nullptr;
			core_window_t * wd = new core_window_t(parent, x, y, width, height, (nana::gui::category::widget_tag**)nullptr);
			handle_manager_.insert(wd, wd->thread_id);
			return wd;
		}

		window_manager::core_window_t* window_manager::create_lite_widget(core_window_t* parent, int x, int y, unsigned width, unsigned height)
		{
			if(parent == nullptr)	return nullptr;
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(parent) == false)	return nullptr;
			core_window_t * wd = new core_window_t(parent, x, y, width, height, (nana::gui::category::lite_widget_tag**)nullptr);
			handle_manager_.insert(wd, wd->thread_id);
			return wd;
		}

		void window_manager::close(core_window_t *wd)
		{
			if(wd == nullptr) return;
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd) == false)	return;

			if(wd->other.category == category::root_tag::value)
			{
				nana::gui::eventinfo ei;
				ei.unload.cancel = false;
				bedrock::raise_event(gui::detail::event_tag::unload, wd, ei, true);
				if(false == ei.unload.cancel)
				{
					//Before close the window, its owner window should be actived, otherwise other window will be
					//activated due to the owner window is not enabled.
					if(wd->flags.modal || (wd->owner == 0) || wd->owner->flags.take_active)
						native_interface::active_owner(wd->root);

					//Close should detach the drawer and send destroy signal to widget object.
					//Otherwise, when a widget object is been deleting in other thread by delete operator, the object will be destroyed
					//before the window_manager destroyes the window, and then, window_manager detaches the
					//non-existing drawer_trigger which is destroyed by destruction of widget. Crash!
					wd->drawer.detached();
					signal_manager_.fireaway(wd, signals::destroy, signals_);
					detach_signal(wd);

					native_interface::close_window(wd->root);
				}
			}
			else
				destroy(wd);
		}

		//destroy
		//@brief:	Delete the window handle
		void window_manager::destroy(core_window_t* wd)
		{
			if(wd == nullptr) return;
			core_window_t* parent = nullptr;
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd) == false)	return;

				parent = wd->parent;

				if(wd == attr_.capture.window)
					capture_window(wd, false);

				if(wd->other.category == category::root_tag::value)
				{
					root_table_type::value_type* object = root_runtime(wd->root);
					object->shortkeys.clear();
					wd->other.attribute.root->focus = nullptr;
				}
				else
					unregister_shortkey(wd);

				if(parent)
				{
					cont_type & cont = parent->children;
					cont_type::iterator i = std::find(cont.begin(), cont.end(), wd);
					if(i != cont.end())
						cont.erase(i);
				}

				this->_m_destroy(wd);
			}

			this->update(parent, false, false);
		}

		//destroy_handle
		//@brief:	Delete window handle, the handle type must be a root and a frame.
		void window_manager::destroy_handle(core_window_t* wd)
		{
			if(wd == nullptr) return;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd) == false)	return;

			if(wd->other.category == category::root_tag::value || wd->other.category != category::frame_tag::value)
			{
				root_table_.erase(wd->root);
				handle_manager_.remove(wd);
			}
		}

		void window_manager::icon(core_window_t* wd, const paint::image& img)
		{
			if(false == img.empty())
			{
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					if(wd->other.category == category::root_tag::value)
						native_interface::window_icon(wd->root, img);
				}
			}
		}

		//show
		//@brief: show or hide a window
		bool window_manager::show(core_window_t* wd, bool visible)
		{
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd))
			{
				if(visible != wd->visible)
				{
					nana::gui::native_window_type nv = nullptr;
					switch(wd->other.category)
					{
					case category::root_tag::value:
						nv = wd->root; break;
					case category::frame_tag::value:
						nv = wd->other.attribute.frame->container; break;
					}

					//Don't set the visible attr of a window if it is a root.
					//The visible attr of a root will be set in the expose event.
					if(nullptr == nv)
						bedrock::instance().event_expose(wd, visible);
					else
						native_interface::show_window(nv, visible, wd->flags.take_active);
				}
				return true;
			}
			return false;
		}

		window_manager::core_window_t* window_manager::find_window(nana::gui::native_window_type root, int x, int y)
		{
			if((false == attr_.capture.ignore_children) || (nullptr == attr_.capture.window) || (attr_.capture.window->root != root))
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				root_table_type::value_type* rrt = root_runtime(root);
				if(rrt && _m_effective(rrt->window, x, y))
					return _m_find(rrt->window, x, y);
			}
			return attr_.capture.window;
		}

		//move the wnd and its all children window, x and y is a relatively coordinate for wnd's parent window
		bool window_manager::move(core_window_t* wd, int x, int y)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					if(wd->other.category != category::root_tag::value)
					{
						//Move child widgets
						if(x != wd->rect.x || y != wd->rect.y)
						{
							int delta_x = x - wd->rect.x;
							int delta_y = y - wd->rect.y;
							wd->rect.x += delta_x;
							wd->rect.y += delta_y;
							this->_m_move_core(wd, delta_x, delta_y);
							return true;
						}
					}
					else
						native_interface::move_window(wd->root, x, y);
				}
			}
			return false;
		}

		bool window_manager::move(core_window_t* wd, int x, int y, unsigned width, unsigned height)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					bool moved = false;
					const bool size_changed = (width != wd->rect.width || height != wd->rect.height);
					if(wd->other.category != category::root_tag::value)
					{
						//Move child widgets
						if(x != wd->rect.x || y != wd->rect.y)
						{
							int delta_x = x - wd->rect.x;
							int delta_y = y - wd->rect.y;
							wd->rect.x += delta_x;
							wd->rect.y += delta_y;
							this->_m_move_core(wd, delta_x, delta_y);
							moved = true;
						}

						if(size_changed)
							size(wd, width, height, true, false);
					}
					else
					{
						if(size_changed)
						{
							wd->rect.width = width;
							wd->rect.height = height;
							wd->drawer.graphics.make(width, height);
							wd->root_graph->make(width, height);
							native_interface::move_window(wd->root, x, y, width, height);

							gui::eventinfo ei;
							ei.identifier = event_tag::size;
							ei.window = reinterpret_cast<window>(wd);
							ei.size.width = width;
							ei.size.height = height;
							bedrock::raise_event(event_tag::size, wd, ei, true);
						}
						else
							native_interface::move_window(wd->root, x, y);
					}

					return (moved || size_changed);
				}
			}
			return false;
		}

		//size
		//@brief: Size a window
		//@param: passive, if it is true, the function would not change the size if wd is a root_widget.
		//			e.g, when the size of window is changed by OS/user, the function should not resize the
		//			window again, otherwise, it causes an infinite loop, because when a root_widget is resized,
		//			window_manager will call the function.
		//
		bool window_manager::size(core_window_t* wd, unsigned width, unsigned height, bool passive, bool ask_update)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					if(wd->rect.width != width || wd->rect.height != height)
					{
						if(wd->max_track_size.width && wd->max_track_size.height)
						{
							if(width > wd->max_track_size.width)
								width = wd->max_track_size.width;
							if(height > wd->max_track_size.height)
								height = wd->max_track_size.height;
						}
						if(wd->min_track_size.width && wd->min_track_size.height)
						{
							if(width < wd->min_track_size.width)
								width = wd->min_track_size.width;
							if(height < wd->min_track_size.height)
								height = wd->min_track_size.height;
						}

						wd->rect.width = width;
						wd->rect.height = height;

						if(category::lite_widget_tag::value != wd->other.category)
						{
							wd->drawer.graphics.make(width, height);
							if(category::root_tag::value == wd->other.category)
							{
								wd->root_graph->make(width, height);
								if(false == passive)
									native_interface::window_size(wd->root, width + wd->extra_width, height + wd->extra_height);
							}
							else if(category::frame_tag::value == wd->other.category)
							{
								native_interface::window_size(wd->other.attribute.frame->container, width, height);
								for(auto natwd : wd->other.attribute.frame->attach)
									native_interface::window_size(natwd, width, height);
							}
						}
						gui::eventinfo ei;
						ei.identifier = event_tag::size;
						ei.window = reinterpret_cast<window>(wd);
						ei.size.width = width;
						ei.size.height = height;

						bedrock::raise_event(event_tag::size, wd, ei, ask_update);
						return true;
					}
				}
			}
			return false;
		}

		window_manager::core_window_t* window_manager::root(nana::gui::native_window_type wd) const
		{
			static std::pair<nana::gui::native_window_type, core_window_t*> cache;
			if(cache.first == wd) return cache.second;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);

			auto rrt = root_runtime(wd);
			if(rrt)
			{
				cache.first = wd;
				cache.second = rrt->window;
				return cache.second;
			}
			return nullptr;
		}

		nana::gui::event_handle window_manager::make_drawer_event(int event_id, core_window_t* wd, core_window_t* listener)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);

				if(handle_manager_.available(wd))
				{
					if(handle_manager_.available(listener))
						return listener->drawer.make_event(event_id, reinterpret_cast<nana::gui::window>(wd), reinterpret_cast<nana::gui::window>(listener));
					else
						return wd->drawer.make_event(event_id, reinterpret_cast<nana::gui::window>(wd));
				}
			}
			return 0;
		}

		//Copy the root buffer that wnd specified into DeviceContext
		void window_manager::map(core_window_t* wd)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
#if defined(NANA_LINUX)
					nana::rectangle vr;
					if(wndlayout_type::read_visual_rectangle(wd, vr))
						wd->drawer.map(reinterpret_cast<nana::gui::window>(wd), vr);	//Copy the root buffer that wd specified into DeviceContext
#elif defined(NANA_WINDOWS)
					if(nana::system::this_thread_id() == wd->thread_id)
					{
						nana::rectangle vr;
						if(wndlayout_type::read_visual_rectangle(wd, vr))
							wd->drawer.map(reinterpret_cast<nana::gui::window>(wd), vr);	//Copy the root buffer that wd specified into DeviceContext
					}
					else
						bedrock::instance().map_thread_root_buffer(wd);
#endif
				}
			}
		}

		bool window_manager::belong_to_lazy(core_window_t * wd) const
		{
			for(; wd; wd = wd->parent)
			{
				if(wd->other.upd_state == core_window_t::update_state::refresh)
					return true;
			}
			return false;
		}

		//update
		//@brief:	update is used for displaying the screen-off buffer.
		//			Because of a good efficiency, if it is called in an event procedure and the event procedure window is the
		//			same as update's, update would not map the screen-off buffer and just set the window for lazy refresh
		bool window_manager::update(core_window_t* wd, bool redraw, bool force)
		{
			if(wd == nullptr) return false;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd) == false) return false;

			if(wd->visible)
			{
				for(core_window_t* pnt = wd->parent ; pnt; pnt = pnt->parent)
				{
					if(pnt->visible == false)
					{
						if(redraw) wd->drawer.refresh();
						return true;
					}
				}

				if(force || (false == belong_to_lazy(wd)))
				{
					wndlayout_type::paint(wd, redraw);
					this->map(wd);
				}
				else
				{
					if(redraw)
						wndlayout_type::paint(wd, true);
					if(wd->other.upd_state == core_window_t::update_state::lazy)
						wd->other.upd_state = core_window_t::update_state::refresh;
				}
			}
			return true;
		}

		void window_manager::refresh(core_window_t* wd)
		{
			if(wd == nullptr)	return;
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);

			//It's not worthy to redraw if visible is false
			if(handle_manager_.available(wd) == false)
				return;

			if(wd->visible)
			{
				core_window_t* parent = wd->parent;
				while(parent)
				{
					if(parent->visible == false)	break;
					parent = parent->parent;
				}

				if(parent)	//only refreshing if it has an invisible parent
				{
					wd->flags.refreshing = true;
					wd->drawer.refresh();
					wd->flags.refreshing = false;
				}
				else
					wndlayout_type::paint(wd, false);
			}
		}

		//do_lazy_refresh
		//@brief: defined a behavior of flush the screen
		//@return: it returns true if the wnd is available
		bool window_manager::do_lazy_refresh(core_window_t* wd, bool force_copy_to_screen)
		{
			if(wd == nullptr)	return false;
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);

			//It's not worthy to redraw if visible is false
			if(handle_manager_.available(wd) == false)
				return false;

			if(wd->visible)
			{
				core_window_t* parent = wd->parent;
				while(parent)
				{
					if(parent->visible == false)	break;
					parent = parent->parent;
				}

				if(parent)	//only refreshing if it has an invisible parent
				{
					wd->flags.refreshing = true;
					wd->drawer.refresh();
					wd->flags.refreshing = false;
				}
				else
				{
					if((wd->other.upd_state == core_window_t::update_state::refresh) || force_copy_to_screen)
					{
						wndlayout_type::paint(wd, false);
						this->map(wd);
					}
				}
			}
			wd->other.upd_state = core_window_t::update_state::none;
			return true;
		}

		//get_graphics
		//@brief: Get a copy of the graphics object of a window.
		//	the copy of the graphics object has a same buf handle with the graphics object's, they are count-refered
		//	here returns a reference that because the framework does not guarantee the wnd's
		//	graphics object available after a get_graphics call.
		bool window_manager::get_graphics(core_window_t* wd, nana::paint::graphics& result)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					result.make(wd->drawer.graphics.width(), wd->drawer.graphics.height());
					result.bitblt(0, 0, wd->drawer.graphics);
					wndlayout_type::paste_children_to_graphics(wd, result);
					return true;
				}
			}
			return false;
		}

		bool window_manager::get_visual_rectangle(core_window_t* wd, nana::rectangle& r)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
					return wndlayout_type::read_visual_rectangle(wd, r);
			}
			return false;
		}

		bool window_manager::tray_make_event(native_window_type wd, unsigned identifier, const event_fn_t& f)
		{
			if(native_interface::is_window(wd))
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				return tray_event_manager_.make(wd, identifier, f);
			}
			return false;
		}

		void window_manager::tray_umake_event(native_window_type wd)
		{
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			tray_event_manager_.umake(wd);
		}

		void window_manager::tray_fire(native_window_type wd, unsigned identifier, const eventinfo& ei)
		{
			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			tray_event_manager_.fire(wd, identifier, ei);
		}

		//set_focus
		//@brief: set a keyboard focus to a window. this may fire a focus event.
		window_manager::core_window_t* window_manager::set_focus(core_window_t* wd)
		{
			if(wd == 0) return 0;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);

			core_window_t * prev_focus = 0;

			if(handle_manager_.available(wd))
			{
				core_window_t* root_wd = wd->root_widget;
				prev_focus = root_wd->other.attribute.root->focus;

				eventinfo ei;
				if(wd != prev_focus)
				{
					//kill the previous window focus
					ei.focus.getting = false;
					root_wd->other.attribute.root->focus = wd;

					if(handle_manager_.available(prev_focus))
					{
						if(prev_focus->together.caret)
							prev_focus->together.caret->set_active(false);

						ei.focus.receiver = wd->root;
						bedrock::raise_event(event_tag::focus, prev_focus, ei, true);
					}
				}
				else if(wd->root == native_interface::get_focus_window())
					wd = nullptr; //no new focus_window

				if(wd)
				{
					if(wd->together.caret)
						wd->together.caret->set_active(true);

					ei.focus.getting = true;
					ei.focus.receiver = wd->root;

					bedrock::raise_event(event_tag::focus, wd, ei, true);
					native_interface::set_focus(root_wd->root);

					bedrock::instance().set_menubar_taken(wd);
				}
			}
			return prev_focus;
		}

		window_manager::core_window_t* window_manager::capture_redirect(core_window_t* msgwnd)
		{
			if(attr_.capture.window && (attr_.capture.ignore_children == false) && (attr_.capture.window != msgwnd))
			{
				//Tests if the msgwnd is a child of captured window,
				//and returns the msgwnd if it is.
				for(core_window_t * child = msgwnd; child; child = child->parent)
				{
					if(child->parent == attr_.capture.window)
						return msgwnd;
				}
			}
			return attr_.capture.window;
		}

		void window_manager::capture_ignore_children(bool ignore)
		{
			attr_.capture.ignore_children = ignore;
		}

		bool window_manager::capture_window_entered(int root_x, int root_y, bool& prev)
		{
			if(attr_.capture.window)
			{
				bool inside = _m_effective(attr_.capture.window, root_x, root_y);
				if(inside != attr_.capture.inside)
				{
					prev = attr_.capture.inside;
					attr_.capture.inside = inside;
					return true;
				}
			}
			return false;
		}

		window_manager::core_window_t * window_manager::capture_window() const
		{
			return attr_.capture.window;
		}

		//capture_window
		//@brief:	set a window that always captures the mouse event if it is not in the range of window
		//@return:	this function dose return the previous captured window. If the wnd set captured twice,
		//			the return value is NULL
		window_manager::core_window_t* window_manager::capture_window(core_window_t* wd, bool value)
		{
			nana::point pos = native_interface::cursor_position();
			if(value)
			{
				if(wd != attr_.capture.window)
				{
					//Thread-Safe Required!
					std::lock_guard<decltype(mutex_)> lock(mutex_);

					if(handle_manager_.available(wd))
					{
						wd->flags.capture = true;
						native_interface::capture_window(wd->root, value);
						core_window_t* prev = attr_.capture.window;
						if(prev && prev != wd)
							attr_.capture.history.push_back(std::make_pair(prev, attr_.capture.ignore_children));

						attr_.capture.window = wd;
						attr_.capture.ignore_children = true;
						native_interface::calc_window_point(wd->root, pos);
						attr_.capture.inside = _m_effective(wd, pos.x, pos.y);
						return prev;
					}
				}

				return attr_.capture.window;
			}
			else if(wd == attr_.capture.window)
			{
				if(attr_.capture.history.size())
				{
					std::pair<core_window_t*, bool> & last = attr_.capture.history[attr_.capture.history.size() - 1];
					attr_.capture.history.erase(attr_.capture.history.begin() + (attr_.capture.history.size() - 1));

					if(handle_manager_.available(last.first))
					{
						attr_.capture.window = last.first;
						attr_.capture.ignore_children = last.second;
						native_interface::capture_window(last.first->root, true);
						native_interface::calc_window_point(last.first->root, pos);
						attr_.capture.inside = _m_effective(last.first, pos.x, pos.y);
					}
					else
						attr_.capture.window = 0;
				}
				else
					attr_.capture.window = 0;

				if(wd && (0 == attr_.capture.window))
					native_interface::capture_window(wd->root, false);
			}
			else
			{
				auto i = std::find_if(attr_.capture.history.begin(), attr_.capture.history.end(),
					[wd](std::pair<core_window_t*, bool> & x){ return (x.first == wd);});

				if(i != attr_.capture.history.end())
					attr_.capture.history.erase(i);

				wd = attr_.capture.window;
			}
			return wd;
		}

		//tabstop
		//@brief: when users press a TAB, the focus should move to the next widget.
		//	this method insert a window which catchs an user TAB into a TAB window container
		//	the TAB window container is held by a wnd's root widget. Not every widget has a TAB window container,
		//	the container is created while a first Tab Window is setting
		void window_manager::tabstop(core_window_t* wd)
		{
			if(wd == 0) return;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd) == false)	return;

			if(nana::gui::detail::tab_type::none == wd->flags.tab)
			{
				wd->root_widget->other.attribute.root->tabstop.push_back(wd);
				wd->flags.tab |= nana::gui::detail::tab_type::tabstop;
			}
		}

		window_manager::core_window_t* window_manager::tabstop_prev(core_window_t* wd) const
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					typedef core_window_t::tabstop_container_type tabstop_cont_t;

					tabstop_cont_t& cont = wd->root_widget->other.attribute.root->tabstop;
					if(cont.size())
					{
						auto i = std::find(cont.begin(), cont.end(), wd);
						if(i == cont.begin())
						{
							if(cont.size() > 1)
								return cont[cont.size() - 1];
						}
						else if(i != cont.end())
							return *(--i);
					}
				}
			}
			return nullptr;
		}

		window_manager::core_window_t* window_manager::tabstop_next(core_window_t* wd) const
		{
			if(wd == nullptr) return nullptr;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd) == false)	return nullptr;

			core_window_t * root_wd = wd->root_widget;
			if(nana::gui::detail::tab_type::none == wd->flags.tab)
			{
				if(root_wd->other.attribute.root->tabstop.size())
					return (root_wd->other.attribute.root->tabstop[0]);
			}
			else if(nana::gui::detail::tab_type::tabstop & wd->flags.tab)
			{
				typedef core_window_t::tabstop_container_type tabstop_cont_t;

				tabstop_cont_t & container = root_wd->other.attribute.root->tabstop;
				if(container.size())
				{
					auto end = container.end();
					auto i = std::find(container.begin(), end, wd);
					if(i != end)
					{
						++i;
						core_window_t* ts = (i != end? (*i) : (*(container.begin())));
						return (ts != wd ? ts : 0);
					}
					else
						return *(container.begin());
				}
			}
			return nullptr;
		}

		void window_manager::remove_trash_handle(unsigned tid)
		{
			handle_manager_.delete_trash(tid);
		}

		bool window_manager::glass_window(core_window_t* wd, bool isglass)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
					return wndlayout_type::glass_window(wd, isglass);
			}
			return false;
		}

		void window_manager::make_glass_background(core_window_t * wd)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
					wndlayout_type::make_glass(wd, true, true);
			}
		}

		bool window_manager::calc_window_point(core_window_t* wd, nana::point& pos)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					if(native_interface::calc_window_point(wd->root, pos))
					{
						pos.x -= wd->root_x;
						pos.y -= wd->root_y;
						return true;
					}
				}
			}
			return false;
		}

		window_manager::root_table_type::value_type* window_manager::root_runtime(nana::gui::native_window_type root) const
		{
			return root_table_.find(root);
		}

		bool window_manager::register_shortkey(core_window_t* wd, unsigned long key)
		{
			if(wd)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				if(handle_manager_.available(wd))
				{
					auto object = root_runtime(wd->root);
					if(object)
						return object->shortkeys.make(reinterpret_cast<nana::gui::window>(wd), key);
				}
			}
			return false;
		}

		void window_manager::unregister_shortkey(core_window_t* wd)
		{
			if(wd == nullptr) return;

			//Thread-Safe Required!
			std::lock_guard<decltype(mutex_)> lock(mutex_);
			if(handle_manager_.available(wd) == false) return;

			auto object = root_runtime(wd->root);
			if(object) object->shortkeys.umake(reinterpret_cast<nana::gui::window>(wd));
		}

		window_manager::core_window_t* window_manager::find_shortkey(nana::gui::native_window_type native_window, unsigned long key)
		{
			if(native_window)
			{
				//Thread-Safe Required!
				std::lock_guard<decltype(mutex_)> lock(mutex_);
				auto object = root_runtime(native_window);
				if(object)
					return reinterpret_cast<core_window_t*>(object->shortkeys.find(key));
			}
			return nullptr;
		}

		void window_manager::_m_destroy(core_window_t* wd)
		{
			if(wd->flags.destroying) return;

			bedrock & bedrock_instance = bedrock::instance();
			auto context = bedrock_instance.get_thread_context();
			if(context && context->event_window == wd)
				context->event_window = 0;

			wd->flags.destroying = true;

			if(wd->together.caret)
			{
				//The deletion of caret wants to know whether the window is destroyed under SOME platform. Such as X11
				delete wd->together.caret;
				wd->together.caret = 0;
			}
			//Delete the children widgets.
			std::for_each(wd->children.rbegin(), wd->children.rend(), [this](core_window_t * child){ this->_m_destroy(child);});
			wd->children.clear();

			nana::gui::eventinfo ei;
			ei.identifier = event_tag::destroy;
			ei.window = reinterpret_cast<nana::gui::window>(wd);
			bedrock::raise_event(event_tag::destroy, wd, ei, true);

			core_window_t * root_wd = wd->root_widget;
			if(root_wd->other.attribute.root->focus == wd)
				root_wd->other.attribute.root->focus = 0;

			if(root_wd->other.attribute.root->menubar == wd)
				root_wd->other.attribute.root->menubar = 0;

			if(wd->flags.glass)
				wndlayout_type::glass_window(wd, false);

			//test if wd is a TABSTOP window
			if(wd->flags.tab & nana::gui::detail::tab_type::tabstop)
			{
				auto & tabstop = root_wd->other.attribute.root->tabstop;
				auto i = std::find(tabstop.begin(), tabstop.end(), wd);
				if(i != tabstop.end())
					tabstop.erase(i);
			}

			if(wd->effect.edge_nimbus)
			{
				auto & container = root_wd->other.attribute.root->effects_edge_nimbus;
				for(auto i = container.begin(); i != container.end(); ++i)
				{
					if(i->window == wd)
					{
						container.erase(i);
						break;
					}
				}
			}

			bedrock_instance.evt_manager.umake(reinterpret_cast<nana::gui::window>(wd));
			wd->drawer.detached();
			signal_manager_.fireaway(wd, signals::destroy, signals_);
			detach_signal(wd);

			if(wd->parent && (wd->parent->children.size() > 1))
			{
				for(auto i = wd->parent->children.begin(), end = wd->parent->children.end();i != end; ++i)
				{
					if(((*i)->index) > (wd->index))
					{
						for(; i != end; ++i)
							--((*i)->index);
						break;
					}
				}
			}

			if(wd->other.category == category::frame_tag::value)
			{
				//remove the frame handle from the WM frames manager.
				{
					std::vector<core_window_t*> & frames = root_wd->other.attribute.root->frames;
					auto i = std::find(frames.begin(), frames.end(), wd);
					if(i != frames.end())
						frames.erase(i);
				}

				//The frame widget does not have an owner, and close their element windows without activating owner.
				//close the frame container window, it's a native window.
				for(auto i : wd->other.attribute.frame->attach)
					native_interface::close_window(i);

				native_interface::close_window(wd->other.attribute.frame->container);
			}

			if(wd->other.category != category::root_tag::value)	//Not a root window
				handle_manager_.remove(wd);
		}

		void window_manager::_m_move_core(core_window_t* wd, int delta_x, int delta_y)
		{
			if(wd->other.category != category::root_tag::value)	//A root widget always starts at (0, 0) and its childs are not to be changed
			{
				wd->root_x += delta_x;
				wd->root_y += delta_y;

				if(wd->other.category == category::frame_tag::value)
					native_interface::move_window(wd->other.attribute.frame->container, wd->root_x, wd->root_y);

				std::for_each(wd->children.begin(), wd->children.end(), [this, delta_x, delta_y](core_window_t * child){ this->_m_move_core(child, delta_x, delta_y);});
			}
		}

		//_m_find
		//@brief: find a window on root window through a given root coordinate.
		//		the given root coordinate must be in the rectangle of wnd.
		window_manager::core_window_t* window_manager::_m_find(core_window_t* wd, int x, int y)
		{
			if(wd->visible == 0) return nullptr;

			for(auto i = wd->children.rbegin(); i != wd->children.rend(); ++i)
			{
				core_window_t* child = *i;
				if((child->other.category != category::root_tag::value) && _m_effective(child, x, y))
				{
					child = _m_find(child, x, y);
					if(child)
						return child;
				}
			}
			return wd;
		}

		//_m_effective, test if the window is a handle of window that specified by (root_x, root_y)
		bool window_manager::_m_effective(core_window_t* wd, int root_x, int root_y)
		{
			if(wd == nullptr || wd->visible == 0)	return false;
			return nana::gui::is_hit_the_rectangle(nana::rectangle(wd->root_x, wd->root_y, wd->rect.width, wd->rect.height), root_x, root_y);
		}
	//end class window_manager
}//end namespace detail
}//end namespace gui
}//end namespace nana
