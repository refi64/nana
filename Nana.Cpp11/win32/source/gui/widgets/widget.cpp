/*
 *	The fundamental widget class implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/widget.cpp
 */

#include <nana/gui/widgets/widget.hpp>

namespace nana
{
namespace gui
{
	/*
	 * class widget
	 *		The definition of class widget
	 */
		widget::~widget(){}

		nana::string widget::caption() const
		{
			return this->_m_caption();
		}
		
		void widget::caption(const nana::string& str)
		{
			this->_m_caption(str);
		}

		void widget::typeface(const nana::paint::font& font)
		{
			this->_m_typeface(font);
		}

		nana::paint::font widget::typeface() const
		{
			return this->_m_typeface();
		}

		void widget::close()
		{
			this->_m_close();
		}

		window widget::parent() const
		{
			return nana::gui::API::get_parent_window(this->handle());
		}

		bool widget::enabled() const
		{
			return nana::gui::API::window_enabled(this->handle());
		}

		void widget::enabled(bool value)
		{
			this->_m_enabled(value);
		}

		bool widget::empty() const
		{
			return (this->handle() == 0);	
		}

		void widget::show()
		{
			this->_m_show(true);
		}

		void widget::hide()
		{
			this->_m_show(false);
		}

		bool widget::visible() const
		{
			return this->_m_visible();
		}

		nana::size widget::size() const
		{
			return nana::gui::API::window_size(this->handle());
		}

		void widget::size(unsigned width, unsigned height)
		{
			this->_m_size(width, height);
		}

		nana::point widget::pos() const
		{
			return nana::gui::API::window_position(this->handle());
		}

		void widget::move(int x, int y)
		{
			this->_m_move(x, y);
		}

		void widget::move(int x, int y, unsigned width, unsigned height)
		{
			this->_m_move(x, y, width, height);
		}

		bool widget::focused() const
		{
			return nana::gui::API::is_focus_window(handle());
		}

		void widget::foreground(nana::color_t value)
		{
			this->_m_foreground(value);
		}

		nana::color_t widget::foreground() const
		{
			return this->_m_foreground();
		}

		void widget::background(nana::color_t value)
		{
			this->_m_background(value);
		}

		nana::color_t widget::background() const
		{
			return this->_m_background();
		}

		void widget::umake_event(event_handle eh) const
		{
			API::umake_event(eh);
		}

		widget::operator widget::dummy_bool_type() const
		{
			return (this->handle()? dummy_bool_type(1):0);
		}

		widget::operator nana::gui::window() const
		{
			return this->handle();
		}

		void widget::_m_complete_creation()
		{}

		nana::string widget::_m_caption() const
		{
			return API::window_caption(this->handle());
		}
		
		void widget::_m_caption(const nana::string& str)
		{
			API::dev::window_caption(this->handle(), str);
		}

		void widget::_m_close()
		{
			nana::gui::API::close_window(this->handle());
		}

		bool widget::_m_enabled() const
		{
			return nana::gui::API::window_enabled(this->handle());
		}

		void widget::_m_enabled(bool value)
		{
			nana::gui::API::window_enabled(this->handle(), value);
		}

		bool widget::_m_show(bool visible)
		{
			API::show_window(this->handle(), visible);
			return visible;
		}

		bool widget::_m_visible() const
		{
			return API::visible(this->handle());
		}

		void widget::_m_size(unsigned width, unsigned height)
		{
			nana::gui::API::window_size(this->handle(), width, height);
		}

		void widget::_m_move(int x, int y)
		{
			nana::gui::API::move_window(this->handle(), x, y);
		}

		void widget::_m_move(int x, int y, unsigned width, unsigned height)
		{
			API::move_window(this->handle(), x, y, width, height);
		}

		void widget::_m_typeface(const nana::paint::font& font)
		{
			nana::gui::API::typeface(this->handle(), font);
		}

		nana::paint::font widget::_m_typeface() const
		{
			return nana::gui::API::typeface(this->handle());
		}

		void widget::_m_foreground(nana::color_t value)
		{
			nana::gui::API::foreground(this->handle(), value);
		}

		nana::color_t widget::_m_foreground() const
		{
			return nana::gui::API::foreground(this->handle());
		}

		void widget::_m_background(nana::color_t value)
		{
			nana::gui::API::background(this->handle(), value);
		}

		nana::color_t widget::_m_background() const
		{
			return nana::gui::API::background(this->handle());
		}

	//end class widget

}//end namespace gui
}//end namespace nana

