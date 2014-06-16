
#include <nana/gui/dragger.hpp>

namespace nana{ namespace gui
{
	class dragger::dragger_impl_t
	{
		struct drag_target_t
		{
			window wd;
			nana::rectangle restrict_area;
			nana::arrange	move_direction;
			nana::point		origin;

			drag_target_t(window w, const nana::rectangle& r, nana::arrange m)
				: wd(w), restrict_area(r), move_direction(m)
			{}
		};

		struct trigger_t
		{
			window wd;
			event_handle press;
			event_handle over;
			event_handle release;
			event_handle destroy;
		};
	public:
		dragger_impl_t()
			: dragging_(false)
		{}

		~dragger_impl_t()
		{
			_m_clear_triggers();
		}

		void drag_target(window wd, const nana::rectangle& restrict_area, arrange arg)
		{
			for (std::vector<drag_target_t>::iterator i = targets_.begin(); i != targets_.end(); ++i)
			{
				drag_target_t & td = *i;
				if (td.wd == wd)
				{
					td.restrict_area = restrict_area;
					td.move_direction = arg;
					return;
				}
			}
			targets_.push_back(drag_target_t(wd, restrict_area, arg));
		}

		void remove_target(window wd)
		{
			for (std::vector<drag_target_t>::iterator i = targets_.begin(); i != targets_.end(); ++i)
			{
				if (i->wd == wd)
				{
					targets_.erase(i);
					return;
				}
			}
		}

		void trigger(window wd)
		{
			trigger_t tg;
			tg.wd = wd;
			nana::functor<void(const eventinfo&)> fn(*this, &dragger_impl_t::_m_trace);
			tg.press = API::make_event<events::mouse_down>(wd, fn);
			tg.over = API::make_event<events::mouse_move>(wd, fn);
			tg.release = API::make_event<events::mouse_up>(wd, fn);
			tg.destroy = API::make_event<events::destroy>(wd, fn);

			triggers_.push_back(tg);
		}
	private:
		void _m_clear_triggers()
		{
			for(std::vector<trigger_t>::iterator i = triggers_.begin(); i != triggers_.end(); ++i)
			{
				API::umake_event(i->press);
				API::umake_event(i->over);
				API::umake_event(i->release);
				API::umake_event(i->destroy);
				API::capture_window(i->wd, false);
			}
			triggers_.clear();
		}

		void _m_destroy(const eventinfo& ei)
		{
			for(std::vector<trigger_t>::iterator i = triggers_.begin(); i != triggers_.end(); ++i)
			{
				if(i->wd == ei.window)
				{
					triggers_.erase(i);
					API::capture_window(ei.window, false);
					return;
				}
			}
		}

		void _m_check_restrict_area(nana::point & pos, const nana::size & size, const nana::rectangle& restr_area)
		{
			if ((pos.x > 0) && (static_cast<int>(size.width) + pos.x > restr_area.right()))
				pos.x = restr_area.right() - static_cast<int>(size.width);

			if (pos.x < restr_area.x)
				pos.x = restr_area.x;

			if ((pos.y > 0) && (static_cast<int>(size.height) + pos.y > restr_area.bottom()))
				pos.y = restr_area.bottom() - static_cast<int>(size.height);

			if (pos.y < restr_area.y)
				pos.y = restr_area.y;
		}

		void _m_trace(const eventinfo& ei)
		{
			switch(ei.identifier)
			{
			case events::mouse_down::identifier:
				dragging_ = true;
				API::capture_window(ei.window, true);
				origin_ = API::cursor_position();
				for(std::vector<drag_target_t>::iterator i = targets_.begin(); i != targets_.end(); ++i)
				{
					i->origin = API::window_position(i->wd);
					window owner = API::get_owner_window(i->wd);
					if(owner)
						API::calc_screen_point(owner, i->origin);
				}
				break;
			case events::mouse_move::identifier:
				if(dragging_ && ei.mouse.left_button)
				{
					nana::point pos = API::cursor_position();
					pos.x -= origin_.x;
					pos.y -= origin_.y;
					for(std::vector<drag_target_t>::iterator i = targets_.begin(); i != targets_.end(); ++i)
					{
						if(API::is_window_zoomed(i->wd, true) == false)
						{
							window owner = API::get_owner_window(i->wd);

							nana::point wdps = i->origin;
							if (owner)
								API::calc_window_point(owner, wdps);

							switch (i->move_direction)
							{
							case nana::arrange::horizontal:
								wdps.x += pos.x;
								break;
							case nana::arrange::vertical:
								wdps.y += pos.y;
								break;
							default:
								wdps.x += pos.x;
								wdps.y += pos.y;
								break;
							}

							if (!i->restrict_area.empty_size())
								_m_check_restrict_area(wdps, API::window_size(i->wd), i->restrict_area);

							API::move_window(i->wd, wdps.x, wdps.y);
						}
					}
				}
				break;
			case events::mouse_up::identifier:
				API::capture_window(ei.window, false);
				dragging_ = false;
				break;
			default:
				break;
			}
		}
		
	private:
		bool dragging_;
		nana::point origin_;
		std::vector<drag_target_t> targets_;
		std::vector<trigger_t> triggers_;
	};

	//class dragger
		dragger::dragger()
			: impl_(new dragger_impl_t)
		{}

		dragger::~dragger()
		{
			delete impl_;
		}
		void dragger::target(window wd)
		{
			impl_->drag_target(wd, rectangle(), nana::arrange::horizontal_vertical);
		}

		void dragger::target(window wd, const rectangle& restrict_area, nana::arrange arg)
		{
			impl_->drag_target(wd, restrict_area, arg);
		}

		void dragger::remove_target(window wd)
		{
			impl_->remove_target(wd);
		}

		void dragger::trigger(window tg)
		{
			impl_->trigger(tg);
		}
	//end class dragger

}//end namespace gui
}//end namespace nana
