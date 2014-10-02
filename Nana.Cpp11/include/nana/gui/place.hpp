/*
 *	An Implementation of Place for Layout
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 *
 *	Contributions:
 *	min/max and splitter bar initial weight by qPCR4vir.
 */

#ifndef NANA_GUI_PLACE_HPP
#define NANA_GUI_PLACE_HPP
#include <utility>
#include <nana/gui/basis.hpp>

namespace nana
{
    ///  Layout managment - an object of class place is attached to a widget, and it automatically positions and resizes the children widgets.
	class place
		: ::nana::noncopyable
	{
		struct implement;

		class field_interface
		{
			field_interface(const field_interface&) = delete;
			field_interface& operator=(const field_interface&) = delete;
			field_interface(field_interface&&) = delete;
			field_interface& operator=(field_interface&&) = delete;
		public:
			field_interface() = default;
			virtual ~field_interface() = default;
			virtual field_interface& operator<<(window) = 0;
			virtual field_interface& fasten(window) = 0;
		};
	public:
        ///  reference to a field manipulator which refers to a field object created by place 
		typedef field_interface & field_reference;

		place();
		place(window);///< Attaches to a specified widget.
		~place();

		/** @brief Bind to a window
		 *	@param handle	A handle to a window which the place wants to attach.
		 *	@remark	It will throw an exception if the place has already binded to a window.
		 */
		void bind(window handle);
		window window_handle() const;
        
		void div(const char* s);              ///< Divides the attached widget into fields.
		field_reference field(const char* name);///< Returns a field with the specified name.
		void collocate();                     ///< Layouts the widgets.

		void erase(window handle);				///< Erases a window from field.
	private:
		implement * impl_;
	};
}//end namespace nana

#endif //#ifndef NANA_GUI_PLACE_HPP
