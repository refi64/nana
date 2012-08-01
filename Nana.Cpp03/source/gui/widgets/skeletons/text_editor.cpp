#include <nana/gui/widgets/skeletons/text_editor.hpp>
#include <nana/system/dataexch.hpp>
#include <nana/unicode_bidi.hpp>

namespace nana{	namespace gui{	namespace widgets
{
	namespace skeletons
	{

		//class text_editor
		text_editor::text_editor(nana::gui::window wd, graph_reference graph)
			:window_(wd), graph_(graph), mask_char_(0)
		{
			text_area_.area.width = graph.width();
			text_area_.area.height = graph.height();
			text_area_.captured = false;
			text_area_.cursor.bind(wd);
			text_area_.tab_space = 4;
			text_area_.hscroll = text_area_.vscroll = 0;
			select_.mode_selection = selection::mode_no_selected;
			select_.dragged = false;

			API::create_caret(wd, 1, line_height());
			API::background(wd, 0xFFFFFF);
			API::foreground(wd, 0x000000);
		}

		void text_editor::border_renderer(nana::functor<void(nana::paint::graphics&)> fn)
		{
			text_area_.border_renderer = fn;
		}

		bool text_editor::text_area(const nana::rectangle& r)
		{
			if(text_area_.area == r)
				return false;

			text_area_.area = r;
			if(attributes_.enable_counterpart)
				attributes_.counterpart.make(r.width, r.height);

			_m_scrollbar();
			return true;
		}

		bool text_editor::tip_string(const nana::string& str)
		{
			if(attributes_.tip_string == str)
				return false;

			attributes_.tip_string = str;
			return true;
		}

		bool text_editor::multi_lines(bool ml)
		{
			if((ml == false) && attributes_.multi_lines)
			{
				//retain the first line and remove the extra lines
				if(textbase_.lines() > 1)
				{
					for(std::size_t i = textbase_.lines() - 1; i > 0; --i)
						textbase_.erase(i);
					this->_m_reset();
				}
			}

			if(attributes_.multi_lines != ml)
			{
				attributes_.multi_lines = ml;
				_m_scrollbar();
				return true;
			}
			return false;
		}

		bool text_editor::multi_lines() const
		{
			return attributes_.multi_lines;
		}

		bool text_editor::editable() const
		{
			return attributes_.editable;
		}

		void text_editor::editable(bool v)
		{
			attributes_.editable = v;
		}

		void text_editor::enable_background(bool enb)
		{
			attributes_.enable_background = enb;
		}

		bool text_editor::enable_background() const
		{
			return attributes_.enable_background;
		}

		void text_editor::enable_background_counterpart(bool enb)
		{
			attributes_.enable_counterpart = enb;
			if(enb)
				attributes_.counterpart.make(text_area_.area.width, text_area_.area.height);
			else
				attributes_.counterpart.release();
		}

		text_editor::ext_renderer_tag& text_editor::ext_renderer() const
		{
			return ext_renderer_;
		}

		unsigned text_editor::line_height() const
		{
			return (graph_ ? (graph_.text_extent_size(STR("jH{")).height) : 0);
		}

		unsigned text_editor::screen_lines() const
		{
			if(graph_ && (text_area_.area.height > text_area_.hscroll))
				return (text_area_.area.height - text_area_.hscroll) / line_height();
			return 0;
		}

		bool text_editor::mouse_enter(bool enter)
		{
			if((false == enter) && (false == text_area_.captured))
				text_area_.cursor.load(cursor::predef::arrow);

			if(API::focus_window() != window_)
			{
				this->redraw(false);
				return true;
			}
			return false;
		}

		bool text_editor::mouse_down(bool left_button, int screen_x, int screen_y)
		{
			if(hit_text_area(screen_x, screen_y))
			{
				if(left_button)
				{
					//Set caret pos by screen point and get the caret pos.
					nana::upoint pos = mouse_caret(screen_x, screen_y);
					API::capture_window(window_, true);
					text_area_.captured = true;
					if(hit_select_area(pos))
					{
						select_.mode_selection = selection::mode_no_selected;
					}
					else
					{
						if(cancel_select() == false)
						{
							select_.a = points_.caret;	//Set begin caret
							set_end_caret();
						}
						select_.mode_selection = selection::mode_mouse_selected;
					}
				}
				text_area_.border_renderer(graph_);
				return true;
			}
			return false;
		}

		bool text_editor::mouse_move(bool left_button, int screen_x, int screen_y)
		{
			if(false == hit_text_area(screen_x, screen_y))
				text_area_.cursor.load(text_area_.captured ? cursor::predef::iterm : cursor::predef::arrow);
			else
				text_area_.cursor.load(cursor::predef::iterm);

			if(left_button)
			{
				nana::upoint pos = caret();
				mouse_caret(screen_x, screen_y);

				if(select_.mode_selection != selection::mode_no_selected)
					set_end_caret();
				else if(select_.dragged == false && pos != caret())
					select_.dragged = true;

				text_area_.border_renderer(graph_);
				return true;
			}
			return false;
		}

		bool text_editor::mouse_up(bool left_button, int screen_x, int screen_y)
		{
			bool do_draw = false;
			if(select_.mode_selection == selection::mode_mouse_selected)
			{
				select_.mode_selection = selection::mode_no_selected;
				set_end_caret();
			}
			else if(select_.mode_selection == selection::mode_no_selected)
			{
				if(select_.dragged == false || move_select() == false)
					cancel_select();
				do_draw = true;
			}
			select_.dragged = false;

			API::capture_window(window_, false);
			text_area_.captured = false;
			if(hit_text_area(screen_x, screen_y) == false)
				text_area_.cursor.load(cursor::predef::arrow);

			text_area_.border_renderer(graph_);
			return do_draw;
		}

		std::size_t text_editor::text_lines() const
		{
			return textbase_.lines();
		}

		bool text_editor::getline(std::size_t n, nana::string& text) const
		{
			if(n < textbase_.lines())
			{
				text = textbase_.getline(n);
				return true;
			}
			return false;
		}

		void text_editor::setline(std::size_t n, const nana::string& text)
		{
			bool mkdraw = false;
			textbase_.cover(n, text.c_str());

			if((points_.caret.y == n) && (text.size() < points_.caret.x))
			{
				points_.caret.x = static_cast<unsigned>(text.size());
				mkdraw = this->_m_adjust_caret_into_screen();
			}

			if(!mkdraw && (static_cast<size_t>(points_.offset.y) <= n) && (n < static_cast<size_t>(points_.offset.y + screen_lines())))
				mkdraw = true;

			if(mkdraw)
				this->redraw(API::focus_window() == window_);
		}

