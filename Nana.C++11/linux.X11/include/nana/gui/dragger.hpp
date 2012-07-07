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
		void drag_target(nana::gui::window);
		void trigger(nana::gui::window);
	private:
		dragger_impl_t * impl_;
	};
}//end namespace gui
}//end namespace nana
#endif
