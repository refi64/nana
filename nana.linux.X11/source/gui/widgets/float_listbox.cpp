/*
 *	A float_listbox Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/float_listbox.cpp
 */

#include <nana/gui/widgets/float_listbox.hpp>
#include <nana/gui/widgets/scroll.hpp>

namespace nana{ namespace gui{
	namespace drawerbase{
		namespace float_listbox
		{
			//class item_renderer
				item_renderer::~item_renderer(){}

				void item_renderer::render(item_renderer::widget_reference, item_renderer::graph_reference graph, const nana::rectangle& r, const nana::string& text, item_renderer::state_t state)
				{
					if(state == StateHighlighted)
					{
						graph.rectangle(r, 0xAFC7E3, false);
						graph.rectangle(r.x + 1, r.y + 1, r.width - 2, r.height - 2, 0xEBF4FB, false);

						graph.set_pixel(r.x, r.y, 0xFFFFFF);
						graph.set_pixel(r.x + r.width - 1, r.y, 0xFFFFFF);
						graph.set_pixel(r.x, r.y + r.height - 1, 0xFFFFFF);
						graph.set_pixel(r.x + r.width - 1, r.y + r.height - 1, 0xFFFFFF);

						graph.set_pixel(r.x + 1, r.y + 1, 0xAFC7E3);
						graph.set_pixel(r.x + r.width - 2, r.y + 1, 0xAFC7E3);
						graph.set_pixel(r.x + 1, r.y + r.height - 2, 0xAFC7E3);
						graph.set_pixel(r.x + r.width - 2, r.y + r.height - 2, 0xAFC7E3);

						graph.shadow_rectangle(r.x + 2, r.y + 2, r.width - 4, r.height - 4, 0xDDECFD, 0xC2DCFD, true);
					}
					else
						graph.rectangle(r, 0xFFFFFF, true);
					
					graph.string(r.x + 2, r.y + 2, 0x0, text);
				}

				unsigned item_renderer::item_pixels(item_renderer::graph_reference graph) const
				{
					return graph.text_extent_size(STR("jHWn/?\\{[(0569")).height + 4;
				}
			//}; end class item_renderer

			//struct module_def
				module_def::module_def()
					:max_items(10), index(npos)
				{}
			//end struct module_def

			//class drawer_impl
			class drawer_impl
			{
			public:
				typedef nana::gui::widget& widget_reference;
				typedef nana::paint::graphics& graph_reference;

				static const unsigned npos = static_cast<unsigned>(-1);

				drawer_impl():widget_(0), graph_(0), module_(0){}

				void clear_state()
				{
					state_.offset_y = 0;
					state_.index = npos;
				}

				void renderer(item_renderer* ir)
				{
					state_.renderer = (ir ? ir : state_.orig_renderer);
				}

				void scroll_items(bool upwards)
				{
					if(scrollbar_.empty()) return;

					bool update = false;
					if(upwards)
					{
						if(state_.offset_y)
						{
							--(state_.offset_y);
							update = true;
						}
					}
					else 
					{
						if((state_.offset_y + module_->max_items) < module_->strings.size())
						{
							++(state_.offset_y);
							update = true;
						}
					}

					if(update)
					{
						draw();
						scrollbar_.value(state_.offset_y);
						API::update_window(widget_->handle());
					}
				}

				void move_items(bool upwards, bool recycle)
				{
					if(module_ && module_->strings.size())
					{
						unsigned init_index = state_.index;
						if(state_.index != module_->npos)
						{
							unsigned last_offset_y = 0;
							if(module_->strings.size() > module_->max_items)
								last_offset_y = static_cast<unsigned>(module_->strings.size() - module_->max_items);

							if(upwards)
							{
								if(state_.index)
									--(state_.index);
								else if(recycle)
								{
									state_.index = static_cast<unsigned>(module_->strings.size() - 1);
									state_.offset_y = last_offset_y;
								}

								if(state_.index < state_.offset_y)
									state_.offset_y = state_.index;
							}
							else
							{
								if(state_.index < module_->strings.size() - 1)
									++(state_.index);
								else if(recycle)
								{
									state_.index = 0;
									state_.offset_y = 0;
								}

								if(state_.index >= state_.offset_y + module_->max_items)
									state_.offset_y = state_.index - module_->max_items + 1;
							}
						}
						else
							state_.index = 0;

						if(init_index != state_.index)
						{
							draw();
							scrollbar_.value(state_.offset_y);
							API::update_window(widget_->handle());
						}
					}
				}