		void text_editor::text(const nana::string& str)
		{
			textbase_.erase_all();
			_m_reset();
			this->put(str);
		}

		nana::string text_editor::text() const
		{
			nana::string str;
			std::size_t lines = textbase_.lines();
			if(lines > 0)
			{
				str += textbase_.getline(0);
				for(std::size_t i = 1; i < lines; ++i)
				{
					str += STR("\n\r");
					str += textbase_.getline(i);
				}
			}
			return str;
		}

		//move_caret
		//Set caret position through text coordinate
		void text_editor::move_caret(size_type x, size_type y)
		{
			if(API::is_focus_window(window_))
			{
				if(y > textbase_.lines())	y = textbase_.lines();

				x = _m_pixels_by_char(y, x) + text_area_.area.x;

				const unsigned line_pixels = line_height();

				int pos_y = static_cast<int>((y - points_.offset.y) * line_pixels + text_area_.area.y);
				int end_y = pos_y + static_cast<int>(line_pixels);
				int pos_x = static_cast<int>(x - points_.offset.x);
				bool visible = true;

				if(pos_x < static_cast<int>(text_area_.area.x) || _m_endx() < pos_x)
					visible = false;
				else if(end_y <= 0 || pos_y >= _m_endy())
					visible = false;
				else if(end_y > _m_endy())
					API::caret_size(window_, nana::size(1, line_pixels - (end_y - _m_endy())));
				else if(API::caret_size(window_).height != line_pixels)
					reset_caret_height();

				if(visible == false && API::caret_visible(window_))
					API::caret_visible(window_, false);
				else if(visible && (API::caret_visible(window_) == false))
					API::caret_visible(window_, true);

				if(visible)
					API::caret_pos(window_, pos_x, pos_y);
			}
		}

		void text_editor::move_caret_end()
		{
			points_.caret.y = static_cast<unsigned>(textbase_.lines());
			if(points_.caret.y) --points_.caret.y;
			points_.caret.x = static_cast<unsigned>(textbase_.getline(points_.caret.y).length());
		}

		void text_editor::reset_caret_height() const
		{
			API::caret_size(this->window_, nana::size(1, this->line_height()));
		}

		void text_editor::reset_caret()
		{
			this->move_caret(points_.caret.x, points_.caret.y);
		}

		void text_editor::show_caret(bool isshow)
		{
			if(isshow == false || API::is_focus_window(window_))
				API::caret_visible(window_, isshow);
		}

		bool text_editor::selected() const
		{
			return (select_.a != select_.b);
		}

		void text_editor::set_end_caret()
		{
			bool new_sel_end = (select_.b != points_.caret);
			select_.b = points_.caret;
			points_.xpos = points_.caret.x;

			if(new_sel_end || _m_adjust_caret_into_screen())
				redraw(true);
		}

		void text_editor::select_all()
		{
			select_.a.x = select_.a.y = 0;
			select_.b.y = static_cast<unsigned>(textbase_.lines());
			if(select_.b.y) --select_.b.y;
			select_.b.x = static_cast<unsigned>(textbase_.getline(select_.b.y).length());
			select_.mode_selection = selection::mode_method_selected;
		}

		bool text_editor::cancel_select()
		{
			select_.mode_selection = selection::mode_no_selected;
			if(_m_cancel_select(0))
			{
				redraw(true);
				return true;
			}
			return false;
		}

		bool text_editor::hit_text_area(int x, int y) const
		{
			return ((text_area_.area.x <= x && x < _m_endx()) && (text_area_.area.y <= y && y < _m_endy()));
		}

		bool text_editor::hit_select_area(nana::upoint pos) const
		{
			nana::upoint a, b;
			this->_m_get_sort_select_points(a, b);
			if(a != b)
			{
				if((pos.y > a.y || (pos.y == a.y && pos.x >= a.x)) && ((pos.y < b.y) || (pos.y == b.y && pos.x < b.x)))
					return true;
			}
			return false;
		}

		bool text_editor::move_select()
		{
			nana::upoint a, b;
			this->_m_get_sort_select_points(a, b);

			if(this->hit_select_area(points_.caret) || (select_.b == points_.caret))
			{
				points_.caret = select_.b;
				if(this->_m_adjust_caret_into_screen())
					redraw(true);
				this->reset_caret();

				return true;
			}

			nana::upoint caret = points_.caret;

			nana::string text;
			if(this->_m_make_select_string(text))
			{
				if(caret.y < a.y || (caret.y == a.y && caret.x < a.x))
				{//forward
					this->_m_erase_select();
					this->_m_put(text);

					select_.a = caret;
					select_.b.y = b.y + (caret.y - a.y);
				}
				else if(b.y < caret.y || (caret.y == b.y && b.x < caret.x))
				{
					this->_m_put(text);
					this->_m_erase_select();

					select_.b.y = caret.y;
					select_.a.y = caret.y - (b.y - a.y);
					select_.a.x = caret.x - (caret.y == b.y ? (b.x - a.x) : 0);
				}
				select_.b.x = b.x + (a.y == b.y ? (select_.a.x - a.x) : 0);

				points_.caret = select_.a;
				reset_caret();
				_m_adjust_caret_into_screen();
				redraw(true);
				return true;
			}
			return false;
		}

		bool text_editor::mask(nana::char_t ch)
		{
			if(mask_char_ != ch)
			{
				mask_char_ = ch;
				return true;
			}
			return false;
		}
	//public:
		void text_editor::draw_scroll_rectangle()
		{
			if(text_area_.vscroll && text_area_.hscroll)
			{
				graph_.rectangle( text_area_.area.x + static_cast<int>(text_area_.area.width - text_area_.vscroll),
									text_area_.area.y + static_cast<int>(text_area_.area.height - text_area_.hscroll),
									text_area_.vscroll, text_area_.hscroll, nana::gui::color::button_face, true);
			}
		}

