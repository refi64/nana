/*
 *	Platform Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/paint/detail/native_paint_interface.cpp
 */

#include <nana/paint/detail/native_paint_interface.hpp>
#include <nana/paint/pixel_buffer.hpp>
#include <nana/gui/layout_utility.hpp>

#include <nana/detail/platform_spec.hpp>

namespace nana
{
namespace paint
{
namespace detail
{

	nana::size drawable_size(drawable_type dw)
	{
		if(0 == dw) return nana::size();
#if defined(NANA_WINDOWS)
		BITMAP bmp;
		::GetObject(dw->pixmap, sizeof bmp, &bmp);
		return nana::size(bmp.bmWidth, bmp.bmHeight);
#elif defined(NANA_X11)
        nana::detail::platform_spec & spec = nana::detail::platform_spec::instance();
        Window root;
        int x, y;
        unsigned width, height;
        unsigned border, depth;
        nana::detail::platform_scope_guard psg;
        ::XGetGeometry(spec.open_display(), dw->pixmap, &root, &x, &y, &width, &height, &border, &depth);
		return nana::size(width, height);
#endif
	}


	unsigned char * fade_table(double fade_rate)
	{
		unsigned char* tablebuf = new unsigned char[0x100 * 2];
		unsigned char* d_table = tablebuf;
		unsigned char* s_table = d_table + 0x100;

		double fade_rate_mul_to_add = 0;
		double fade_rate_2 = fade_rate + fade_rate;
		double fade_rate_3 = fade_rate_2 + fade_rate;
		double fade_rate_4 = fade_rate_3 + fade_rate;

		for(int i = 0; i < 0x100; i += 4, fade_rate_mul_to_add += fade_rate_4)
		{
			d_table[0] = static_cast<unsigned char>(fade_rate_mul_to_add);
			s_table[0] = i - d_table[0];

			d_table[1] = static_cast<unsigned char>(fade_rate_mul_to_add + fade_rate);
			s_table[1] = i + 1 - d_table[1];

			d_table[2] = static_cast<unsigned char>(fade_rate_mul_to_add + fade_rate_2);
			s_table[2] = i + 2 - d_table[2];

			d_table[3] = static_cast<unsigned char>(fade_rate_mul_to_add + fade_rate_3);
			s_table[3] = i + 3 - d_table[3];

			d_table += 4;
			s_table += 4;
		}
		return tablebuf;
	}

	void free_fade_table(const unsigned char* table)
	{
		delete [] table;
	}

	nana::pixel_rgb_t fade_color(nana::pixel_rgb_t bgcolor, nana::pixel_rgb_t fgcolor, double fade_rate)
	{
		pixel_rgb_t ret;
		double lrate = 1.0 - fade_rate;

		ret.u.element.red = static_cast<unsigned char>(bgcolor.u.element.red * fade_rate + fgcolor.u.element.red * lrate);
		ret.u.element.green = static_cast<unsigned char>(bgcolor.u.element.green * fade_rate + fgcolor.u.element.green * lrate);
		ret.u.element.blue = static_cast<unsigned char>(bgcolor.u.element.blue * fade_rate + fgcolor.u.element.blue * lrate);
		ret.u.element.alpha_channel = 0;
		return ret;
	}

	nana::pixel_rgb_t fade_color(nana::pixel_rgb_t bgcolor, nana::pixel_rgb_t fgcolor, const unsigned char* const fade_table)
	{
		const unsigned char * const s_fade_table = fade_table + 0x100;

		bgcolor.u.element.red = fade_table[bgcolor.u.element.red] + s_fade_table[fgcolor.u.element.red];
		bgcolor.u.element.green = fade_table[bgcolor.u.element.green] + s_fade_table[fgcolor.u.element.green];
		bgcolor.u.element.blue = fade_table[bgcolor.u.element.blue] + s_fade_table[fgcolor.u.element.blue];
		return bgcolor;
	}

	nana::pixel_rgb_t fade_color_intermedia(nana::pixel_rgb_t fgcolor, const unsigned char* fade_table)
	{
		fade_table += 0x100;
		fgcolor.u.element.red = fade_table[fgcolor.u.element.red];
		fgcolor.u.element.green = fade_table[fgcolor.u.element.green];
		fgcolor.u.element.blue = fade_table[fgcolor.u.element.blue];
		return fgcolor;
	}

	nana::pixel_rgb_t fade_color_by_intermedia(nana::pixel_rgb_t bgcolor, nana::pixel_rgb_t fgcolor_intermedia, const unsigned char* const fade_table)
	{
		bgcolor.u.element.red = fade_table[bgcolor.u.element.red] + fgcolor_intermedia.u.element.red;
		bgcolor.u.element.green = fade_table[bgcolor.u.element.green] + fgcolor_intermedia.u.element.green;
		bgcolor.u.element.blue = fade_table[bgcolor.u.element.blue] + fgcolor_intermedia.u.element.blue;
		return bgcolor;
	}

	void blend(drawable_type dw, int x, int y, unsigned width, unsigned height, unsigned color, double fade_rate)
	{
		if(fade_rate <= 0) return;
		if(fade_rate > 1) fade_rate = 1;

		nana::size sz = drawable_size(dw);
		nana::rectangle r;
		if(false == nana::gui::overlap(nana::rectangle(0, 0, sz.width, sz.height), nana::rectangle(x, y, width, height), r))
			return;

		unsigned red = static_cast<unsigned>((color & 0xFF0000) * fade_rate);
		unsigned green = static_cast<unsigned>((color & 0xFF00) * fade_rate);
		unsigned blue = static_cast<unsigned>((color & 0xFF) * fade_rate);

		double lrate = 1 - fade_rate;
		pixel_buffer pixbuf(dw, r.y, r.height);

		for(size_t row = 0; row < r.height; ++row)
		{
			nana::pixel_rgb_t * i = pixbuf.raw_ptr(row) + r.x;
			const nana::pixel_rgb_t * const end = i + r.width;
			for(; i < end; ++i)
			{
				unsigned px_r = ((static_cast<unsigned>((i->u.color & 0xFF0000) * lrate) + red) & 0xFF0000);
				unsigned px_g = ((static_cast<unsigned>((i->u.color & 0xFF00) * lrate) + green) & 0xFF00);
				unsigned px_b = ((static_cast<unsigned>((i->u.color & 0xFF) * lrate) + blue) & 0xFF);
				i->u.color = (px_r | px_g | px_b);
			}
		}
		pixbuf.paste(nana::rectangle(r.x, 0, r.width, r.height), dw, r.x, r.y);
	}
}//end namespace detail
}//end namespace paint
}//end namespace nana
