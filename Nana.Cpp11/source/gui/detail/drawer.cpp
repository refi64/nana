/*
 *	A Drawer Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://nanapro.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/drawer.cpp
 */

#include <nana/config.hpp>
#include GUI_BEDROCK_HPP
#include <nana/gui/detail/drawer.hpp>
#include <nana/gui/detail/dynamic_drawing_object.hpp>
#include <nana/gui/detail/effects_renderer.hpp>

namespace nana
{
namespace gui
{
	typedef detail::edge_nimbus_renderer<detail::bedrock::core_window_t> edge_nimbus_renderer_t;

	//class drawer_trigger
		drawer_trigger::~drawer_trigger(){}
		void drawer_trigger::bind_window(widget_reference){}
		void drawer_trigger::attached(graph_reference){}	//none-const
		void drawer_trigger::detached(){}	//none-const
		void drawer_trigger::notify_background_change(graph_reference){}
		void drawer_trigger::typeface_changed(graph_reference){}
		void drawer_trigger::refresh(graph_reference){}

		void drawer_trigger::resize(graph_reference graph, const eventinfo&)
		{
			refresh(graph);
			detail::bedrock::instance().thread_context_lazy_refresh();
		}

		void drawer_trigger::click(graph_reference, const eventinfo&){}
		void drawer_trigger::dbl_click(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_enter(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_move(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_leave(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_down(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_up(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_wheel(graph_reference, const eventinfo&){}
		void drawer_trigger::mouse_drop(graph_reference, const eventinfo&){}
		void drawer_trigger::focus(graph_reference, const eventinfo&){}
		void drawer_trigger::key_down(graph_reference, const eventinfo&){}
		void drawer_trigger::key_char(graph_reference, const eventinfo&){}
		void drawer_trigger::key_up(graph_reference, const eventinfo&){}
		void drawer_trigger::shortkey(graph_reference, const eventinfo&){}

	//end class drawer_trigger

	namespace detail
	{
		typedef bedrock bedrock_type;

			class drawer_binder
			{
			public:
				drawer_binder(drawer& object, void (drawer::*routine)(const eventinfo&))
					:this_(object), routine_(routine)
				{}

				void operator()(const eventinfo& ei)
				{
					(this_.*routine_)(ei);
				}
			private:
				drawer& this_;
				void (drawer::* routine_)(const eventinfo&);
			};

		//class drawer
		drawer::drawer():realizer_(0), refreshing_(false)
		{
		}

		drawer::~drawer()
		{
			clear();
		}

			// the event is fired by window_layout
		void drawer::notify_background_change()
		{
			if(realizer_)
			{
				realizer_->notify_background_change(graphics);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::typeface_changed()
		{
			if(realizer_)
				realizer_->typeface_changed(graphics);
		}

		void drawer::click(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->click(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::dbl_click(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->dbl_click(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_enter(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_enter(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_move(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_move(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_leave(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_leave(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_down(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_down(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_up(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_up(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_wheel(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_wheel(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::mouse_drop(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->mouse_drop(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::resize(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->resize(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::focus(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->focus(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::key_down(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->key_down(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::key_char(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->key_char(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::key_up(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->key_up(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::shortkey(const eventinfo& ei)
		{
			if(realizer_)
			{
				realizer_->shortkey(graphics, ei);
				_m_draw_dynamic_drawing_object();
			}
		}

		void drawer::map(window wd, const nana::rectangle& vr)	//Copy the root buffer to screen
		{
			if(wd)
			{
				bedrock_type::core_window_t* iwd = reinterpret_cast<bedrock_type::core_window_t*>(wd);
				const bool caret = (iwd->together.caret && iwd->together.caret->visible());
				if(caret) iwd->together.caret->visible(false);

				if(false == edge_nimbus_renderer_t::instance().render(iwd))
					iwd->root_graph->paste(iwd->root, vr, vr.x, vr.y);

				if(caret) iwd->together.caret->visible(true);
			}
		}

		void drawer::refresh()
		{
			if(realizer_ && (refreshing_ == false))
			{
				refreshing_ = true;
				realizer_->refresh(graphics);
				_m_draw_dynamic_drawing_object();
				graphics.flush();
				refreshing_ = false;
			}
		}

		drawer_trigger* drawer::realizer() const
		{
			return realizer_;
		}

		void drawer::attached(drawer_trigger& realizer)
		{
			realizer_ = &realizer;
			realizer.attached(graphics);
		}

		drawer_trigger* drawer::detached()
		{
			if(realizer_)
			{
				drawer_trigger * old = realizer_;
				realizer_ = 0;
				old->detached();
				return old;
			}
			return 0;
		}

		void drawer::clear()
		{
			for(auto p : dynamic_drawing_objects_)
				delete p;

			std::vector<dynamic_drawing::object*>().swap(dynamic_drawing_objects_);
		}

		void drawer::draw(std::function<void(paint::graphics&)> && f)
		{
			if(f)
				dynamic_drawing_objects_.push_back(new dynamic_drawing::user_draw_function(std::move(f)));
		}

		void drawer::string(int x, int y, unsigned color, const nana::char_t* text)
		{
			if(text)
			{
				dynamic_drawing_objects_.push_back(new dynamic_drawing::string(x, y, color, text));
			}
		}

		void drawer::line(int x, int y, int x2, int y2, unsigned color)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::line(x, y, x2, y2, color));
		}

		void drawer::rectangle(int x, int y, unsigned width, unsigned height, unsigned color, bool issolid)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::rectangle(x, y, width, height, color, issolid));
		}

		void drawer::shadow_rectangle(int x, int y, unsigned width, unsigned height, nana::color_t beg, nana::color_t end, bool vertical)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::shadow_rectangle(x, y, width, height, beg, end, vertical));
		}

		void drawer::bitblt(int x, int y, unsigned width, unsigned height, const paint::graphics& graph, int srcx, int srcy)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::bitblt(x, y, width, height, graph, srcx, srcy));
		}

		void drawer::bitblt(int x, int y, unsigned width, unsigned height, const paint::image& img, int srcx, int srcy)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::bitblt_image(x, y, width, height, img, srcx, srcy));
		}

		void drawer::stretch(const nana::rectangle & r_dst, const paint::graphics& graph, const nana::rectangle& r_src)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::stretch(r_dst, graph, r_src));
		}

		void drawer::stretch(const nana::rectangle & r_dst, const paint::image& img, const nana::rectangle& r_src)
		{
			dynamic_drawing_objects_.push_back(new dynamic_drawing::stretch(r_dst, img, r_src));
		}

		event_handle drawer::make_event(int evtid, window trigger)
		{
			bedrock_type & bedrock = bedrock_type::instance();
			void (drawer::*answer)(const eventinfo&) = nullptr;
			switch(evtid)
			{
			case event_tag::click:
				answer = &drawer::click;	break;
			case event_tag::dbl_click:
				answer = &drawer::dbl_click;	break;
			case event_tag::mouse_enter:
				answer = &drawer::mouse_enter;	break;
			case event_tag::mouse_leave:
				answer = &drawer::mouse_leave;	break;
			case event_tag::mouse_down:
				answer = &drawer::mouse_down;	break;
			case event_tag::mouse_up:
				answer = &drawer::mouse_up;	break;
			case event_tag::mouse_move:
				answer = &drawer::mouse_move;	break;
			case event_tag::mouse_wheel:
				answer = &drawer::mouse_wheel;	break;
			case event_tag::mouse_drop:
				answer = &drawer::mouse_drop;	break;
			case event_tag::size:
				answer = &drawer::resize;	break;
			case event_tag::focus:
				answer = &drawer::focus;	break;
			case event_tag::key_down:
				answer = &drawer::key_down;	break;
			case event_tag::key_char:
				answer = &drawer::key_char;	break;
			case event_tag::key_up:
				answer = &drawer::key_up;	break;
			case event_tag::shortkey:
				answer = &drawer::shortkey;	break;
			}

			if(answer && (0 == bedrock.evt_manager.the_number_of_handles(trigger, evtid, true)))
				return bedrock.evt_manager.make_for_drawer(evtid, trigger, bedrock.category(reinterpret_cast<bedrock::core_window_t*>(trigger)), drawer_binder(*this, answer));

			return nullptr;
		}

		void drawer::_m_draw_dynamic_drawing_object()
		{
			for(auto * dw : dynamic_drawing_objects_)
				dw->draw(graphics);
		}
	}//end namespace detail
}//end namespace gui
}//end namespace nana