		void text_editor::redraw(bool has_focus)
		{
			nana::color_t bgcolor = API::background(window_);
			nana::color_t fgcolor = API::foreground(window_);
			//Draw background
			if(attributes_.enable_background)
				graph_.rectangle(text_area_.area, bgcolor, true);

			if(ext_renderer_.background)
				ext_renderer_.background(graph_, text_area_.area, bgcolor);

			if(attributes_.counterpart && text_area_.area.width && text_area_.area.height)
				attributes_.counterpart.bitblt(0, 0, text_area_.area.width, text_area_.area.height, graph_, text_area_.area.x, text_area_.area.y);

			int y = text_area_.area.y;

			if((false == textbase_.empty()) || has_focus)
			{
				const unsigned line_hg = this->line_height();

				nana::upoint a, b;
				_m_get_sort_select_points(a, b);
				//unsigned whitespace_w = graph_.text_extent_size(STR(" ")).width;

				for(unsigned ln = points_.offset.y; ln < textbase_.lines(); ++ln, y += line_hg)
				{
					_m_draw_string(y, fgcolor, ln, true);
				}
			}
			else
				_m_draw_tip_string();

			draw_scroll_rectangle();
			text_area_.border_renderer(graph_);
		}
	//public:
		void text_editor::put(const nana::string& text)
		{
			points_.caret = _m_erase_select();
			points_.caret = this->_m_put(text);
			if(graph_)
			{
				_m_adjust_caret_into_screen();
				reset_caret();
				redraw(API::is_focus_window(window_));
				_m_scrollbar();

				points_.xpos = points_.caret.x;
			}
		}

		void text_editor::put(nana::char_t c)
		{
			bool refresh = (select_.a != select_.b);
			if(refresh)
				points_.caret = _m_erase_select();

			textbase_.insert(points_.caret.y, points_.caret.x, c);
			points_.caret.x ++;

			if(refresh || _m_draw(c))
				redraw(true);
			else
				draw_scroll_rectangle();

			_m_scrollbar();

			points_.xpos = points_.caret.x;
		}

		void text_editor::copy() const
		{
			nana::string str;
			if(this->_m_make_select_string(str))
			{
				nana::system::dataexch de;
				de.set(str);
			}
		}

		void text_editor::paste()
		{
			points_.caret = _m_erase_select();

			nana::system::dataexch de;
			nana::string text;
			de.get(text);
			this->put(text);
		}

		void text_editor::enter()
		{
			if(attributes_.multi_lines)
			{
				bool need_refresh = false;

				if(select_.a != select_.b)
				{
					points_.caret = _m_erase_select();
					need_refresh = true;
				}

				const string_type& lnstr = textbase_.getline(points_.caret.y);

				if(lnstr.size() > points_.caret.x)
				{
					textbase_.insertln(points_.caret.y + 1, lnstr.c_str() + points_.caret.x);
					textbase_.erase(points_.caret.y, points_.caret.x, lnstr.size() - points_.caret.x);
				}
				else
				{
					if(textbase_.lines() == 0) textbase_.insertln(0, STR(""));
					textbase_.insertln(points_.caret.y + 1, STR(""));
				}

				points_.caret.x = 0;
				points_.caret.y++;

				if(points_.offset.x || points_.caret.y < textbase_.lines() || textbase_.getline(points_.caret.y).size())
				{
					points_.offset.x = 0;
					need_refresh = true;
				}

				if(this->_m_adjust_caret_into_screen() || need_refresh)
					redraw(true);

				_m_scrollbar();
			}
		}

		void text_editor::del()
		{
			bool has_erase = true;

			if(select_.a == select_.b)
			{
				if(textbase_.getline(points_.caret.y).size() > points_.caret.x)
				{
					points_.caret.x++;
				}
				else if(textbase_.lines() && (points_.caret.y < textbase_.lines() - 1))
				{	//Move to next line
					points_.caret.x = 0;
					++ points_.caret.y;
				}
				else
					has_erase = false;	//No characters behind the caret
			}

			if(has_erase)	this->backspace();

			_m_scrollbar();
			points_.xpos = points_.caret.x;
		}

		void text_editor::backspace()
		{
			bool has_to_redraw = true;
			if(select_.a == select_.b)
			{
				if(points_.caret.x)
				{
					unsigned erase_number = 1;
					--points_.caret.x;
#ifndef NANA_UNICODE
					const string_type& lnstr = textbase_.getline(points_.caret.y);
					if(is_incomplete(lnstr, points_.caret.x) && (points_.caret.x))
					{
						textbase_.erase(points_.caret.y, points_.caret.x, 1);
						--points_.caret.x;
						erase_number = 2;
					}
#endif
					textbase_.erase(points_.caret.y, points_.caret.x, erase_number);
					if(this->_m_move_offset_x_while_over_border(-2) == false)
					{
						_m_update_line(points_.caret.y);
						this->draw_scroll_rectangle();
						has_to_redraw = false;
					}
				}
				else if(points_.caret.y)
				{
					points_.caret.x = static_cast<unsigned>(textbase_.getline(-- points_.caret.y).size());
					textbase_.merge(points_.caret.y);
				}
			}
			else
				points_.caret = _m_erase_select();

			if(has_to_redraw)
			{
				_m_adjust_caret_into_screen();
				redraw(true);
			}
			_m_scrollbar();
		}

		bool text_editor::move(nana::char_t key)
		{
			switch(key)
			{
			case keyboard::left:	move_left();	break;
			case keyboard::right:	move_right();	break;
			case keyboard::up:		move_up();		break;
			case keyboard::down:	move_down();	break;
			case keyboard::del:		del();	break;
			default:
				return false;
			}
			return true;
		}

		void text_editor::move_up()
		{
			bool need_redraw = _m_cancel_select(0);
			if(points_.caret.y)
			{
				points_.caret.x = static_cast<unsigned>(textbase_.getline(--points_.caret.y).size());

				if(points_.xpos < points_.caret.x)
					points_.caret.x = points_.xpos;

				if((static_cast<int>(points_.caret.y) < points_.offset.y) && (need_redraw == false))
					need_redraw = true;

				if(static_cast<unsigned>(points_.offset.y) > points_.caret.y)
					this->_m_offset_y(static_cast<int>(points_.caret.y));

				if(this->_m_adjust_caret_into_screen() && (need_redraw == false))
					need_redraw = true;
			}

			if(need_redraw)	redraw(true);
			_m_scrollbar();
		}

		void text_editor::move_down()
		{
			bool need_redraw = _m_cancel_select(0);
			if(points_.caret.y + 1 < textbase_.lines())
			{
				points_.caret.x = static_cast<unsigned>(textbase_.getline(++points_.caret.y).size());

				if(points_.xpos < points_.caret.x)
					points_.caret.x = points_.xpos;

				if(this->_m_adjust_caret_into_screen() && (need_redraw == false))
					need_redraw = true;
			}
			if(need_redraw)	redraw(true);
			_m_scrollbar();
		}

