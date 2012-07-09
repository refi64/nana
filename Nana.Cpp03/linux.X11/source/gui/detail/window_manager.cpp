/*
 *	Window Manager Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/window_manager.cpp
 *
 */

#include <nana/gui/detail/window_manager.hpp>

namespace nana
{
namespace gui
{
namespace detail
{

	//class signal_manager

		//begin struct inner_invoker
			signal_manager::inner_invoker::~inner_invoker(){}
		//end struct inner_invoker

		signal_manager::~signal_manager()
		{
			manager_.clear();
		}

		void signal_manager::umake(identifier id)
		{
			if(id == 0) return;
			manager_.erase(id);
			end_ = manager_.end();
		}

		void signal_manager::fireaway(signal_manager::identifier wd, int message, const signals& info)
		{
			std::map<identifier, inner_invoker*>::iterator i = manager_.find(wd);
			if(i != manager_.end())
			{
				inner_invoker* & invk = i->second;
				if(invk)	invk->fireaway(message, info);
			}
		}
	//end class signal_manager


	//class shortkey_container::
		bool shortkey_container::make(nana::gui::window wnd, unsigned long key)
		{
			if(wnd == 0) return false;
			if(key < 0x61) key += (0x61 - 0x41);

			for(std::vector<item_type>::iterator i = keybase_.begin(); i != keybase_.end(); ++i)
			{
				if(i->window == wnd)
				{
					i->keys.push_back(key);
					return true;
				}
			}

			item_type m;
			m.window = wnd;
			m.keys.push_back(key);
			keybase_.push_back(m);

			return true;
		}

		void shortkey_container::clear()
		{
			keybase_.clear();
		}

		void shortkey_container::umake(nana::gui::window wnd)
		{
			if(wnd == 0) return;
			for(std::vector<item_type>::iterator i = keybase_.begin(); i != keybase_.end(); ++i)
			{
				if(i->window == wnd)
				{
					keybase_.erase(i);
					return;
				}
			}
		}

		nana::gui::window shortkey_container::find(unsigned long key)
		{
			if(key < 0x61) key += (0x61 - 0x41);

			for(std::vector<item_type>::iterator i = keybase_.begin(); i != keybase_.end(); ++i)
			{
				for(std::vector<unsigned long>::iterator n = i->keys.begin(); n != i->keys.end(); ++n)
					if(key == *n)
						return i->window;
			}
			return 0;
		}
	//end class shortkey_container


	//class tray_event_manager
		void tray_event_manager::fire(native_window_type wd, unsigned identifier, const eventinfo& ei)
		{
			maptable_type::const_iterator i = maptable_.find(wd);
			if(i == maptable_.end()) return;
			
			event_maptable_type::const_iterator u = i->second.find(identifier);
			if(u == i->second.end()) return;
			
			const fvec_t & fvec = u->second;
			for(fvec_t::const_iterator j = fvec.begin(); j != fvec.end(); ++j)
				(*j)(ei);
		}

		bool tray_event_manager::make(native_window_type wd, unsigned identifier, const nana::functor<void(const eventinfo&)> & f)
		{
			if(wd)
			{
				maptable_[wd][identifier].push_back(f);
				return true;
			}
			return false;
		}

		void tray_event_manager::umake(native_window_type wd)
		{
			maptable_.erase(wd);
		}
	//end class tray_event_manager
}//end namespace detail
}//end namespace gui
}//end namespace nana
