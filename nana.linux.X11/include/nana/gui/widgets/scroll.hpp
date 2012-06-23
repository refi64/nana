/*
 *	A Scroll Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/scroll.hpp
 */
#ifndef NANA_GUI_WIDGET_SCROLL_HPP
#define NANA_GUI_WIDGET_SCROLL_HPP

#include "widget.hpp"
#include <nana/paint/gadget.hpp>

namespace nana{ namespace gui{

	namespace drawerbase
	{
		namespace scroll
		{
			struct metrics_type
			{
				typedef size_t size_type;

				size_type peak;
				size_type range;
				size_type step;
				size_type value;

				int what;
				size_type	scroll_length;
				int			scroll_pos;
				int			trace;		//trace = 1, the mouse is pressed on the scrollbutton, 2: the mouse is pressed on scroll background
				int			mouse_pos;

				metrics_type();
			};

			class drawer
			{
			public:
				struct states
				{
					enum{none, highlight, actived, selected};
				};

				struct buttons
				{
					enum{none, scroll, first, second};
				};

				typedef nana::paint::graphics& graph_reference;
				const static unsigned fixedsize = 16;

				drawer(metrics_type& m);
				void set_vertical(bool v);
				int what(graph_reference graph, int x, int y);
			public:
				void draw(graph_reference graph, int what, bool moused, int delta_pos = 0);
			private:
				bool _m_check() const;
				bool _m_auto_scroll(graph_reference graph);
				void _m_background(graph_reference graph, bool normal);
				void _m_button_frame(graph_reference graph, int x, int y, unsigned width, unsigned height, int state);
				void _m_adjust_scroll(graph_reference graph, int delta_pos);
				void _m_draw_scroll(graph_reference graph, int state, int delta_pos);
				void _m_draw_button(graph_reference graph, int x, int y, unsigned width, unsigned height, int what, int state);
			private:
				metrics_type &metrics_;
				bool	vertical_;
			};



			template<bool Vertical>
			class trigger
				: public nana::gui::drawer_trigger
			{
			public:
				typedef metrics_type::size_type size_type;

				trigger()
					: graph_(0), drawer_(metrics_)
				{
					drawer_.set_vertical(Vertical);
				}

				const metrics_type& metrics() const
				{
					return metrics_;
				}

				void peak(size_type s)
				{
					if(graph_ && (metrics_.peak != s))
					{
						metrics_.peak = s;
						API::refresh_window(widget_->handle());
					}
				}

				void value(size_type s)
				{
					if(graph_ && (metrics_.value != s))
					{
						metrics_.value = s;
						API::refresh_window(widget_->handle());
					}
				}

				void range(size_type s)
				{
					if(graph_ && (metrics_.range != s))
					{
						metrics_.range = s;
						API::refresh_window(widget_->handle());
					}
				}

				void step(size_type s)
				{
					metrics_.step = s;
				}

				size_type make_step(bool forward)
				{
					if(graph_)
					{
						bool changed = false;
						if(forward)
						{
							size_type maxv = metrics_.peak - metrics_.range;

							if(metrics_.peak > metrics_.range && metrics_.value < maxv)
							{
								if(maxv - metrics_.value >= metrics_.step)
									metrics_.value += metrics_.step;
								else
									metrics_.value = maxv;
								changed = true;
							}
						}
						else if(metrics_.value)
						{
							if(metrics_.value > metrics_.step)
								metrics_.value -= metrics_.step;
							else
								metrics_.value = 0;

							changed = true;
						}
					}

					return metrics_.value;
				}
			private:
				void bind_window(widget_reference widget)
				{
					widget_ = &widget;
					widget.caption(STR("Nana Scroll"));

					using namespace API::dev;
					window wd = widget.handle();
					make_drawer_event<events::mouse_enter>(wd);
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_up>(wd);
					make_drawer_event<events::mouse_leave>(wd);
					make_drawer_event<events::mouse_wheel>(wd);
					make_drawer_event<events::size>(wd);
				}

				void attached(graph_reference graph)
				{
					graph_ = &graph;
				}

				void detached()
				{
					API::dev::umake_drawer_event(widget_->handle());
					graph_ = 0;
				}

				void refresh(graph_reference graph)
				{
					drawer_.draw(graph, metrics_.what, false, 0);
				}

				void resize(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					drawer_.draw(graph, metrics_.what, false, 0);
					API::lazy_refresh();
				}

				void mouse_enter(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					metrics_.what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
					drawer_.draw(graph, metrics_.what, false);
					API::lazy_refresh();
				}

				void mouse_move(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					int what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
					if(metrics_.what != what || ei.mouse.left_button)
					{
						if(ei.mouse.left_button == false)
							metrics_.what = what;

						//if metrics_.trace is 1, it indicates the mouse pressed on the scroll button. So the scroll button must trace the mouse
						drawer_.draw(graph, metrics_.what, ei.mouse.left_button,
							(ei.mouse.left_button && (1 == metrics_.trace) ? (Vertical ? ei.mouse.y : ei.mouse.x) - metrics_.mouse_pos : 0)
							);
						API::lazy_refresh();
					}
				}

				void mouse_down(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(ei.mouse.left_button)
					{
						metrics_.what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
						switch(metrics_.what)
						{
						case drawer::buttons::first:
						case drawer::buttons::second:
							this->make_step(metrics_.what == drawer::buttons::second);
							break;
						case drawer::buttons::scroll:
						case drawer::buttons::none:
							metrics_.trace = (metrics_.what == drawer::buttons::none ? 2 : 1);
							metrics_.mouse_pos = (Vertical ? ei.mouse.y : ei.mouse.x);
							break;
						}
						drawer_.draw(graph, metrics_.what, true);
						API::lazy_refresh();
						API::capture_window(widget_->handle(), true);
					}
				}

				void mouse_up(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(ei.mouse.left_button)
					{
						metrics_.what = drawer_.what(graph, ei.mouse.x, ei.mouse.y);
						metrics_.mouse_pos = 0;
						metrics_.trace = 0;
						drawer_.draw(graph, metrics_.what, false);
						API::lazy_refresh();
						API::capture_window(widget_->handle(), false);
					}
				}

				void mouse_leave(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					metrics_.what = -1;
					drawer_.draw(graph, -1, false);
					API::lazy_refresh();
				}

				void mouse_wheel(graph_reference graph, const nana::gui::eventinfo& ei)
				{
					size_type value = metrics_.value;

					this->make_step(ei.wheel.upwards == false);
					this->make_step(ei.wheel.upwards == false);

					if(value != this->make_step(ei.wheel.upwards == false))
					{
						drawer_.draw(graph, metrics_.what, false);
						API::lazy_refresh();
					}
				}
			private:
				nana::gui::widget * widget_;
				nana::paint::graphics * graph_;
				metrics_type metrics_;
				drawer	drawer_;
			};
		}//end namespace scroll
	}//end namespace drawerbase

