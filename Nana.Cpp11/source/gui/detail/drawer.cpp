/*
 *	A Drawer Implementation
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/drawer.cpp
 */

#include <nana/config.hpp>
#include GUI_BEDROCK_HPP
#include <nana/gui/detail/drawer.hpp>
#include <nana/gui/detail/dynamic_drawing_object.hpp>
#include <nana/gui/detail/effects_renderer.hpp>
#include <nana/gui/detail/basic_window.hpp>

#if defined(NANA_X11)
	#include <nana/detail/linux_X11/platform_spec.hpp>
#endif

#include <algorithm>

namespace nana
{
	typedef ::nana::detail::edge_nimbus_renderer<::nana::detail::bedrock::core_window_t> edge_nimbus_renderer_t;

	//class drawer_trigger
		drawer_trigger::~drawer_trigger(){}
		void drawer_trigger::attached(widget_reference, graph_reference){}
		void drawer_trigger::detached(){}	//none-const
		void drawer_trigger::typeface_changed(graph_reference){}
		void drawer_trigger::refresh(graph_reference){}

		void drawer_trigger::resizing(graph_reference, const arg_resizing&){}

		void drawer_trigger::resized(graph_reference graph, const arg_resized&)
		{
			this->refresh(graph);
			detail::bedrock::instance().thread_context_lazy_refresh();
		}

		void drawer_trigger::move(graph_reference, const arg_move&){}
		void drawer_trigger::click(graph_reference, const arg_mouse&){}
		void drawer_trigger::dbl_click(graph_reference, const arg_mouse&){}
		void drawer_trigger::mouse_enter(graph_reference, const arg_mouse&){}
		void drawer_trigger::mouse_move(graph_reference, const arg_mouse&){}
		void drawer_trigger::mouse_leave(graph_reference, const arg_mouse&){}
		void drawer_trigger::mouse_down(graph_reference, const arg_mouse&){}
		void drawer_trigger::mouse_up(graph_reference, const arg_mouse&){}
		void drawer_trigger::mouse_wheel(graph_reference, const arg_wheel&){}
		void drawer_trigger::mouse_dropfiles(graph_reference, const arg_dropfiles&){}
		void drawer_trigger::focus(graph_reference, const arg_focus&){}
		void drawer_trigger::key_press(graph_reference, const arg_keyboard&){}
		void drawer_trigger::key_char(graph_reference, const arg_keyboard&){}
		void drawer_trigger::key_release(graph_reference, const arg_keyboard&){}
		void drawer_trigger::shortkey(graph_reference, const arg_keyboard&){}

	//end class drawer_trigger

	namespace detail
	{
		typedef bedrock bedrock_type;

		//class drawer
		drawer::drawer()
			:	core_window_(nullptr), realizer_(nullptr), refreshing_(false)
		{
		}

		drawer::~drawer()
		{
			for(auto p : dynamic_drawing_objects_)
			{
				delete p;
			}
		}

		void drawer::bind(basic_window* cw)
		{
			core_window_ = cw;
		}

		void drawer::typeface_changed()
		{
			if(realizer_)
				realizer_->typeface_changed(graphics);
		}