				size_t index() const
				{
					return state_.index;
				}

				nana::gui::widget* widget()
				{
					return widget_;
				}

				void attach(nana::gui::widget* wd, nana::paint::graphics* graph)
				{
					if(wd)
					{
						widget_ = wd;
						wd->make_event<events::mouse_wheel>(*this, &drawer_impl::_m_on_scroll);
					}
					if(graph) graph_ = graph;
				}

				void detach()
				{
					graph_ = 0;
				}

				void resize()
				{
					if(module_)
					{
						size_t items = (module_->max_items <= module_->strings.size() ? module_->max_items : module_->strings.size());
						size_t h = items * state_.renderer->item_pixels(*graph_);
						widget_->size(widget_->size().width, static_cast<unsigned>(h + 4));
					}
				}

				void set_module(const module_def& md)
				{
					module_ = &md;
				}

				void set_result()
				{
					if(module_)
						module_->index = state_.index;
				}

				bool right_area(graph_reference graph, int x, int y) const
				{
					return ((1 < x && 1 < y) &&
						x < static_cast<int>(graph.width()) - 2 &&
						y < static_cast<int>(graph.height()) - 2);
				}

				bool set_mouse(graph_reference graph, int x, int y)
				{
					if(this->right_area(graph, x, y))
					{
						const unsigned n = (y - 2) / state_.renderer->item_pixels(graph) + state_.offset_y;
						if(n != state_.index)
						{
							state_.index = n;
							return true;
						}
					}
					return false;
				}

				void draw()
				{
					if(module_)
					{
						bool pages = (module_->max_items < module_->strings.size());
						const unsigned outter_w = (4 + (pages ? 16 : 0));

						if(graph_->width() > outter_w && graph_->height() > 4 )
						{
							//Draw items
							unsigned items = static_cast<unsigned>(pages ? module_->max_items : module_->strings.size());							
							items += state_.offset_y;

							const unsigned item_pixels = state_.renderer->item_pixels(*graph_);

							nana::rectangle item_r(2, 2, graph_->width() - outter_w, item_pixels);

							for(unsigned i = state_.offset_y; i < items; ++i)
							{
								item_renderer::state_t state = item_renderer::StateNone;
								if(i == state_.index) state = item_renderer::StateHighlighted;

								state_.renderer->render(*widget_, *graph_, item_r, module_->strings[i], state);
								item_r.y += item_pixels;
							}
						}	
						_m_open_scrollbar(*widget_, pages);
					}
					else
					{
						graph_->string(4, 4, 0x808080, STR("Empty Listbox, No Module!"));
					}

					//Draw border
					graph_->rectangle(0, 0, graph_->width(), graph_->height(), 0x0, false);
					graph_->rectangle(1, 1, graph_->width() - 2, graph_->height() - 2, 0xFFFFFF, false);
				}
			private:
				void _m_open_scrollbar(widget_reference wd, bool v)
				{
					if(v)
					{
						if(scrollbar_.empty() && module_)
						{
							scrollbar_.create(wd, wd.size().width - 18, 2, 16, wd.size().height - 4);
							scrollbar_.amount(module_->strings.size());
							scrollbar_.range(module_->max_items);
							scrollbar_.value(state_.offset_y);
							scrollbar_.make_event<events::mouse_wheel>(*this, &drawer_impl::_m_on_scroll);
							scrollbar_.make_event<events::mouse_move>(*this, &drawer_impl::_m_on_scroll);
							scrollbar_.make_event<events::mouse_up>(*this, &drawer_impl::_m_on_scroll);
						}
					}
					else
						scrollbar_.close();
				}