	template<bool Vertical>
	class scroll
		: public widget_object<category::widget_tag, drawerbase::scroll::trigger<Vertical> >
	{
		typedef widget_object<category::widget_tag, drawerbase::scroll::trigger<Vertical> > base_type;
	public:
		typedef size_t size_type;

		scroll(){}

		scroll(window wd)
		{
			//create is an unqualified name.
			this->create(wd, 0, 0, 0, 0);
		}

		scroll(window wd, const nana::rectangle& r)
		{
			this->create(wd, r.x, r.y, r.width, r.height);
		}

		scroll(window wd, int x, int y, unsigned width, unsigned height)
		{
			this->create(wd, x, y, width, height);
		}

		size_type amount() const
		{
			return this->get_drawer_trigger().metrics().peak;
		}

		void amount(size_type Max)
		{
			return this->get_drawer_trigger().peak(Max);
		}

		size_type range() const
		{
			return this->get_drawer_trigger().metrics().range;
		}

		void range(size_type r)
		{
			return this->get_drawer_trigger().range(r);
		}

		size_type value() const
		{
			return this->get_drawer_trigger().metrics().value;
		}

		void value(size_type s)
		{
			return this->get_drawer_trigger().value(s);
		}

		size_type step() const
		{
			return this->get_drawer_trigger().metrics().step;
		}

		void step(size_type s)
		{
			return this->get_drawer_trigger().step(s);
		}

		size_type make_step(bool forward)
		{
			size_t v = value();
			if(v != this->get_drawer_trigger().make_step(forward))
			{
				API::refresh_window(this->handle()); //need this-> because of two-stage name lookup
				return value();
			}
			return v;
		}

		size_type make_scroll(bool forward)
		{
			size_t v = value();
			this->get_drawer_trigger().make_step(forward);
			this->get_drawer_trigger().make_step(forward);

			if(v != this->get_drawer_trigger().make_step(forward))
			{
				API::refresh_window(this->handle()); //need this-> because of two-stage name lookup
				return value();
			}
			return v;
		}
	};//end class scroll

}//end namespace gui
}//end namespace nana

#endif