		void text_editor::move_left()
		{
			if(_m_cancel_select(1) == false)
			{
				if(points_.caret.x)
				{
					--points_.caret.x;
#ifndef NANA_UNICODE
					if(is_incomplete(textbase_.getline(points_.caret.y), points_.caret.x))
						--points_.caret.x;
#endif
					if(this->_m_move_offset_x_while_over_border(-2))
						redraw(true);
				}
				else if(points_.caret.y)
				{	//Move to previous line
					points_.caret.x = static_cast<unsigned>(textbase_.getline(-- points_.caret.y).size());
					if(this->_m_adjust_caret_into_screen())
						redraw(true);
				}
			}
			else
			{
				_m_adjust_caret_into_screen();
				redraw(true);
			}

			_m_scrollbar();
			points_.xpos = points_.caret.x;
		}

		void text_editor::move_right()
		{
			if(_m_cancel_select(2) == false)
			{
				nana::string lnstr = textbase_.getline(points_.caret.y);
				if(lnstr.size() > points_.caret.x)
				{
					++points_.caret.x;
#ifndef NANA_UNICODE
					if(is_incomplete(lnstr, points_.caret.x))
						++points_.caret.x;
#endif
					if(this->_m_move_offset_x_while_over_border(2))
						redraw(true);
				}
				else if(textbase_.lines() && (points_.caret.y < textbase_.lines() - 1))
				{	//Move to next line
					points_.caret.x = 0;
					++ points_.caret.y;

					if(this->_m_adjust_caret_into_screen())
						redraw(true);
				}
			}
			else
			{
				_m_adjust_caret_into_screen();
				redraw(true);
			}

			_m_scrollbar();
			points_.xpos = points_.caret.x;
		}

		nana::upoint text_editor::mouse_caret(int screen_x, int screen_y)
		{
			points_.caret = this->_m_screen_to_caret(screen_x, screen_y);
			if(this->_m_adjust_caret_into_screen())
				redraw(true);

			this->move_caret(points_.caret.x, points_.caret.y);
			return points_.caret;
		}

		nana::upoint text_editor::caret() const
		{
			return points_.caret;
		}

		bool text_editor::scroll(bool upwards, bool vertical)
		{
			if(vertical && attributes_.vscroll)
			{
				attributes_.vscroll->make_step(!upwards);

				if(_m_scroll_text(true))
				{
					redraw(true);
					return true;
				}
			}
			return false;
		}

	//private:
		bool text_editor::_m_scroll_text(bool vertical)
		{
			if(attributes_.vscroll && vertical)
			{
				if(static_cast<int>(attributes_.vscroll->value()) != points_.offset.y)
				{
					_m_offset_y(static_cast<int>(attributes_.vscroll->value()));
					return true;
				}
			}
			else if(attributes_.hscroll && (vertical == false))
			{
				if(attributes_.hscroll->value() != points_.offset.x)
				{
					points_.offset.x = static_cast<int>(attributes_.hscroll->value());
					return true;
				}
			}
			return false;
		}

		void text_editor::_m_on_scroll(const nana::gui::eventinfo& ei)
		{
			if(nana::gui::events::mouse_move::identifier == ei.identifier && (ei.mouse.left_button == false))
				return;

			bool vertical;
			if(attributes_.vscroll && (attributes_.vscroll->handle() == ei.window))
				vertical = true;
			else if(attributes_.hscroll && (attributes_.hscroll->handle() == ei.window))
				vertical = false;
			else
				return;

			if(_m_scroll_text(vertical))
			{
				redraw(true);
				reset_caret();
				nana::gui::API::update_window(this->window_);
			}
		}

		void text_editor::_m_scrollbar()
		{
			this->_m_get_scrollbar_size();

			if(this->text_area_.vscroll)
			{
				nana::gui::scroll<true>* scroll = this->attributes_.vscroll;
				int x = text_area_.area.x + static_cast<int>(text_area_.area.width - text_area_.vscroll);
				if(scroll == 0)
				{
					using namespace nana::gui;
					scroll = new nana::gui::scroll<true>;
					scroll->create(window_, rectangle(x, text_area_.area.y, text_area_.vscroll, _m_get_text_area_height()));
					scroll->make_event<events::mouse_down>(*this, &text_editor::_m_on_scroll);
					scroll->make_event<events::mouse_move>(*this, &text_editor::_m_on_scroll);
					scroll->make_event<events::mouse_wheel>(*this, &text_editor::_m_on_scroll);
					this->attributes_.vscroll = scroll;
					API::take_active(scroll->handle(), false, window_);
				}

				if(textbase_.lines() != scroll->amount())
					scroll->amount(static_cast<int>(textbase_.lines()));

				if(this->screen_lines() != scroll->range())
					scroll->range(this->screen_lines());

				if(this->points_.offset.y != static_cast<int>(scroll->value()))
					scroll->value(this->points_.offset.y);

				scroll->move(x, text_area_.area.y, text_area_.vscroll, _m_get_text_area_height());
			}
			else if(this->attributes_.vscroll)
			{
				nana::gui::scroll<true>* scroll = this->attributes_.vscroll;
				this->attributes_.vscroll = 0;
				delete scroll;
			}

			//HScroll
			if(this->text_area_.hscroll)
			{
				nana::gui::scroll<false>* scroll = this->attributes_.hscroll;
				unsigned text_area_width = _m_get_text_area_width();
				int y = text_area_.area.y + static_cast<int>(text_area_.area.height - text_area_.hscroll);
				if(scroll == 0)
				{
					using namespace nana::gui;
					scroll = new nana::gui::scroll<false>;
					scroll->create(window_, rectangle(text_area_.area.x, y, text_area_width, this->text_area_.hscroll));
					scroll->make_event<events::mouse_down>(*this, &text_editor::_m_on_scroll);
					scroll->make_event<events::mouse_move>(*this, &text_editor::_m_on_scroll);
					scroll->make_event<events::mouse_wheel>(*this, &text_editor::_m_on_scroll);
					scroll->step(20);
					this->attributes_.hscroll = scroll;
					API::take_active(scroll->handle(), false, window_);
				}

				nana::size text_size = this->_m_text_extent_size(textbase_.getline(textbase_.max_line().first).c_str(), textbase_.max_line().second);

				text_size.width += 1;
				if(text_size.width > scroll->amount())
					scroll->amount(text_size.width);

				if(text_area_width != scroll->range())	scroll->range(text_area_width);
				if(this->points_.offset.x != scroll->value())
					scroll->value(this->points_.offset.x);

				scroll->move(text_area_.area.x, y, text_area_width, text_area_.hscroll);

			}
			else if(this->attributes_.hscroll)
			{
				nana::gui::scroll<false>* scroll = this->attributes_.hscroll;
				this->attributes_.hscroll = 0;
				delete scroll;
			}
		}