				void _m_on_scroll(const nana::gui::eventinfo& ei)
				{
					switch(ei.identifier)
					{
					case nana::gui::events::mouse_wheel::identifier:
						this->scroll_items(ei.wheel.upwards);
						break;
					case nana::gui::events::mouse_move::identifier:
					case nana::gui::events::mouse_up::identifier:
						if(ei.mouse.left_button && (scrollbar_.value() != state_.offset_y))
						{
							state_.offset_y = static_cast<unsigned>(scrollbar_.value());
							draw();
							API::update_window(widget_->handle());
						}
						break;
					}
				}
			private:
				nana::gui::widget * widget_;
				nana::paint::graphics * graph_;
				struct state_type
				{
					unsigned offset_y;
					unsigned index;			//The index of the selected item.

					item_renderer * const orig_renderer;
					item_renderer * renderer;

					state_type(): offset_y(0), index(npos), orig_renderer(new item_renderer), renderer(orig_renderer){}
					~state_type()
					{
						delete orig_renderer;
					}
				}state_;
				nana::gui::scroll<true> scrollbar_;

				const module_def* module_;
			};

			//class drawer_impl;

			
			//class trigger
				trigger::trigger()
					:drawer_(new drawer_impl)
				{}

				drawer_impl& trigger::get_drawer_impl()
				{
					return *drawer_;
				}

				const drawer_impl& trigger::get_drawer_impl() const
				{
					return *drawer_;
				}

				void trigger::bind_window(trigger::widget_reference widget)
				{
					drawer_->attach(&widget, 0);
				}

				void trigger::attached(trigger::graph_reference graph)
				{
					drawer_->attach(0, &graph);
					window wd = drawer_->widget()->handle();
					API::dev::make_drawer_event<events::mouse_move>(wd);
					API::dev::make_drawer_event<events::mouse_down>(wd);
					API::dev::make_drawer_event<events::mouse_up>(wd);
				}

				void trigger::detached()
				{
					drawer_->detach();
					API::dev::umake_drawer_event(drawer_->widget()->handle());
				}

				void trigger::refresh(trigger::graph_reference)
				{
					drawer_->draw();
				}

				void trigger::mouse_move(trigger::graph_reference graph, const eventinfo& ei)
				{
					if(drawer_->set_mouse(graph, ei.mouse.x, ei.mouse.y))
					{
						drawer_->draw();
						API::lazy_refresh();
					}
				}

				void trigger::mouse_up(trigger::graph_reference graph, const eventinfo& ei)
				{
					if(drawer_->right_area(graph, ei.mouse.x, ei.mouse.y))
						drawer_->set_result();
					
					drawer_->widget()->close();					
				}
			//end class trigger
		}
	}//end namespace drawerbase

	//class float_listbox
		float_listbox::float_listbox(){}

		float_listbox::float_listbox(nana::gui::window wd, int x, int y, unsigned width, unsigned height)
			:base_type(wd, false, x, y, width, height, appear::bald<appear::floating, appear::no_activate>())
		{
			API::capture_window(handle(), true);
			API::capture_ignore_children(false);
			API::take_active(handle(), false, parent());
			get_drawer_trigger().get_drawer_impl().clear_state();
		}

		void float_listbox::set_module(const float_listbox::module_type& md)
		{
			get_drawer_trigger().get_drawer_impl().set_module(md);
			get_drawer_trigger().get_drawer_impl().resize();
			show();
		}

		void float_listbox::scroll_items(bool upwards)
		{
			get_drawer_trigger().get_drawer_impl().scroll_items(upwards);
		}

		void float_listbox::move_items(bool upwards, bool recycle)
		{
			get_drawer_trigger().get_drawer_impl().move_items(upwards, recycle);
		}

		void float_listbox::renderer(item_renderer* ir)
		{
			get_drawer_trigger().get_drawer_impl().renderer(ir);
			get_drawer_trigger().get_drawer_impl().resize();
		}

		size_t float_listbox::index() const
		{
			return get_drawer_trigger().get_drawer_impl().index();
		}
	//end class float_listbox
}//end namespace gui
}
