/*
 *	A Tooltip Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/tooltip.cpp
 */

#include <nana/gui/tooltip.hpp>
#include <nana/gui/timer.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace tooltip
		{
			class drawer
				: public nana::gui::drawer_trigger
			{
			public:
				void set_tooltip_text(const nana::string& text)
				{
					text_ = text;

					nana::size txt_s;

					nana::string::size_type beg = 0;
					nana::string::size_type off = text.find('\n', beg);

					for(;off != nana::string::npos; off = text.find('\n', beg))
					{
						nana::size s = graph_->text_extent_size(text_.c_str() + beg, off - beg);
						if(s.width > txt_s.width)
							txt_s.width = s.width;
						txt_s.height += s.height + 2;
						beg = off + 1;
					}

					nana::size s = graph_->text_extent_size(text_.c_str() + beg);
					if(s.width > txt_s.width)
						txt_s.width = s.width;
					txt_s.height += s.height;

					widget_->size(txt_s.width + 10, txt_s.height + 10);
				}
			private:
				void bind_window(widget_reference widget)
				{
					widget_ = &widget;
				}
				void attached(graph_reference graph)
				{
					graph_ = &graph;
				}

				void refresh(graph_reference graph)
				{
					nana::size text_s = graph.text_extent_size(text_);

					graph.rectangle(0, 0, graph.width(), graph.height(), 0x0, false);
					graph.rectangle(1, 1, graph.width() - 2, graph.height() - 2, 0xF0F0F0, true);

					const int x = 5;
					int y = 5;

					nana::string::size_type beg = 0;
					nana::string::size_type off = text_.find('\n', beg);

					for(;off != nana::string::npos; off = text_.find('\n', beg))
					{
						graph.string(x, y, 0x0, text_.substr(beg, off - beg));
						y += text_s.height + 2;
						beg = off + 1;
					}

					graph.string(x, y, 0x0, text_.c_str() + beg, text_.size() - beg);
				}
			private:
				nana::gui::widget	*widget_;
				nana::paint::graphics * graph_;
				nana::string text_;
			};

			class window
				: public nana::gui::widget_object<nana::gui::category::root_tag, drawer>
			{
				typedef nana::gui::widget_object<nana::gui::category::root_tag, drawer> base_type;
			public:
				window()
					:base_type(0, 0, 0, 0, nana::gui::appear::bald<nana::gui::appear::floating>())
				{
					nana::gui::API::take_active(this->handle(), false, 0);
					timer_.interval(500);
					timer_.make_tick(nana::functor<void()>(*this, &window::_m_tick));
				}

				void set_tooltip_text(const nana::string& text)
				{
					this->get_drawer_trigger().set_tooltip_text(text);
				}
			private:
				void _m_tick()
				{
					timer_.enable(false);
					this->show();
				}

			private:
				nana::gui::timer timer_;

			};//class window


			class controller
			{
				typedef std::pair<nana::gui::window, nana::string> pair_t;
				typedef controller self_type;

				controller()
					:window_(0), count_ref_(0)
				{}
			public:

				static nana::threads::token& locker()
				{
					static nana::threads::token lock;
					return lock;
				}

				static controller* object(bool destroy = false)
				{
					static controller* ptr;
					if(destroy)
					{
						delete ptr;
						ptr = 0;
					}
					else if(ptr == 0)
					{
						ptr = new controller;
					}
					return ptr;
				}

				void inc()
				{
					++count_ref_;
				}

				unsigned long dec()
				{
					return --count_ref_;
				}

				void set(nana::gui::window wnd, const nana::string& str)
				{
					pair_t * p = _m_get(wnd);
					p ->second = str;
				}

				void show(int x, int y, const nana::string& text)
				{
					if(0 == window_)
						window_ = new window;

					window_->set_tooltip_text(text);
					nana::size sz = window_->size();
					nana::size screen_size = API::screen_size();
					if(x + sz.width >= screen_size.width)
						x = static_cast<int>(screen_size.width) - static_cast<int>(sz.width);

					if(y + sz.height >= screen_size.height)
						y = static_cast<int>(screen_size.height) - static_cast<int>(sz.height);

					if(x < 0) x = 0;
					if(y < 0) y = 0;
					window_->move(x, y);
				}

				void close()
				{
					delete window_;
					window_ = 0;
				}
			private:
				void _m_enter(const nana::gui::eventinfo& ei)
				{
					pair_t * p = _m_get(ei.window);
					if(p && p->second.size())
					{
						nana::point pos = nana::gui::API::cursor_position();
						this->show(pos.x, pos.y + 20, p->second);
					}
				}

				void _m_leave(const nana::gui::eventinfo& ei)
				{
					delete window_;
					window_ = 0;
				}

				void _m_destroy(const nana::gui::eventinfo& ei)
				{
					for(std::vector<pair_t*>::iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if((*i)->first == ei.window)
						{
							cont_.erase(i);
							return;
						}
					}
				}
			private:
				pair_t* _m_get(nana::gui::window wnd)
				{
					for(std::vector<pair_t*>::iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if((*i)->first == wnd)
							return *i;
					}

					nana::gui::API::make_event<nana::gui::events::mouse_enter>(wnd, nana::functor<void(const nana::gui::eventinfo&)>(*this, &self_type::_m_enter));
					nana::gui::API::make_event<nana::gui::events::mouse_leave>(wnd, nana::functor<void(const nana::gui::eventinfo&)>(*this, &self_type::_m_leave));
					nana::gui::API::make_event<nana::gui::events::mouse_down>(wnd, nana::functor<void(const nana::gui::eventinfo&)>(*this, &self_type::_m_leave));
					nana::gui::API::make_event<nana::gui::events::destroy>(wnd, nana::functor<void(const nana::gui::eventinfo&)>(*this, &self_type::_m_destroy));

					pair_t * newp = new pair_t(wnd, nana::string());
					cont_.push_back(newp);

					return newp;
				}
			private:
				window * window_;
				std::vector<pair_t*> cont_;
				unsigned long count_ref_;
			};
		}//namespace tooltip
	}//namespace drawerbase

	//class tooltip
		tooltip::tooltip()
		{
			nana::threads::scope_guard sg(drawerbase::tooltip::controller::locker());
			drawerbase::tooltip::controller::object()->inc();
		}

		tooltip::~tooltip()
		{
			nana::threads::scope_guard sg(drawerbase::tooltip::controller::locker());

			if(0 == drawerbase::tooltip::controller::object()->dec())
				drawerbase::tooltip::controller::object(true);
		}

		void tooltip::set(nana::gui::window wnd, const nana::string& text)
		{
			if(false == nana::gui::API::empty_window(wnd))
			{
				nana::threads::scope_guard sg(drawerbase::tooltip::controller::locker());
				drawerbase::tooltip::controller::object()->set(wnd, text);
			}
		}

		void tooltip::show(nana::gui::window wnd, int x, int y, const nana::string& text)
		{
			nana::threads::scope_guard sg(drawerbase::tooltip::controller::locker());

			nana::point pos(x, y);
			nana::gui::API::calc_screen_point(wnd, pos);
			drawerbase::tooltip::controller::object()->show(pos.x, pos.y, text);
		}

		void tooltip::close()
		{
			nana::threads::scope_guard sg(drawerbase::tooltip::controller::locker());
			drawerbase::tooltip::controller::object()->close();
		}
	//};//class tooltip

}//namespace gui
}//namespace nana

