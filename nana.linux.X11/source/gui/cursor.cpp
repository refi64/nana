/*
 *	An Cursor Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/cursor.hpp
 *	@patchs:
 *		Jan 03 2012, unsigned to size_t conversion fail for x64, Hiroshi Seki
 */

#include <nana/gui/cursor.hpp>
#include <nana/gui/detail/bedrock.hpp>
#include <nana/detail/platform_spec.hpp>
#include <nana/gui/programming_interface.hpp>
#include <map>
namespace nana
{
namespace gui
{
	namespace detail
	{
		struct cursor_impl
		{
			cursor::predef::t predef;
			nana::gui::native_window_type wd;

			cursor_impl()
				: predef(cursor::predef::arrow), wd(0)
			{}

			cursor_impl(nana::gui::native_window_type wd)
				:predef(cursor::predef::arrow), wd(wd)
			{}
		};
#if defined(NANA_WINDOWS)
		const nana::char_t* translate(cursor::predef::t id)
		{
			typedef cursor::predef predef;
			const nana::char_t* name = IDC_ARROW;

			switch(id)
			{
			case predef::arrow:
				name = IDC_ARROW;	break;
			case predef::wait:
				name = IDC_WAIT;	break;
			case predef::hand:
				name = IDC_HAND;	break;
			case predef::size_we:
				name = IDC_SIZEWE;	break;
			case predef::size_ns:
				name = IDC_SIZENS;	break;
			case predef::size_bottom_left:
			case predef::size_top_right:
				name = IDC_SIZENESW;	break;
			case predef::size_top_left:
			case predef::size_bottom_right:
				name = IDC_SIZENWSE;	break;
			case predef::iterm:
				name = IDC_IBEAM;	break;
			}
			return name;
		}
#endif

		bool load_corsor(nana::gui::native_window_type wd, cursor::predef::t id)
		{
			const bedrock::thread_context* contx = bedrock::instance().get_thread_context();
			if(contx)
			{
#if defined(NANA_WINDOWS)
				const nana::char_t *idstr = translate(id);
#ifdef _WIN64
				::SetClassLongPtr(reinterpret_cast<HWND>(wd), GCLP_HCURSOR,
					reinterpret_cast<LONG_PTR>(::LoadCursor(0, idstr)));
#else
				::SetClassLong(reinterpret_cast<HWND>(wd), GCL_HCURSOR,
					static_cast<unsigned long>(reinterpret_cast<size_t>(::LoadCursor(0, idstr))));
#endif
#elif defined(NANA_LINUX)

#endif
				return true;
			}
			return false;
		}
	}

	cursor::cursor()
		:impl_(new detail::cursor_impl)
	{}

	cursor::cursor(nana::gui::window wd)
		:impl_(new detail::cursor_impl(nana::gui::API::root(wd)))
	{}

	cursor::cursor(const cursor& rhs)
		:impl_(new detail::cursor_impl(rhs.impl_->wd))
	{
		_m_assign(rhs.impl_);
	}

	cursor& cursor::operator =(const cursor& rhs)
	{
		if(this != & rhs)
			_m_assign(rhs.impl_);

		return *this;
	}

	cursor::~cursor()
	{
		delete impl_;
	}

	void cursor::_m_assign(detail::cursor_impl* rhs)
	{
		if(rhs)
		{
			impl_->predef = rhs->predef;
			impl_->wd = rhs->wd;
		}
	}

	bool cursor::bind(nana::gui::window wd)
	{
		nana::gui::native_window_type ntwd = nana::gui::API::root(wd);
		if(ntwd != impl_->wd)
		{
			impl_->wd = ntwd;
			load(predef::arrow);
		}
		return false;
	}

	cursor::predef::t cursor::load(cursor::predef::t id)
	{
		const nana::gui::detail::bedrock::thread_context* contx = nana::gui::detail::bedrock::instance().get_thread_context();
		if(contx && impl_->wd)
		{
			if(id == impl_->predef)
				return id;

			if(detail::load_corsor(impl_->wd, id))
			{
				cursor::predef::t prev = impl_->predef;
				impl_->predef = id;
				return prev;
			}
		}

		return predef::arrow;
	}

	cursor::predef::t cursor::get() const
	{
		return impl_->predef;
	}

	//auto_cursor_impl
	class auto_cursor::auto_cursor_impl
	{
		struct cursor_window_t
		{
			nana::gui::native_window_type native;
			cursor::predef::t id;
			nana::gui::event_handle enter;
			nana::gui::event_handle leave;
		};
	public:
		typedef std::map<nana::gui::window, cursor_window_t> table;
		typedef table::iterator iterator;

		auto_cursor_impl()
			: trace_(0)
		{}

		~auto_cursor_impl()
		{
			for(iterator i = manager_.begin(); i != manager_.end(); ++i)
				_m_erase_core(i);
			manager_.clear();
		}

		void set(nana::gui::window wd, cursor::predef::t id)
		{
			iterator i = manager_.find(wd);
			if(id == cursor::predef::arrow)
			{
				_m_erase_core(i);
				manager_.erase(i);
				return;
			}

			if(manager_.end() == i)
			{
				nana::functor<void(const eventinfo&)> fn(*this, &auto_cursor_impl::_m_trace);
				cursor_window_t & cw = manager_[wd];
				cw.native = API::root(wd);
				cw.id = id;
				cw.enter = API::make_event<events::mouse_enter>(wd, fn);
				cw.leave = API::make_event<events::mouse_leave>(wd, fn);
			}
			else
                i->second.id = id;
		}

		void erase(nana::gui::window wd)
		{
			iterator i = manager_.find(wd);
			if(i != manager_.end())
			{
				if(trace_ == wd)
					detail::load_corsor(i->second.native, cursor::predef::arrow);

				_m_erase_core(i);
				manager_.erase(i);
			}
		}
	private:
		void _m_erase_core(iterator i)
		{
			cursor_window_t & cw = i->second;
			API::umake_event(cw.enter);
			API::umake_event(cw.leave);
		}
	private:
		void _m_trace(const eventinfo& ei)
		{
			trace_ = (ei.identifier == events::mouse_enter::identifier ? ei.window : 0);

			iterator i = manager_.find(ei.window);
			if(i != manager_.end())
				detail::load_corsor(i->second.native, (trace_ ? i->second.id : cursor::predef::arrow));
		}
	private:
		nana::gui::window trace_;
		table manager_;
	};

	//class auto_cursor
		auto_cursor::auto_cursor()
			: impl_(new auto_cursor_impl)
		{}

		auto_cursor::~auto_cursor()
		{
			delete impl_;
		}

		void auto_cursor::set(nana::gui::window wd, cursor::predef::t id)
		{
			impl_->set(wd, id);
		}

		void auto_cursor::erase(nana::gui::window wd)
		{
			impl_->erase(wd);
		}
	//end class auto_cursor
}//end namespace gui
}//end namespace nana
