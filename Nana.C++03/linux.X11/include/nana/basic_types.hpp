/*
 *	Basic Types definition
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/basic_types.hpp
 */

#ifndef NANA_BASIC_TYPES_HPP
#define NANA_BASIC_TYPES_HPP

#include "macros.hpp"

namespace nana
{
	namespace detail
	{
		struct drawable_impl_type;	//declearation, defined in platform_spec.hpp
	}

	namespace paint
	{
		typedef nana::detail::drawable_impl_type*	drawable_type;
	}

	namespace gui
	{
		enum mouse_action_t{mouse_action_normal, mouse_action_over, mouse_action_pressed, mouse_action_end};
	}

	typedef unsigned scalar_t;
	typedef unsigned char	uint8_t;
	typedef unsigned long	uint32_t;
	typedef unsigned		uint_t;
	typedef unsigned		color_t;

	const color_t null_color = 0xFFFFFFFF;

	struct pixel_rgb_t
	{
		union
		{
			struct element_tag
			{
				unsigned int blue:8;
				unsigned int green:8;
				unsigned int red:8;
				unsigned int alpha_channel:8;
			}element;

			color_t color;
		}u;
	};

	struct point
	{
		point();
		point(int x, int y);
		bool operator==(const point&) const;
		bool operator!=(const point&) const;
		bool operator<(const point&) const;
		bool operator<=(const point&) const;
		bool operator>(const point&) const;
		bool operator>=(const point&) const;

		int x;
		int y;
	};

	struct upoint
	{
		typedef unsigned value_type;

		upoint();
		upoint(value_type x, value_type y);
		bool operator==(const upoint&) const;
		bool operator!=(const upoint&) const;
		bool operator<(const upoint&) const;
		bool operator<=(const upoint&) const;
		bool operator>(const upoint&) const;
		bool operator>=(const upoint&) const;

		value_type x;
		value_type y;
	};

	struct size
	{
		size();
		size(unsigned width, unsigned height);

		bool operator==(const size& rhs) const;
		bool operator!=(const size& rhs) const;

		unsigned width;
		unsigned height;
	};

	struct rectangle
	{
		rectangle();
		rectangle(int x, int y, unsigned width, unsigned height);
		rectangle(const size &);
		rectangle(const point&, const size&);

		bool operator==(const rectangle& rhs) const;
		bool operator!=(const rectangle& rhs) const;

		int x;
		int y;
		unsigned width;
		unsigned height;
	};

	struct arrange
	{
		enum t{unkown, horizontal, vertical, horizontal_vertical};
		t value;

		arrange();
		arrange(t);
		operator t() const;
		arrange& operator=(t);
		bool operator==(t) const;
		bool operator!=(t) const;
	};
}//end namespace nana

#endif


