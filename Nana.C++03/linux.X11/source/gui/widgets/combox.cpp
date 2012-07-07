/*
 *	A Combox Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/combox.cpp
 */

#include <nana/gui/wvl.hpp>
#include <nana/gui/widgets/combox.hpp>
#include <nana/paint/gadget.hpp>
#include <nana/system/dataexch.hpp>
#include <nana/gui/widgets/float_listbox.hpp>
#include <nana/gui/widgets/skeletons/text_editor.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace combox
		{
			class drawer_impl
			{
			public:
				typedef nana::paint::graphics & graph_reference;
				typedef nana::gui::widget	& widget_reference;

				enum{WhereUnknown, WhereText, WherePushButton};
				enum{StateNone, StateMouseOver, StatePress};

				mutable ext_event_tag ext_event;

				drawer_impl()
					:widget_(0), graph_(0), item_renderer_(0)
				{}

				void renderer(drawerbase::float_listbox::item_renderer* ir)
				{
					item_renderer_ = ir;
				}

				void insert(const nana::string& text)
				{
					module_.strings.push_back(text);
				}

				int get_where() const
				{
					return state_.pointer_where;
				}

				void bind(widget_reference wd)
				{
					widget_ = &wd;
				}

				void attached(graph_reference graph)
				{
					editor_ = new widgets::skeletons::text_editor(widget_->handle(), graph);
					editor_->border_renderer(nana::make_fun(*this, &drawer_impl::draw_border));
					editor_->multi_lines(false);
					this->editable(false);

					graph_ = &graph;
				}

				void detached()
				{
					delete editor_;
					editor_ = 0;
					graph_ = 0;
				}

				void text_area(unsigned width, unsigned height)
				{
					editor_->text_area(nana::rectangle(2, 2, width > 19 ? width - 19 : 0, height > 4 ? height - 4 : 0));
				}

				widgets::skeletons::text_editor * editor() const
				{
					return editor_;
				}

				nana::gui::widget* widget()
				{
					return widget_;
				}

				nana::paint::graphics * graph()
				{
					return graph_;
				}

				void clear()
				{
					module_.strings.clear();
				}

				void editable(bool enb)
				{
					if(editor_)
					{
						editor_->editable(enb);

						if(enb)
							editor_->ext_renderer().background.close();
						else
							editor_->ext_renderer().background.assign(*this, &drawer_impl::_m_draw_background);

						editor_->enable_background(enb);
						editor_->enable_background_counterpart(!enb);
						API::refresh_window(this->widget_->handle());
					}
				}

				bool editable() const
				{
					return (editor_ ? editor_->editable() : false);
				}

				bool calc_where(graph_reference graph, int x, int y)
				{
					int new_where = WhereUnknown;

					if(1 < x && x < static_cast<int>(graph.width()) - 2 && 1 < y && y < static_cast<int>(graph.height()) - 2)
					{
						if((editor_->editable() == false) || (static_cast<int>(graph.width()) - 22 <= x))
							new_where = WherePushButton;
						else
							new_where = WhereText;
					}

					if(new_where != state_.pointer_where)
					{
						state_.pointer_where = new_where;
						return true;
					}
					return false;
				}

				void set_mouse_over(bool mo)
				{
					state_.state = mo ? StateMouseOver : StateNone;
					state_.pointer_where = WhereUnknown;
				}

				void set_mouse_press(bool mp)
				{
					state_.state = (mp ? StatePress : StateMouseOver);
				}

				void set_focused(bool f)
				{
					if(editor_)
					{
						state_.focused = f;
						if(editor_->editable())
						{
							if(f)
								editor_->select_all();
							else
								editor_->cancel_select();
						}
					}
				}

				bool has_lister() const
				{
					return (state_.lister != 0);
				}

				void open_lister()
				{
					if(state_.lister == 0)
					{
						state_.lister = & nana::gui::form_loader<nana::gui::float_listbox>()(widget_->handle(), 0, widget_->size().height, widget_->size().width, 10);
						state_.lister->renderer(item_renderer_);
						state_.lister->set_module(module_);
						//The lister window closes by itself. I just take care about the unload event.
						state_.lister->make_event<nana::gui::events::unload>(*this, &drawer_impl::_m_lister_close_sig);
					}
				}

				void scroll_items(bool upwards)
				{
					if(state_.lister)
						state_.lister->scroll_items(upwards);
				}

				void move_items(bool upwards, bool recycle)
				{
					if(0 == state_.lister)
					{
						unsigned orig_i = module_.index;
						if(upwards)
						{
							if(module_.index)
								--(module_.index);
							else if(recycle)
								module_.index = static_cast<unsigned>(module_.strings.size() - 1);
						}
						else
						{
							if(module_.index != module_.strings.size() - 1)
								++(module_.index);
							else if(recycle)
								module_.index = 0;
						}

						if(orig_i != module_.index)
							this->set_text(module_.strings[module_.index]);
					}
					else
						state_.lister->move_items(upwards, recycle);
				}

				void set_text(const nana::string& text)
				{
					if(widget_)
					{
						nana::point pos = API::cursor_position();
						API::calc_window_point(widget_->handle(), pos);
						if(this->calc_where(*graph_, pos.x, pos.y))
							state_.state = StateNone;

						editor_->text(text);
						bool enb = widget_->enabled();
						_m_draw_push_button(enb);

						//Yes, it's safe to static_cast here!
						ext_event.selected(*static_cast<nana::gui::combox*>(widget_));
					}
				}

				void draw()
				{
					bool enb = widget_->enabled();

					if(editor_)
					{
						nana::size sz = widget_->size();
						text_area(sz.width, sz.height);
						editor_->redraw(state_.focused);
					}
					_m_draw_push_button(enb);
				}

				void draw_border(nana::paint::graphics& graph)
				{
					nana::rectangle r(graph.size());
					graph.rectangle(r, (state_.focused ? 0x0595E2 : 0x999A9E), false);
					r.x = r.y = 1;
					r.width -= 2;
					r.height -= 2;
					graph.rectangle(r, 0xFFFFFF, false);
				}

				unsigned option() const
				{
					return module_.index;
				}

				void option(unsigned index)
				{
					if(index < module_.strings.size())
					{
						nana::string & str = module_.strings[index];
						if(module_.index != index)
						{
							module_.index = index;
							set_text(str);
						}
						else if(str != widget_->caption())
							set_text(str);
					}
				}

				nana::string text(unsigned i) const
				{
					if(i < module_.strings.size())
						return module_.strings[i];
					return nana::string();
				}
			private:
				void _m_lister_close_sig()
				{
					state_.lister = 0;	//The lister closes by itself.
					if(module_.index != module_.npos)
					{
						set_text(module_.strings[module_.index]);
						API::update_window(*widget_);
					}
				}

				void _m_draw_background(nana::paint::graphics&, const nana::rectangle&, nana::color_t)
				{
					int x = 1, y = 1;
					unsigned color_start = color::button_face_shadow_start, color_end = gui::color::button_face_shadow_end;
					if(state_.state == StatePress)
					{
						x = 2;
						y = 2;
						color_start = gui::color::button_face_shadow_end;
						color_end = gui::color::button_face_shadow_start;
					}

					graph_->shadow_rectangle(x, y, graph_->width() - (x << 1), graph_->height() - (y << 1), color_start, color_end, true);
				}

				void _m_draw_push_button(bool enabled)
				{
					if(0 == graph_) return;

					int left = graph_->width() - 17;
					int right = left + 16;
					int top = 1;
					int bottom = graph_->height() - 2;
					int mid = top + (bottom - top) * 5 / 18;

					nana::color_t topcol, topcol_ln, botcol, botcol_ln;
					if(this->has_lister() || (state_.state == StatePress && state_.pointer_where == WherePushButton))
					{
						topcol_ln = nana::paint::graphics::mix(0x3F476C, 0x0, 0.8);
						botcol_ln = nana::paint::graphics::mix(0x031141, 0x0, 0.8);
						topcol = nana::paint::graphics::mix(0x3F83B4, 0x0, 0.8);
						botcol = nana::paint::graphics::mix(0x0C4A95, 0x0, 0.8);					
					}
					else if(state_.state == this->StateMouseOver)
					{
						topcol_ln = nana::paint::graphics::mix(0x3F476C, 0xFFFFFF, 0.9);
						botcol_ln = nana::paint::graphics::mix(0x031141, 0xFFFFFF, 0.9);
						topcol = nana::paint::graphics::mix(0x3F83B4, 0xFFFFFF, 0.9);
						botcol = nana::paint::graphics::mix(0x0C4A95, 0xFFFFFF, 0.9);
					}
					else
					{
						topcol_ln = 0x3F476C;
						botcol_ln = 0x031141;
						topcol = 0x3F83B4;
						botcol = 0x0C4A95;
					}

					graph_->line(left, top, left, mid, topcol_ln);
					graph_->line(right - 1, top, right - 1, mid, topcol_ln);

					graph_->line(left, mid + 1, left, bottom, botcol_ln);
					graph_->line(right - 1, mid + 1, right - 1, bottom, botcol_ln);

					graph_->rectangle(left + 1, top, right - left - 2, mid - top + 1, topcol, true);
					graph_->rectangle(left + 1, mid + 1, right - left - 2, bottom - mid, botcol, true);

					nana::paint::gadget::arrow_16_pixels(*graph_, left, top + ((bottom - top) / 2) - 7, (enabled ? 0xFFFFFF : nana::gui::color::dark_border), 1, nana::paint::gadget::directions::to_south);

				}
			private:
				nana::gui::float_listbox::module_type module_;
				nana::gui::widget * widget_;
				nana::paint::graphics * graph_;

				drawerbase::float_listbox::item_renderer* item_renderer_;

				//textbox	txtbox_;
				widgets::skeletons::text_editor * editor_;

				struct state_type
				{
					bool focused;
					int		state;
					int	 pointer_where;

					nana::gui::float_listbox * lister;

					state_type()
						:   focused(false),
                            state(StateNone), pointer_where(WhereUnknown), lister(0)
					{}

				}state_;
			};


			//class trigger
				trigger::trigger()
					: drawer_(new drawer_impl)
				{
				}

				trigger::~trigger()
				{
					delete drawer_;
					drawer_ = 0;
				}

				drawer_impl& trigger::get_drawer_impl()
				{
					return *drawer_;
				}

				const drawer_impl& trigger::get_drawer_impl() const
				{
					return *drawer_;
				}

				void trigger::bind_window(trigger::widget_reference wd)
				{
					drawer_->bind(wd);
					wd.background(0xFFFFFF);
				}

				void trigger::attached(trigger::graph_reference graph)
				{
					drawer_->attached(graph);

					window wd = drawer_->widget()->handle();
					using namespace API::dev;
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_up>(wd);
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_enter>(wd);
					make_drawer_event<events::mouse_leave>(wd);
					make_drawer_event<events::focus>(wd);
					make_drawer_event<events::mouse_wheel>(wd);
					make_drawer_event<events::key_down>(wd);
					make_drawer_event<events::key_char>(wd);
					make_drawer_event<events::focus>(wd);

					effects::edge_nimbus(wd, effects::edge_nimbus_active);
					effects::edge_nimbus(wd, effects::edge_nimbus_over);
				}

				void trigger::detached()
				{
					drawer_->detached();
					API::dev::umake_drawer_event(drawer_->widget()->handle());
				}

				void trigger::refresh(trigger::graph_reference)
				{
					drawer_->draw();
				}

				void trigger::focus(trigger::graph_reference, const nana::gui::eventinfo& ei)
				{
					drawer_->set_focused(ei.focus.getting);
					if(drawer_->widget()->enabled())
					{
						drawer_->draw();
						drawer_->editor()->reset_caret();
						API::lazy_refresh();
					}
				}

				void trigger::mouse_enter(trigger::graph_reference, const nana::gui::eventinfo&)
				{
					drawer_->set_mouse_over(true);
					if(drawer_->widget()->enabled())
					{
						drawer_->draw();
						API::lazy_refresh();
					}
				}

				void trigger::mouse_leave(trigger::graph_reference, const nana::gui::eventinfo&)
				{
					drawer_->set_mouse_over(false);
					drawer_->editor()->mouse_enter(false);
					if(drawer_->widget()->enabled())
					{
						drawer_->draw();
						API::lazy_refresh();
					}
				}

				void trigger::mouse_down(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					drawer_->set_mouse_press(true);

					if(drawer_->widget()->enabled())
					{
						widgets::skeletons::text_editor * editor = drawer_->editor();

						if(editor->mouse_down(ei.mouse.left_button, ei.mouse.x, ei.mouse.y))
						{
						}
						else if(drawer_->WherePushButton == drawer_->get_where())
							drawer_->open_lister();

						drawer_->draw();
						if(drawer_->editor()->editable())
							drawer_->editor()->reset_caret();

						API::lazy_refresh();
					}
				}

				void trigger::mouse_up(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(drawer_->widget()->enabled())
					{
						if(false == drawer_->has_lister())
						{
							drawer_->editor()->mouse_up(ei.mouse.left_button, ei.mouse.x, ei.mouse.y);
							drawer_->set_mouse_press(false);
							drawer_->draw();
							API::lazy_refresh();
						}
					}
				}

				void trigger::mouse_move(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(drawer_->widget()->enabled())
					{
						bool redraw = drawer_->calc_where(graph, ei.mouse.x, ei.mouse.y);
						redraw |= drawer_->editor()->mouse_move(ei.mouse.left_button, ei.mouse.x, ei.mouse.y);

						if(redraw)
						{
							drawer_->draw();
							drawer_->editor()->reset_caret();
							API::lazy_refresh();
						}
					}
				}

				void trigger::mouse_wheel(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(drawer_->widget()->enabled())
					{
						if(drawer_->has_lister())
							drawer_->scroll_items(ei.wheel.upwards);
						else
							drawer_->move_items(ei.wheel.upwards, false);
					}
				}

				void trigger::key_down(trigger::graph_reference, const nana::gui::eventinfo& ei)
				{
					if(drawer_->widget()->enabled())
					{
						using namespace nana::gui;
						switch(ei.keyboard.key)
						{
						case keyboard::up:
						case keyboard::left:
							drawer_->move_items(true, true);
							break;
						case keyboard::down:
						case keyboard::right:
							drawer_->move_items(false, true);
							break;
						}
					}
				}

				void trigger::key_char(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					widgets::skeletons::text_editor * editor = drawer_->editor();
					if(drawer_->widget()->enabled() && editor->editable())
					{
						switch(ei.keyboard.key)
						{
						case '\b':
							editor->backspace();	break;
						case '\n': case '\r':
							editor->enter();	break;
						case keyboard::cancel:
							editor->copy();	break;
						case keyboard::sync:
							editor->paste();	break;
						case keyboard::tab:
							editor->put(keyboard::tab); break;
						default:
							if(ei.keyboard.key >= 0xFF || (32 <= ei.keyboard.key && ei.keyboard.key <= 126))
								editor->put(ei.keyboard.key);
							else if(sizeof(nana::char_t) == sizeof(char))
							{	//Non-Unicode Version for Non-English characters
								if(ei.keyboard.key & (1<<(sizeof(nana::char_t)*8 - 1)))
									editor->put(ei.keyboard.key);
							}
						}
						editor->reset_caret();
						API::lazy_refresh();
					}
					else
					{
						switch(ei.keyboard.key)
						{
						case keyboard::cancel:
							editor->copy();	break;
						case keyboard::sync:
							editor->paste();	break;
						}
					}
				}
			//end class trigger
		}
	}//end namespace drawerbase

	//class combox
		combox::combox(){}

		combox::combox(window wd)
		{
			create(wd, 0, 0, 0, 0);
		}

		combox::combox(window wd, const nana::rectangle& r)
		{
			this->create(wd, r.x, r.y, r.width, r.height);
		}

		combox::combox(window wd, int x, int y, unsigned w, unsigned h)
		{
			this->create(wd, x, y, w, h);
		}

		void combox::clear()
		{
			nana::gui::internal_scope_guard sg;
			get_drawer_trigger().get_drawer_impl().clear();
			API::refresh_window(this->handle());
		}

		void combox::editable(bool eb)
		{
			get_drawer_trigger().get_drawer_impl().editable(eb);
		}

		bool combox::editable() const
		{
			return get_drawer_trigger().get_drawer_impl().editable();
		}

		combox& combox::push_back(const nana::string& text)
		{
			get_drawer_trigger().get_drawer_impl().insert(text);
			return *this;
		}

		unsigned combox::option() const
		{
			return get_drawer_trigger().get_drawer_impl().option();
		}

		void combox::option(unsigned i)
		{
			get_drawer_trigger().get_drawer_impl().option(i);
		}

		nana::string combox::text(unsigned i) const
		{
			return get_drawer_trigger().get_drawer_impl().text(i);
		}

		combox::ext_event_type& combox::ext_event() const
		{
			return get_drawer_trigger().get_drawer_impl().ext_event;
		}

		void combox::renderer(combox::item_renderer* ir)
		{
			get_drawer_trigger().get_drawer_impl().renderer(ir);
		}
	//end class combox
}//end namespace gui
}