		unsigned text_editor::_m_get_text_area_width() const
		{
			return (text_area_.area.width > text_area_.vscroll ? text_area_.area.width - text_area_.vscroll : 0);
		}

		unsigned text_editor::_m_get_text_area_height() const
		{
			return (text_area_.area.height > text_area_.hscroll ? text_area_.area.height - text_area_.hscroll : 0);
		}

		void text_editor::_m_get_scrollbar_size()
		{
			text_area_.hscroll = 0;

			//Only the textbox is multi_lines, it enables the scrollbars
			if(this->attributes_.multi_lines)
			{
				text_area_.vscroll = (textbase_.lines() > this->screen_lines() ? 16 : 0);

				std::pair<size_t, size_t> max_line = textbase_.max_line();
				if(max_line.second)
				{
					if(this->points_.offset.x || _m_text_extent_size(textbase_.getline(max_line.first).c_str(), max_line.second).width > _m_get_text_area_width())
					{
						text_area_.hscroll = 16;
						if((text_area_.vscroll == 0) && (textbase_.lines() > this->screen_lines()))
							text_area_.vscroll = 16;
					}
				}
			}
			else
				text_area_.vscroll = 0;
		}

		void text_editor::_m_reset()
		{
			points_.caret.x = points_.caret.y = 0;
			points_.offset.x = 0;
			this->_m_offset_y(0);
			select_.a = select_.b;
		}

		nana::upoint text_editor::_m_put(nana::string text)
		{
			std::size_t lines = _m_make_simple_nl(text);

			nana::upoint caret = points_.caret;
			if(lines > 1)
			{
				nana::string orig_str = textbase_.getline(caret.y);
				unsigned orig_x = caret.x;

				nana::string::size_type beg = 0, end = text.find('\n');
				if(attributes_.multi_lines)
				{
					if(orig_str.size() == orig_x)
						textbase_.insert(caret.y, caret.x, text.substr(beg, end - beg).c_str());
					else
						textbase_.cover(caret.y, (orig_str.substr(0, orig_x) + text.substr(beg, end - beg)).c_str());

					std::size_t n = 2;
					++caret.y;
					beg = end + 1;

					end = text.find('\n', beg);

					while(end != nana::string::npos)
					{
						if(n != lines)
							textbase_.insertln(caret.y, text.substr(beg, end - beg).c_str());

						beg = end + 1;
						caret.y++;
						++n;

						end = text.find('\n', beg);
					}

					textbase_.insertln(caret.y, (text.substr(beg) + orig_str.substr(orig_x)).c_str());
					caret.x = static_cast<unsigned>(text.size() - beg);
				}
				else
				{
					nana::string newstr = text.substr(beg, end);
					textbase_.insert(caret.y, caret.x, newstr.c_str());
					caret.x += static_cast<unsigned>(newstr.size());
				}
			}
			else
			{
				textbase_.insert(caret.y, caret.x, text.c_str());
				caret.x += static_cast<unsigned>(text.size());
			}
			return caret;
		}

		nana::upoint text_editor::_m_erase_select()
		{
			nana::upoint a, b;
			_m_get_sort_select_points(a, b);
			if(a != b)
			{
				if(a.y != b.y)
				{
					textbase_.erase(a.y, a.x, 0xFFFFFFFF);

					for(unsigned ln = a.y + 1; ln < b.y; ++ln)
						textbase_.erase(a.y + 1);

					textbase_.erase(a.y + 1, 0, b.x);
					textbase_.merge(a.y);
				}
				else
					textbase_.erase(a.y, a.x, b.x - a.x);

				select_.a = select_.b;
				return a;
			}

			return points_.caret;
		}

		bool text_editor::_m_make_select_string(nana::string& text) const
		{
			nana::upoint a, b;
			this->_m_get_sort_select_points(a, b);
			if(a != b)
			{
				if(a.y != b.y)
				{
					text = textbase_.getline(a.y).substr(a.x);
					text += STR("\r\n");
					for(unsigned i = a.y + 1; i < b.y; ++i)
					{
						text += textbase_.getline(i).substr(0);
						text += STR("\r\n");
					}
					text += textbase_.getline(b.y).substr(0, b.x);
				}
				else
					text = textbase_.getline(a.y).substr(a.x, b.x - a.x);

				return true;
			}
			return false;
		}

		//_m_make_simple_nl
		//@brief: transform a string if it contains "0xD\0xA" or "0xA\0xD"
		std::size_t text_editor::_m_make_simple_nl(nana::string& text)
		{
			std::size_t lines = 1;
			nana::string::size_type beg = 0;

			while(true)
			{
				nana::string::size_type nl = text.find('\n', beg);
				if(nana::string::npos == nl) break;

				if(nl && (text.at(nl - 1) == 0xD))
					text.erase(--nl, 1);
				else if(nl < text.size() - 1 && text.at(nl + 1) == 0xD)
					text.erase(nl + 1, 1);

				beg = nl + 1;
				++lines;
			}

			nana::string::size_type pos = text.find_last_not_of(nana::char_t(0));
			if(pos != text.npos)
				text.erase(pos + 1);
			return lines;
		}

		bool text_editor::_m_cancel_select(int align)
		{
			if(select_.a != select_.b)
			{
				nana::upoint a, b;
				_m_get_sort_select_points(a, b);

				switch(align)
				{
				case 1:
					points_.caret = a;
					this->_m_move_offset_x_while_over_border(-2);
					break;
				case 2:
					points_.caret = b;
					this->_m_move_offset_x_while_over_border(2);
					break;
				}
				select_.a = select_.b = points_.caret;
				this->reset_caret();
				return true;
			}
			return false;
		}

