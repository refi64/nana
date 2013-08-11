#ifndef NANA_PAINT_DETAIL_IMAGE_ICO_HPP
#define NANA_PAINT_DETAIL_IMAGE_ICO_HPP

#include "image_impl_interface.hpp"

namespace nana{	namespace paint
{
	namespace detail
	{
		class image_ico
			:public image::image_impl_interface
		{
#if defined(NANA_WINDOWS)
			struct handle_deleter
			{
				void operator()(HICON handle) const;
			};//end struct handle_deleter
			typedef nana::refer<HICON, handle_deleter> refer_type;
#else
			typedef nana::refer<int*> refer_type;
#endif
		public:
			image_ico(bool is_ico);

			bool open(const nana::char_t* filename);
			bool alpha_channel() const;
			bool empty() const;
			void close();
			nana::size size() const;
			virtual void paste(const nana::rectangle& src_r, graph_reference graph, int x, int y) const;
			virtual void stretch(const nana::rectangle&, graph_reference graph, const nana::rectangle& r) const;

			const refer_type & ref() const;
		private:
			const bool	is_ico_;
			nana::size	size_;
			refer_type refer_;
		};//end class image_ico
	}
}//end namespace paint
}//end namespace nana

#endif
