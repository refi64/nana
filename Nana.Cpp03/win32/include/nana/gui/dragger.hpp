#ifndef NANA_GUI_DRAGGER_HPP
#define NANA_GUI_DRAGGER_HPP

#include "programming_interface.hpp"

namespace nana{ namespace gui{

	class dragger
		: nana::noncopyable
	{
		class dragger_impl_t;
	public:
		dragger();
		~dragger();
		void drag_target(nana::gui::window wd);
		void trigger(nana::gui::window wd);
	private:
		dragger_impl_t * impl_;
	};
}//end namespace gui
}//end namespace nana
#endif