		unsigned text_editor::_m_tabs_pixels(size_type tabs) const
		{
			if(0 == tabs) return 0;

			nana::char_t ws[2] = {};
			ws[0] = mask_char_ ? mask_char_ : ' ';
			return static_cast<unsigned>(tabs * graph_.text_extent_size(ws).width * text_area_.tab_space);
		}

		nana::size text_editor::_m_text_extent_size(const char_type* str) const
		{
			return _m_text_extent_size(str, nana::strlen(str));
		}

		nana::size text_editor::_m_text_extent_size(const char_type* str, size_type n) const
		{
			if(graph_)
			{
				if(mask_char_)
				{
					nana::string maskstr;
					maskstr.append(n, mask_char_);
					return graph_.text_extent_size(maskstr);
				}
				else
					return graph_.text_extent_size(str, static_cast<unsigned>(n));
			}
			return nana::size();
		}

		//_m_move_offset_x_while_over_border
		//@brief: Move the view window
		bool text_editor::_m_move_offset_x_while_over_border(int many)
		{
			const string_type& lnstr = textbase_.getline(points_.caret.y);
			unsigned width = _m_text_extent_size(lnstr.c_str(), points_.caret.x).width;
			if(many < 0)
			{
				many = -many;
				if(points_.offset.x && (points_.offset.x >= static_cast<int>(width)))
				{	//Out of screen text area
					if(points_.caret.x > static_cast<unsigned>(many))
						points_.offset.x = static_cast<int>(width - _m_text_extent_size(lnstr.c_str() + points_.caret.x - many, many).width);
					else
						points_.offset.x = 0;
					return true;
				}
			}
			else if(many)
			{
				width += text_area_.area.x;
				if(static_cast<int>(width) - points_.offset.x >= _m_endx())
				{	//Out of screen text area
					points_.offset.x = width - _m_endx() + 1;
					string_type::size_type rest_size = lnstr.size() - points_.caret.x;
					points_.offset.x += static_cast<int>(_m_text_extent_size(lnstr.c_str() + points_.caret.x, (rest_size >= static_cast<unsigned>(many) ? static_cast<unsigned>(many) : rest_size)).width);
					return true;
				}
			}
			return false;
		}

		//_m_endx
		//@brief: Get the right point of text area
		int text_editor::_m_endx() const
		{
			return static_cast<int>(text_area_.area.x + text_area_.area.width - text_area_.vscroll);
		}

		//_m_endy
		//@brief: Get the bottom point of text area
		int text_editor::_m_endy() const
		{
			return static_cast<int>(text_area_.area.y + text_area_.area.height - text_area_.hscroll);
		}

		void text_editor::_m_draw_tip_string() const
		{
			graph_.string(text_area_.area.x - points_.offset.x, text_area_.area.y, 0x787878, attributes_.tip_string);
		}

		void text_editor::_m_update_line(std::size_t textline) const
		{
			int top = text_area_.area.y + line_height() * textline;
			graph_.rectangle(text_area_.area.x, top, text_area_.area.width, line_height(), API::background(window_), true);
			_m_draw_string(top, API::foreground(window_), textline, true);
		}

