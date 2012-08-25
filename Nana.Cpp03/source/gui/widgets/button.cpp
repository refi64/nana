/*
 *	A Button Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/button.cpp
 */

#include <nana/gui/widgets/button.hpp>
#include <nana/paint/text_renderer.hpp>

namespace nana{namespace gui{
namespace drawerbase
{
	namespace button
	{
		struct image_block
		{
			bool enable;
			nana::gui::button::state::t who;
			nana::point pos;
		};

		struct trigger::bgimage_tag
		{
			typedef nana::gui::button::state state;
			nana::paint::image image;
			nana::rectangle valid_area;
			nana::arrange	arrange;
			nana::size		block_size;
			image_block		block[nana::gui::button::state::disabled + 1];

			struct stretch_tag
			{
				nana::arrange arrange;
				int beg;
				int end;
			}stretch;

			bgimage_tag()
			{
				for(int i = 0; i < nana::gui::button::state::disabled + 1; ++i)
				{
					block[i].enable = true;
					block[i].who = static_cast<nana::gui::button::state::t>(i);
				}

				stretch.arrange = nana::arrange::unkown;
				stretch.beg = stretch.end = 0;
			}

			void set_valid_area(nana::arrange arg, const nana::rectangle& r)
			{
				arrange = arg;
				valid_area = r;
			}

			bool enable(state::t sta, bool eb)
			{
				if(state::normal <= sta && sta <= state::disabled)
				{
					if(block[sta].enable != eb)
					{
						block[sta].enable = eb;
						this->update_blocks();
						return true;
					}
				}
				return false;
			}

			bool join(state::t to, state::t from)
			{
				if(to != from)
				{
					if(state::normal <= to && to <= state::disabled && state::normal <= from && from <= state::disabled)
					{
						if(block[from].who != block[to].who)
						{
							block[from].who = block[to].who;
							update_blocks();
							return true;
						}
					}
				}
				return false;
			}

			void set_stretch(nana::arrange arg, int beg, int end)
			{
				stretch.arrange = arg;
				if(beg > end) beg = end;
				stretch.beg = beg;
				stretch.end = end;
			}

			void update_blocks()
			{
				int blocks = 0;
				for(int i = 0; i < nana::gui::button::state::disabled + 1; ++i)
				{
					if(block[i].enable && (block[i].who == i))
						++blocks;
				}

				if(blocks == 0) return;

				unsigned each_pixels = (arrange.value == arrange::horizontal ? valid_area.width : valid_area.height) / blocks;
				if(arrange.value == arrange::horizontal)
				{
					each_pixels = valid_area.width / blocks;
					block_size.width = each_pixels;
					block_size.height = valid_area.height;
				}
				else
				{
					each_pixels = valid_area.height / blocks;
					block_size.height = each_pixels;
					block_size.width = valid_area.width;
				}

				int pos = 0;
				for(int i = 0; i < nana::gui::button::state::disabled + 1; ++i)
				{
					if(block[i].enable && (block[i].who == i))
					{
						if(arrange.value == arrange::horizontal)
						{
							block[i].pos.x = valid_area.x + pos;
							block[i].pos.y = valid_area.y;
						}
						else
						{
							block[i].pos.x = valid_area.x;
							block[i].pos.y = valid_area.y + pos;
						}
						pos += static_cast<int>(each_pixels);
					}
				}
			}
		};

		trigger::attr_tag::attr_tag()
			:	omitted(false), focused(false), pressed(false), state(nana::gui::button::state::normal),
                enable_pushed(false), focus_color(true), icon(0)
		{}

		trigger::attr_tag::~attr_tag()
		{
			delete icon;
		}

		//trigger
		//@brief: draw the button
		trigger::trigger()
			:widget_(0), graph_(0), bgimage_(0)
		{
		}

		trigger::~trigger()
		{
			delete bgimage_;
		}

		void trigger::bind_window(nana::gui::widget& wdg)
		{
			widget_ = &wdg;
		}

		void trigger::attached(graph_reference graph)
		{
			window wd = widget_->handle();

			using namespace API::dev;
			make_drawer_event<events::mouse_enter>(wd);
			make_drawer_event<events::mouse_leave>(wd);
			make_drawer_event<events::mouse_down>(wd);
			make_drawer_event<events::mouse_up>(wd);
			make_drawer_event<events::key_char>(wd);
			make_drawer_event<events::key_down>(wd);
			make_drawer_event<events::focus>(wd);

			API::tabstop(wd);
			effects::edge_nimbus(wd, effects::edge_nimbus_active);
			effects::edge_nimbus(wd, effects::edge_nimbus_over);

			graph_ = &graph;
		}

		void trigger::detached()
		{
			API::dev::umake_drawer_event(widget_->handle());
		}

