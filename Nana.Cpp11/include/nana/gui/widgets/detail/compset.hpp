#ifndef NANA_GUI_WIDGETS_DETAIL_COMPSET_HPP
#define NANA_GUI_WIDGETS_DETAIL_COMPSET_HPP

#include <nana/basic_types.hpp>

namespace nana{	namespace gui { namespace widgets{ namespace detail
{

	template<typename Component, typename ItemAttribute>
	class compset
	{
	public:
		typedef Component component_t;
		typedef ItemAttribute item_attribute_t;

		struct comp_attribute_t
		{
			nana::rectangle area;
			bool mouse_pointed;
		};

		virtual ~compset(){}

		virtual const item_attribute_t& item_attribute() const = 0;
		virtual bool comp_attribute(component_t, comp_attribute_t &) const = 0;
	};
}//end namespace detail
}//end namespace widgets
}//end namespace gui
}//end namespace nana

#endif