		void text_editor::_m_draw_string(int top, unsigned color, std::size_t textline, bool if_mask) const
		{
			const string_type& linestr = textbase_.getline(textline);
			unicode_bidi bidi;
			std::vector<unicode_bidi::entity> reordered;
			bidi.linestr(linestr.c_str(), linestr.size(), reordered);

			int x = text_area_.area.x - points_.offset.x;
			int xend = text_area_.area.x + static_cast<int>(text_area_.area.width);

			if(if_mask && mask_char_)
			{
				std::size_t n = 0;
				for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
					n += (i->end - i->begin);

				nana::string maskstr;
				maskstr.append(n, mask_char_);
				graph_.string(x, top, color, maskstr);
				return;
			}

			unsigned whitespace_w = graph_.text_extent_size(STR(" ")).width;

			const unsigned line_h_pixels = this->line_height();

			//The line of text is in the range of selection
			nana::upoint a, b;
			_m_get_sort_select_points(a, b);

			//The text is not selected or the whole line text is selected
			if((select_.a == select_.b) || (select_.a.y != textline && select_.b.y != textline))
			{
				bool selected = (a.y < textline && textline < b.y);
				for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
				{
					std::size_t len = i->end - i->begin;
					unsigned str_w = graph_.text_extent_size(i->begin, len).width;

					if((x + static_cast<int>(str_w) > text_area_.area.x) && (x < xend))
					{
						if(selected)
						{
							graph_.rectangle(x, top, str_w, line_h_pixels, 0x3399FF, true);
							graph_.string(x, top, 0xFFFFFF, i->begin, len);
						}
						else
							graph_.string(x, top, color, i->begin, len);
					}
					x += static_cast<int>(str_w);
				}
				if(selected)
					graph_.rectangle(x, top, whitespace_w, line_h_pixels, 0x3399FF, true);
			}
			else
			{
				const nana::char_t * strbeg = linestr.c_str();
				if(a.y == b.y)
				{
					for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
					{
						std::size_t len = i->end - i->begin;
						unsigned str_w = graph_.text_extent_size(i->begin, len).width;
						if((x + static_cast<int>(str_w) > text_area_.area.x) && (x < xend))
						{
							std::size_t pos = i->begin - strbeg;

							if(pos + len <= a.x || pos >= b.x)
							{
								//NOT selected
								graph_.string(x, top, color, i->begin, len);
							}
							else if(a.x <= pos && pos + len <= b.x)
							{
								//Whole selected
								graph_.rectangle(x, top, str_w, line_h_pixels, 0x3399FF, true);
								graph_.string(x, top, 0xFFFFFF, i->begin, len);
							}
							else if(pos <= a.x && a.x < pos + len)
							{	//Partial selected
								int endpos = (b.x < pos + len ? b.x : pos + len);
								unsigned * pxbuf = new unsigned[len];
								if(graph_.glyph_pixels(i->begin, len, pxbuf))
								{
									unsigned head_w = 0;
									for(std::size_t u = 0; u < a.x - pos; ++u)
										head_w += pxbuf[u];

									unsigned sel_w = 0;
									for(std::size_t u = a.x - pos; u < endpos - pos; ++u)
										sel_w += pxbuf[u];

									if(_m_is_right_text(*i))
									{	//RTL
										graph_.string(x, top, color, i->begin, len);
										nana::paint::graphics graph(str_w, line_h_pixels);
										graph.typeface(graph_.typeface());
										graph.rectangle(0x3399FF, true);
										graph.string(0, 0, 0xFFFFFF, i->begin, len);

										int sel_xpos = static_cast<int>(str_w - head_w - sel_w);
										graph_.bitblt(x + sel_xpos, top, sel_w, line_h_pixels, graph, sel_xpos, 0);
									}
									else
									{	//LTR
										graph_.string(x, top, color, i->begin, a.x - pos);

										graph_.rectangle(x + head_w, top, sel_w, line_h_pixels, 0x3399FF, true);
										graph_.string(x + head_w, top, 0xFFFFFF, i->begin + (a.x - pos), endpos - a.x);

										if(static_cast<size_t>(endpos) < pos + len)
											graph_.string(x + static_cast<int>(head_w + sel_w), top, color, i->begin + (endpos - pos), pos + len - endpos);
									}
								}
								delete [] pxbuf;
							}
							else if(pos <= b.x && b.x < pos + len)
							{	//Partial selected
								int endpos = b.x;
								unsigned sel_w = graph_.glyph_extent_size(i->begin, len, 0, endpos - pos).width;
								if(_m_is_right_text(*i))
								{	//RTL
									graph_.string(x, top, color, i->begin, len);
									nana::paint::graphics graph(str_w, line_h_pixels);
									graph.typeface(graph_.typeface());
									graph.rectangle(0x3399FF, true);
									graph.string(0, 0, 0xFFFFFF, i->begin, len);
									graph_.bitblt(x + (str_w - sel_w), top, sel_w, line_h_pixels, graph, str_w - sel_w, 0);
								}
								else
								{	//LTR
									graph_.rectangle(x, top, sel_w, line_h_pixels, 0x3399FF, true);
									graph_.string(x, top, 0xFFFFFF, i->begin, endpos - pos);
									graph_.string(x + sel_w, top, color, i->begin + (endpos - pos), pos + len - endpos);
								}

							}
						}
						x += static_cast<int>(str_w);
					}
				}
				else if(a.y == textline)
				{
					for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
					{
						std::size_t len = i->end - i->begin;
						unsigned str_w = graph_.text_extent_size(i->begin, len).width;
						if((x + static_cast<int>(str_w) > text_area_.area.x) && (x < xend))
						{
							std::size_t pos = i->begin - strbeg;
							if(pos + len <= a.x)
							{
								graph_.string(x, top, color, i->begin, len);
							}
							else if(a.x < pos)
							{
								graph_.rectangle(x, top, str_w, line_h_pixels, 0x3399FF, true);
								graph_.string(x, top, 0xFFFFFF, i->begin, len);
							}
							else
							{
								unsigned head_w = graph_.glyph_extent_size(i->begin, len, 0, a.x - pos).width;
								if(_m_is_right_text(*i))
								{	//RTL
									graph_.string(x, top, color, i->begin, len);
									nana::paint::graphics graph(str_w, line_h_pixels);
									graph.typeface(graph_.typeface());
									graph.rectangle(0x3399FF, true);
									graph.string(0, 0, 0xFFFFFF, i->begin, len);
									graph_.bitblt(x, top, str_w - head_w, line_h_pixels, graph, 0, 0);
								}
								else
								{	//LTR
									graph_.string(x, top, color, i->begin, a.x - pos);
									graph_.rectangle(x + head_w, top, str_w - head_w, line_h_pixels, 0x3399FF, true);
									graph_.string(x + head_w, top, 0xFFFFFF, i->begin + a.x - pos, len - (a.x - pos));
								}
							}
						}

						x += static_cast<int>(str_w);
					}
					if(a.y <= textline && textline < b.y)
						graph_.rectangle(x, top, whitespace_w, line_h_pixels, 0x3399FF, true);
				}
				else if(b.y == textline)
				{
					for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
					{
						std::size_t len = i->end - i->begin;
						unsigned str_w = graph_.text_extent_size(i->begin, len).width;
						if((x + static_cast<int>(str_w) > text_area_.area.x) && (x < xend))
						{
							std::size_t pos = i->begin - strbeg;

							if(pos + len <= b.x)
							{
								graph_.rectangle(x, top, str_w, line_h_pixels, 0x3399FF, true);
								graph_.string(x, top, 0xFFFFFF, i->begin, len);
							}
							else if(pos <= b.x && b.x < pos + len)
							{
								unsigned sel_w = graph_.glyph_extent_size(i->begin, len, 0, b.x - pos).width;
								if(_m_is_right_text(*i))
								{
									nana::paint::graphics graph(str_w, line_h_pixels);
									graph.typeface(graph_.typeface());
									graph.rectangle(0x3399FF, true);
									graph.string(0, 0, 0xFFFFFF, i->begin, len);
									graph_.string(x, top, color, i->begin, len);

									graph_.bitblt(x + (str_w - sel_w), top, sel_w, line_h_pixels, graph, str_w - sel_w, 0);
								}
								else
								{
									graph_.rectangle(x, top, sel_w, line_h_pixels, 0x3399FF, true);
									graph_.string(x, top, 0xFFFFFF, i->begin, b.x - pos);
									graph_.string(x + sel_w, top, color, i->begin + b.x - pos, len - (b.x - pos));
								}
							}
							else
								graph_.string(x, top, color, i->begin, len);
						}

						x += static_cast<int>(str_w);
					}
				}

			}
		}

		//_m_draw
		//@brief: Draw a character at a position specified by caret pos.
		//@return: true if beyond the border
		bool text_editor::_m_draw(nana::char_t c)
		{
			if(this->_m_adjust_caret_into_screen())
				return true;

			const nana::string & lnstr = textbase_.getline(points_.caret.y);
			if(text_area_.area.x + static_cast<int>(graph_.bidi_extent_size(lnstr).width) < _m_endx())
			{
				_m_update_line(points_.caret.y);
				return false;
			}
			return true;
		}

		void text_editor::_m_get_sort_select_points(nana::upoint& a, nana::upoint& b) const
		{
			if((select_.a.y > select_.b.y) || ((select_.a.y == select_.b.y) && (select_.a.x > select_.b.x)))
			{
				a = select_.b;
				b = select_.a;
			}
			else
			{
				a = select_.a;
				b = select_.b;
			}
		}

		void text_editor::_m_offset_y(int y)
		{
			points_.offset.y = y;
		}