		bool trigger::enable_pushed(bool eb)
		{
			attr_.enable_pushed = eb;
			return((eb == false) && pushed(false));
		}

		bool trigger::pushed(bool pshd)
		{
			if(pshd != attr_.pressed)
			{
				attr_.pressed = pshd;
				if(pshd)
				{
					attr_.state = nana::gui::button::state::pressed;
				}
				else
				{
					window wd = API::find_window(API::cursor_position());
					if(wd == this->widget_->handle())
						attr_.state = nana::gui::button::state::highlight;
					else
						attr_.state = (attr_.focused ? nana::gui::button::state::focused : nana::gui::button::state::normal);
				}
				return true;
			}
			return false;
		}

		bool trigger::pushed() const
		{
			return attr_.pressed;
		}

		void trigger::omitted(bool om)
		{
			attr_.omitted = om;
		}

		bool trigger::focus_color(bool eb)
		{
			if(eb != attr_.focus_color)
			{
				attr_.focus_color = eb;
				return true;
			}
			return false;
		}

		void trigger::refresh(graph_reference graph)
		{
			_m_draw(graph);
		}

		void trigger::mouse_enter(graph_reference graph, const eventinfo&)
		{
			attr_.state = (attr_.pressed ?  nana::gui::button::state::pressed : nana::gui::button::state::highlight);
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::mouse_leave(graph_reference graph, const eventinfo&)
		{
			attr_.state = (attr_.focused ? nana::gui::button::state::focused : nana::gui::button::state::normal);
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::mouse_down(graph_reference graph, const eventinfo&)
		{
			attr_.pressed = true;
			attr_.state = nana::gui::button::state::pressed;
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::mouse_up(graph_reference graph, const eventinfo&)
		{
			if(attr_.enable_pushed)
				return;

			if(attr_.state == nana::gui::button::state::pressed)
				attr_.state = nana::gui::button::state::highlight;
			else
				attr_.state = (attr_.focused ? nana::gui::button::state::focused : nana::gui::button::state::normal);

			attr_.pressed = false;
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::key_char(graph_reference, const eventinfo& ei)
		{
			if(ei.keyboard.key == gui::keyboard::enter)
			{
				eventinfo e;
				e.mouse.ctrl = false;
				e.mouse.mid_button = e.mouse.right_button = false;
				e.mouse.left_button = true;
				e.mouse.shift = false;
				e.mouse.x = e.mouse.y = 0;
				API::raise_event<events::click>(widget_->handle(), e);
			}
		}

		void trigger::key_down(graph_reference, const eventinfo& ei)
		{
			bool ch_tabstop_next;
			switch(ei.keyboard.key)
			{
			case keyboard::left: case keyboard::up:
				ch_tabstop_next = false;
				break;
			case keyboard::right: case keyboard::down:
				ch_tabstop_next = true;
				break;
			default:
				return;
			}
			API::move_tabstop(widget_->handle(), ch_tabstop_next);
		}

		void trigger::focus(graph_reference graph, const eventinfo& ei)
		{
			attr_.focused = ei.focus.getting;
			_m_draw(graph);
			API::lazy_refresh();
		}

		void trigger::_m_draw_title(graph_reference graph, bool enabled)
		{
			nana::string text = widget_->caption();

			nana::string::value_type shortkey;
			nana::string::size_type shortkey_pos;
			nana::string str = API::transform_shortkey_text(text, shortkey, &shortkey_pos);

			nana::size ts = graph.text_extent_size(str);

			nana::size icon_sz;
			if(attr_.icon)
			{
				icon_sz = attr_.icon->size();
				icon_sz.width += 5;
			}

			int x = (static_cast<int>(graph.width()  - 1  - ts.width) >> 1);
			int y = (static_cast<int>(graph.height() - 1 - ts.height) >> 1);

			if(x < static_cast<int>(icon_sz.width))
				x = static_cast<int>(icon_sz.width);

			unsigned omitted_pixels = graph.width() - icon_sz.width;
			if(ts.width)
			{
				nana::paint::text_renderer tr(graph);
				if(enabled)
				{
					if(attr_.state == nana::gui::button::state::pressed)
					{
						++x;
						++y;
					}
					color_t fgcolor = (attr_.focus_color ? (attr_.focused ? 0xFF : attr_.fgcolor) : attr_.fgcolor);
					if(attr_.omitted)
						tr.render(x, y, fgcolor, str.c_str(), str.size(), omitted_pixels, true);
					else
						graph.bidi_string(x, y, fgcolor, str.c_str(), str.size());

					if(shortkey)
					{
						unsigned off_w = (shortkey_pos ? graph.text_extent_size(str, static_cast<unsigned>(shortkey_pos)).width : 0);
						nana::size shortkey_size = graph.text_extent_size(str.c_str() + shortkey_pos, 1);
						x += off_w;
						y += shortkey_size.height;
						graph.line(x, y, x + shortkey_size.width - 1, y, 0x0);
					}
				}
				else
				{
					if(attr_.omitted)
					{
						tr.render(x + 1, y + 1, 0xFFFFFF, str.c_str(), str.size(), omitted_pixels, true);
						tr.render(x, y, 0x808080, str.c_str(), str.size(), omitted_pixels, true);
					}
					else
					{
						graph.bidi_string(x + 1, y + 1, 0xFFFFFF, str.c_str(), str.size());
						graph.bidi_string(x, y, 0x808080, str.c_str(), str.size());
					}
				}
			}

			if(attr_.icon)
				attr_.icon->paste(graph, 3, (graph.height() - icon_sz.height) / 2);
		}

		void trigger::_m_draw(graph_reference graph)
		{
			window wd = widget_->handle();
			bool eb = API::window_enabled(wd);
			bool def_bground = true;
			attr_.bgcolor = API::background(wd);
			attr_.fgcolor = API::foreground(wd);
			if(bgimage_)
			{
				std::size_t which = (eb ? attr_.state : nana::gui::button::state::disabled);
				image_block & block = bgimage_->block[bgimage_->block[which].who];
				if(block.enable)
				{
					def_bground = false;
					if(bgimage_->stretch.arrange == nana::arrange::horizontal && (bgimage_->stretch.beg < bgimage_->stretch.end))
					{
						unsigned img_beg_width = static_cast<unsigned>(bgimage_->stretch.beg);
						unsigned img_mid_width = static_cast<unsigned>(bgimage_->stretch.end - bgimage_->stretch.beg);
						unsigned img_end_width = bgimage_->block_size.width - static_cast<unsigned>(bgimage_->stretch.end);
						unsigned height = bgimage_->block_size.height;

						if(bgimage_->stretch.beg)
							bgimage_->image.paste(graph, 0, 0, img_beg_width, height, block.pos.x, block.pos.y);

						unsigned width = graph.width() - (img_beg_width + img_end_width);
						bgimage_->image.paste(graph, bgimage_->stretch.beg, 0, width, height, block.pos.x + bgimage_->stretch.beg, block.pos.y, img_mid_width, height);
						if(bgimage_->stretch.end)
							bgimage_->image.paste(graph, graph.width() - img_end_width, 0, img_end_width, height, bgimage_->stretch.end, block.pos.y);
					}
					else if((bgimage_->stretch.arrange == nana::arrange::horizontal_vertical) && (bgimage_->stretch.beg >= bgimage_->stretch.end))
					{
						bgimage_->image.paste(graph, 0, 0, graph.width(), graph.height(), block.pos.x, block.pos.y, bgimage_->block_size.width, bgimage_->block_size.height);
					}
					else
					{
						if(graph.width() > bgimage_->block_size.width || graph.height() > bgimage_->block_size.height)
						{
							_m_draw_background(graph);
							_m_draw_border(graph);
						}
						bgimage_->image.paste(graph, 0, 0, bgimage_->block_size.width, bgimage_->block_size.height, block.pos.x, block.pos.y);
					}
				}
			}
			if(def_bground)
			{
				_m_draw_background(graph);
				_m_draw_border(graph);
			}
			_m_draw_title(graph, eb);
		}

		void trigger::_m_draw_background(graph_reference graph)
		{
			int x = 1, y = 1;
			unsigned color_start = color::button_face_shadow_start, color_end = gui::color::button_face_shadow_end;
			if(attr_.state == nana::gui::button::state::pressed)
			{
				x = 2;
				y = 2;
				color_start = gui::color::button_face_shadow_end;
				color_end = gui::color::button_face_shadow_start;
			}

			graph.shadow_rectangle(x, y, graph.width() - 2, graph.height() - 2, color_start, color_end, true);
		}

		void trigger::_m_draw_border(graph_reference graph)
		{
			int right = graph.width() - 1;
			int bottom = graph.height() - 1;
			graph.line(1, 0, right - 1, 0, 0x7F7F7F);
			graph.line(1, bottom, right - 1, bottom, 0x707070);
			graph.line(0, 1, 0, bottom - 1, 0x7F7F7F);
			graph.line(right, 1, right, bottom - 1, 0x707070);

			graph.set_pixel(1, 1, 0x919191);
			graph.set_pixel(right - 1, 1, 0x919191);
			graph.set_pixel(right - 1, bottom - 1, 0x919191);
			graph.set_pixel(1, bottom - 1, 0x919191);

			graph.set_pixel(0, 0, gui::color::button_face);
			graph.set_pixel(right, 0, gui::color::button_face);
			graph.set_pixel(0, bottom, gui::color::button_face);
			graph.set_pixel(right, bottom, gui::color::button_face);


			if(attr_.state == nana::gui::button::state::pressed)
				graph.rectangle(1, 1, graph.width() - 2, graph.height() - 2, 0xC3C3C3, false);
		}

		void trigger::icon(const nana::paint::image& img)
		{
			if(img.empty())	return;

			if(0 == attr_.icon)
				attr_.icon = new paint::image;
			*attr_.icon = img;
		}

		void trigger::image(const nana::paint::image& img)
		{
			delete bgimage_;
			bgimage_ = 0;

			if(img.empty() == false)
			{
				bgimage_ = new bgimage_tag;
				bgimage_->image = img;
				bgimage_->set_valid_area(nana::arrange::horizontal, nana::rectangle(0, 0, img.size().width, img.size().height));
			}
		}

		trigger::bgimage_tag * trigger::ref_bgimage()
		{
			return bgimage_;
		}
		//end class trigger
	}//end namespace button
}//end namespace drawerbase

		//button
		//@brief:Defaine a button widget and it provides the interfaces to be operational
			button::button(){}

			button::button(window wd, bool visible)
			{
				create(wd, rectangle(), visible);
			}

			button::button(window wd, const rectangle& r, bool visible)
			{
				create(wd, r, visible);
			}

			void button::icon(const nana::paint::image& img)
			{
				internal_scope_guard isg;
				get_drawer_trigger().icon(img);
				API::refresh_window(handle());
			}

			void button::image(const nana::char_t * filename)
			{
				nana::paint::image img;
				if(img.open(filename))
				{
					internal_scope_guard isg;
					base_type::get_drawer_trigger().image(img);
					API::refresh_window(this->handle());
				}
			}

			void button::image(const nana::paint::image& img)
			{
				internal_scope_guard isg;
				base_type::get_drawer_trigger().image(img);
				API::refresh_window(this->handle());
			}

			void button::image_enable(button::state::t sta, bool eb)
			{
				internal_scope_guard isg;
				drawerbase::button::trigger::bgimage_tag * bgi = get_drawer_trigger().ref_bgimage();
				if(bgi && bgi->enable(sta, eb))
					API::refresh_window(this->handle());
			}

			void button::image_valid_area(nana::arrange arg, const nana::rectangle& r)
			{
				internal_scope_guard isg;
				drawerbase::button::trigger::bgimage_tag * bgi = get_drawer_trigger().ref_bgimage();
				if(bgi)
				{
					bgi->set_valid_area(arg, r);
					bgi->update_blocks();
					API::refresh_window(this->handle());
				}
			}

			void button::image_join(button::state::t target, button::state::t from)
			{
				internal_scope_guard isg;
				drawerbase::button::trigger::bgimage_tag * bgi = get_drawer_trigger().ref_bgimage();
				if(bgi && bgi->join(target, from))
					API::refresh_window(this->handle());
			}

			void button::image_stretch(nana::arrange arg, int beg, int end)
			{
				internal_scope_guard isg;
				drawerbase::button::trigger::bgimage_tag * bgi = get_drawer_trigger().ref_bgimage();
				if(bgi)
				{
					bgi->set_stretch(arg, beg, end);
					API::refresh_window(this->handle());
				}
			}

			void button::enable_pushed(bool eb)
			{
				internal_scope_guard isg;
				if(get_drawer_trigger().enable_pushed(eb))
					API::refresh_window(this->handle());
			}

			bool button::pushed() const
			{
				return get_drawer_trigger().pushed();
			}

			void button::pushed(bool psd)
			{
				internal_scope_guard isg;
				if(get_drawer_trigger().pushed(psd))
					API::refresh_window(handle());
			}

			void button::omitted(bool om)
			{
				internal_scope_guard isg;
				get_drawer_trigger().omitted(om);
				API::refresh_window(handle());
			}

			void button::enable_focus_color(bool eb)
			{
				internal_scope_guard isg;
				if(get_drawer_trigger().focus_color(eb))
					API::refresh_window(handle());
			}

			void button::_m_shortkey()
			{
				eventinfo ei;
				API::raise_event<nana::gui::events::click>(this->handle(), ei);
			}

			void button::_m_complete_creation()
			{
				this->make_event<events::shortkey>(*this, &self_type::_m_shortkey);
			}

			void button::_m_caption(const nana::string& text)
			{
				API::unregister_shortkey(this->handle());

				nana::string::value_type shortkey;
				API::transform_shortkey_text(text, shortkey, 0);
				if(shortkey)
					API::register_shortkey(this->handle(), shortkey);

				base_type::_m_caption(text);
			}
		//end class button
}//end namespace gui
}//end namespace nana