		void drawer::click_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->click(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::dbl_click_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->dbl_click(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_enter_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_enter(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_move_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_move(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_leave_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_leave(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_down_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_down(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_up_arg(const arg_mouse& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_up(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_wheel_arg(const arg_wheel& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_wheel(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::mouse_dropfiles(const arg_dropfiles& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->mouse_dropfiles(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::resizing_arg(const arg_resizing& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->resizing(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::resized_arg(const arg_resized& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->resized(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::move_arg(const arg_move& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->move(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::focus_arg(const arg_focus& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->focus(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::key_press_arg(const arg_keyboard& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->key_press(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::key_char_arg(const arg_keyboard& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->key_char(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::key_release_arg(const arg_keyboard& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->key_release(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::shortkey_arg(const arg_keyboard& arg)
		{
			if (realizer_)
			{
				_m_bground_pre();
				realizer_->shortkey(graphics, arg);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
			}
		}

		void drawer::map(window wd)	//Copy the root buffer to screen
		{
			if(wd)
			{
				bedrock_type::core_window_t* iwd = reinterpret_cast<bedrock_type::core_window_t*>(wd);
				bedrock_type::core_window_t * caret_wd = iwd->root_widget->other.attribute.root->focus;

				bool owns_caret = (caret_wd && (caret_wd->together.caret) && (caret_wd->together.caret->visible()));
				
				//The caret in X11 is implemented by Nana, it is different from Windows'
				//the caret in X11 is asynchronous, it is hard to hide and show the caret
				//immediately, and therefore the caret always be flickering when the graphics
				//buffer is mapping to the window.
				if(owns_caret)
				{
#ifndef NANA_X11
					caret_wd->together.caret->visible(false);
#else
					owns_caret = nana::detail::platform_spec::instance().caret_update(iwd->root, *iwd->root_graph, false);
#endif
				}
				
				if(false == edge_nimbus_renderer_t::instance().render(iwd))
				{
					nana::rectangle vr;
					if(bedrock_type::window_manager_t::wndlayout_type::read_visual_rectangle(iwd, vr))
						iwd->root_graph->paste(iwd->root, vr, vr.x, vr.y);
				}
				
				if(owns_caret)
				{
#ifndef NANA_X11
					caret_wd->together.caret->visible(true);
#else
					nana::detail::platform_spec::instance().caret_update(iwd->root, *iwd->root_graph, true);
#endif
				}
			}
		}

		void drawer::refresh()
		{
			if(realizer_ && (refreshing_ == false))
			{
				refreshing_ = true;
				_m_bground_pre();
				realizer_->refresh(graphics);
				_m_draw_dynamic_drawing_object();
				_m_bground_end();
				graphics.flush();
				refreshing_ = false;
			}
		}

		drawer_trigger* drawer::realizer() const
		{
			return realizer_;
		}

		void drawer::attached(widget& wd, drawer_trigger& realizer)
		{
			realizer_ = &realizer;
			realizer.attached(wd, graphics);
		}

		drawer_trigger* drawer::detached()
		{
			if(realizer_)
			{
				auto rmp = realizer_;
				realizer_ = nullptr;
				rmp->detached();
				return rmp;
			}
			return nullptr;
		}

		void drawer::clear()
		{
			std::vector<dynamic_drawing::object*> then;
			for(auto p : dynamic_drawing_objects_)
			{
				if(p->diehard())
					then.push_back(p);
				else
					delete p;
			}

			then.swap(dynamic_drawing_objects_);
		}

		void* drawer::draw(std::function<void(paint::graphics&)> && f, bool diehard)
		{
			if(f)
			{
				auto p = new dynamic_drawing::user_draw_function(std::move(f), diehard);
				dynamic_drawing_objects_.push_back(p);
				return (diehard ? p : nullptr);
			}
			return nullptr;
		}

		void drawer::erase(void * p)
		{
			if(p)
			{
				auto i = std::find(dynamic_drawing_objects_.begin(), dynamic_drawing_objects_.end(), p);
				if(i != dynamic_drawing_objects_.end())
					dynamic_drawing_objects_.erase(i);
			}
		}

		void drawer::_m_bground_pre()
		{
			if(core_window_->effect.bground && core_window_->effect.bground_fade_rate < 0.01)
				core_window_->other.glass_buffer.paste(graphics, 0, 0);
		}

		void drawer::_m_bground_end()
		{
			if(core_window_->effect.bground && core_window_->effect.bground_fade_rate >= 0.01)
				core_window_->other.glass_buffer.blend(core_window_->other.glass_buffer.size(), graphics, nana::point(), core_window_->effect.bground_fade_rate);
		}

		void drawer::_m_draw_dynamic_drawing_object()
		{
			for(auto * dw : dynamic_drawing_objects_)
				dw->draw(graphics);
		}
	}//end namespace detail
}//end namespace nana