		//_m_adjust_caret_into_screen
		//@brief:	Adjust the text offset in order to moving caret into visible area if it is out of the visible area
		//@note:	the function assumes the points_.caret is correct
		bool text_editor::_m_adjust_caret_into_screen()
		{
			this->_m_get_scrollbar_size();
			const unsigned screen_lines = this->screen_lines();

			const unsigned delta_pixels = _m_text_extent_size(STR("    ")).width;
			unsigned x = points_.caret.x;
			const string_type& lnstr = textbase_.getline(points_.caret.y);

			if(x > lnstr.size()) x = static_cast<unsigned>(lnstr.size());

			unsigned text_w = _m_pixels_by_char(points_.caret.y, x);

			unsigned area_w = this->_m_get_text_area_width();

			bool adjusted = true;
			if(static_cast<int>(text_w) < points_.offset.x)
			{
				if(text_w > delta_pixels)
					points_.offset.x = text_w - delta_pixels;
				else
					points_.offset.x = 0;
			}
			else if(area_w && (text_w >= points_.offset.x + area_w))
				points_.offset.x = text_w - area_w + 2;
			else
				adjusted = false;

			if(screen_lines && (points_.caret.y >= points_.offset.y + screen_lines))
			{
				_m_offset_y(static_cast<int>(points_.caret.y - screen_lines) + 1);
				adjusted = true;
			}
			else if(static_cast<int>(points_.caret.y) < points_.offset.y)
			{
				if(screen_lines >= static_cast<unsigned>(points_.offset.y))
					_m_offset_y(0);
				else
					_m_offset_y(static_cast<int>(points_.offset.y - screen_lines));
				adjusted = true;
			}
			else if(points_.offset.y)
			{
				if(textbase_.lines() <= screen_lines)
				{
					_m_offset_y(0);
					adjusted = true;
				}
			}

			_m_scrollbar();
			return adjusted;
		}

		//_m_screen_to_caret
		//@brief: Sets the caret position from the screen point specified by (x, y)
		//@param x, y: screen point
		nana::upoint text_editor::_m_screen_to_caret(int x, int y) const
		{
			nana::upoint res;

			if(textbase_.lines())
			{
				if(y < static_cast<int>(text_area_.area.y))
					y = points_.offset.y ? points_.offset.y - 1 : 0;
				else
					y = (y - static_cast<int>(text_area_.area.y)) / static_cast<int>(this->line_height()) + points_.offset.y;

				if(textbase_.lines() <= static_cast<unsigned>(y))
					res.y = static_cast<int>(textbase_.lines()) - 1;
				else
					res.y = y;
			}

			//Convert the screen point to text caret point
			const string_type& lnstr = textbase_.getline(res.y);

			res.x = static_cast<int>(lnstr.size());
			if(res.x)
			{
				x = x - text_area_.area.x + points_.offset.x;
				if(x > 0)
				{
					unicode_bidi bidi;
					std::vector<unicode_bidi::entity> reordered;
					bidi.linestr(lnstr.c_str(), lnstr.size(), reordered);

					int xbeg = 0;
					for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
					{
						std::size_t len = i->end - i->begin;
						unsigned str_w = _m_text_extent_size(i->begin, len).width;
						if(xbeg <= x && x < xbeg + static_cast<int>(str_w))
						{
							unsigned * pxbuf = new unsigned[len];
							if(graph_.glyph_pixels(i->begin, len, pxbuf))
							{
								x -= xbeg;
								if(_m_is_right_text(*i))
								{	//RTL
									for(std::size_t u = 0; u < len; ++u)
									{
										int chbeg = (str_w - pxbuf[u]);
										if(chbeg <= x && x < static_cast<int>(str_w))
										{
											if((pxbuf[u] > 1) && (x > chbeg + static_cast<int>(pxbuf[u] >> 1)))
												res.x = static_cast<unsigned>(u);
											else
												res.x = static_cast<unsigned>(u + 1);
											break;
										}
										str_w -= pxbuf[u];
									}
								}
								else
								{
									//LTR
									for(std::size_t u = 0; u < len; ++u)
									{
										if(x < static_cast<int>(pxbuf[u]))
										{
											if((pxbuf[u] > 1) && (x > static_cast<int>(pxbuf[u] >> 1)))
												res.x = static_cast<unsigned>(u + 1);
											else
												res.x = static_cast<unsigned>(u);
											break;
										}
										x -= pxbuf[u];
									}
								}
								res.x += static_cast<unsigned>(i->begin - lnstr.c_str());
							}
							delete [] pxbuf;
							return res;
						}
						xbeg += static_cast<int>(str_w);
					}
				}
				else
					res.x = 0;
			}

			return res;
		}

		unsigned text_editor::_m_pixels_by_char(std::size_t textline, std::size_t pos) const
		{
			unicode_bidi bidi;
			std::vector<unicode_bidi::entity> reordered;

			const nana::string& lnstr = textbase_.getline(textline);
			bidi.linestr(lnstr.c_str(), lnstr.size(), reordered);
			const nana::char_t * ch = (pos <= static_cast<int>(lnstr.size()) ? lnstr.c_str() + pos : 0);

			unsigned text_w = 0;
			for(std::vector<unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
			{
				std::size_t len = i->end - i->begin;
				if(i->begin <= ch && ch <= i->end)
				{
					if(_m_is_right_text(*i))
					{
						//RTL
						unsigned * pxbuf = new unsigned[len];
						graph_.glyph_pixels(i->begin, len, pxbuf);
						unsigned * end = pxbuf + len;
						for(unsigned * u = pxbuf + (ch - i->begin); u != end; ++u)
							text_w += *u;

						delete [] pxbuf;
					}
					else
					{
						//LTR
						text_w += _m_text_extent_size(i->begin, ch - i->begin).width;
					}
					break;
				}
				else
					text_w += _m_text_extent_size(i->begin, len).width;
			}
			return text_w;
		}

		bool text_editor::_m_is_right_text(const unicode_bidi::entity& e)
		{
			if(e.bidi_char_type == unicode_bidi::bidi_char::L)
				return false;
			return (e.level & 1);
		}

		//struct attributes
			text_editor::attributes::attributes()
				:	multi_lines(true), editable(true),
					enable_background(true), enable_counterpart(false),
					vscroll(0), hscroll(0)
			{}
		//end struct attributes

		//struct coordinate
			text_editor::coordinate::coordinate():xpos(0){}
		//end struct coordinate
		//end class text_editor
	}//end namespace skeletons
}//end namespace widgets
}//end namespace gui
}//end namespace nana
