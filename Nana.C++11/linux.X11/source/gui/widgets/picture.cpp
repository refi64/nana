/*
 *	A Picture Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/picture.cpp
 *	@description:
 *		Used for showing a picture
 */

#include <nana/gui/widgets/picture.hpp>
#include <nana/paint/image.hpp>

namespace nana
{
namespace gui
{
	namespace xpicture
	{

		//class picture_drawer:: public nana::gui::drawer_trigger
			picture_drawer::picture_drawer():graph_(nullptr)
			{
				backimg_.arg = nana::arrange::unkown;
				backimg_.beg = backimg_.end = 0;
			}

			picture_drawer::runtime_type::runtime_type()
				:background_shadow_start(0), background_shadow_end(0), horizontal(true)
			{}

			void picture_drawer::bind_window(nana::gui::widget& widget)
			{
				widget_ = &widget;
			}

			void picture_drawer::attached(nana::paint::graphics& ghc)
			{
				graph_ = &ghc;
			}

			void picture_drawer::load(const nana::char_t* file)
			{
				backimg_.image.open(file);
			}

			void picture_drawer::load(const nana::paint::image& img)
			{
				backimg_.image = img;
			}

			void picture_drawer::bitblt(int x, int y, unsigned width, unsigned height, const nana::paint::graphics& source, int src_x, int src_y)
			{
				if(graph_)
					graph_->bitblt(x, y, width, height, source, src_x, src_y);
			}

			void picture_drawer::set_shadow_background(unsigned begin_color, unsigned end_color, bool horizontal)
			{
				runtime_.background_shadow_end = end_color;
				runtime_.background_shadow_start = begin_color;
				runtime_.horizontal = horizontal;

				_m_draw_background();
			}

			bool picture_drawer::bgstyle(bool is_stretch, nana::arrange arg, int beg, int end)
			{
				if(backimg_.image.empty() == false)
				{
					backimg_.is_stretch = is_stretch;
					backimg_.arg = arg;

					if(arg == nana::arrange::horizontal_vertical) return false;

					if(beg < 0) beg = 0;
					if(end < beg) end = beg;
					if((backimg_.beg == beg) && (backimg_.end == end)) return false;

					nana::size imgsize = backimg_.image.size();
					unsigned scale = (arg == nana::arrange::horizontal ? imgsize.width : imgsize.height);

					if(beg < 0)
						beg = 0;
					else if(static_cast<unsigned>(beg) >= scale)
						beg = static_cast<int>(scale) - 1;

					if(end < beg)
						end = beg;
					else if(static_cast<unsigned>(end) >= scale)
						end = static_cast<int>(scale) - 1;
					
					backimg_.beg = beg;
					backimg_.end = end;
					return true;
				}
				
				backimg_.arg = nana::arrange::unkown;
				return false;
			}

			void picture_drawer::refresh(graph_reference graph)
			{
				if(graph.changed())
				{
					_m_draw_background();
					if(backimg_.image.empty() == false)
					{
						nana::size imgsize = backimg_.image.size();
						nana::size gsize(graph.width(), graph.height());

						switch(backimg_.arg)
						{
						case nana::arrange::unkown:
							backimg_.image.paste(graph, 0, 0);
							break;
						case nana::arrange::horizontal:
							if(backimg_.beg < backimg_.end)
							{
								unsigned block_tail = imgsize.width - backimg_.end;

								if(backimg_.beg)
									backimg_.image.paste(graph, 0, 0, static_cast<unsigned>(backimg_.beg), imgsize.height);

								if(block_tail)
									backimg_.image.paste(graph, gsize.width - block_tail, 0, block_tail, imgsize.height, static_cast<int>(imgsize.width - block_tail), 0);
								
								if(backimg_.beg < backimg_.end)
								{
									unsigned fixed_size = backimg_.beg + block_tail;
									if(fixed_size < gsize.width)
									{
										if(false == backimg_.is_stretch)
										{
											unsigned imgarea = backimg_.end - backimg_.beg;
											fixed_size = gsize.width - fixed_size;
											int x = backimg_.beg;
											while(imgarea < fixed_size)
											{
												backimg_.image.paste(graph, x, 0, imgarea, imgsize.height, backimg_.beg, 0);
												x += static_cast<int>(imgarea);
												fixed_size -= imgarea;
											}
											if(fixed_size)
												backimg_.image.paste(graph, x, 0, fixed_size, imgsize.height, backimg_.beg, 0);
										}
										else
											backimg_.image.paste(graph, backimg_.beg, 0, gsize.width - fixed_size, imgsize.height, backimg_.beg, 0, imgsize.width - fixed_size, imgsize.height);
										
									}
								}
							}
							else
							{
								if(false == backimg_.is_stretch)
								{
									int x = 0;
									while(x < static_cast<int>(gsize.width))
									{
										backimg_.image.paste(graph, x, 0);
										x += static_cast<int>(imgsize.width);
									}									
								}
								else
									backimg_.image.paste(graph, 0, 0, gsize.width, imgsize.height, 0, 0, imgsize.width, imgsize.height);
							}
							break;
						case nana::arrange::vertical:
							if(backimg_.beg < backimg_.end)
							{
								unsigned block_tail = imgsize.height - backimg_.end;

								if(backimg_.beg)
									backimg_.image.paste(graph, 0, 0, imgsize.width, static_cast<unsigned>(backimg_.beg));

								if(block_tail)
									backimg_.image.paste(graph, 0, gsize.height - block_tail, imgsize.width, block_tail, 0, static_cast<int>(imgsize.height - block_tail));
								
								if(backimg_.beg < backimg_.end)
								{
									unsigned fixed_size = backimg_.beg + block_tail;
									if(fixed_size < gsize.height)
									{
										if(false == backimg_.is_stretch)
										{
											unsigned imgarea = backimg_.end - backimg_.beg;
											fixed_size = gsize.height - fixed_size;
											int y = backimg_.beg;
											while(imgarea < fixed_size)
											{
												backimg_.image.paste(graph, 0, y, imgsize.width, imgarea, 0, backimg_.beg);
												y += static_cast<int>(imgarea);
												fixed_size -= imgarea;
											}
											if(fixed_size)
												backimg_.image.paste(graph, 0, y, imgsize.width, fixed_size, 0, backimg_.beg);
										}
										else
											backimg_.image.paste(graph, 0, backimg_.beg, imgsize.width, gsize.height - fixed_size, 0, backimg_.beg, imgsize.width, imgsize.height - fixed_size);
									}
								}
							}
							else
							{
								if(false == backimg_.is_stretch)
								{
									int y = 0;
									while(y < static_cast<int>(gsize.height))
									{
										backimg_.image.paste(graph, 0, y);
										y += static_cast<int>(imgsize.height);
									}
								}
								else
									backimg_.image.paste(graph, 0, 0, imgsize.width, gsize.height, 0, 0, imgsize.width, imgsize.height);
							}
							break;
						case nana::arrange::horizontal_vertical:
							if(backimg_.is_stretch == false)
							{
								int y = 0;
								while(y < static_cast<int>(gsize.height))
								{
									int x = 0;
									while(x < static_cast<int>(gsize.width))
									{
										backimg_.image.paste(graph, x, y);
										x += static_cast<int>(imgsize.width);
									}
									y += static_cast<int>(imgsize.height);
								}
							}
							else
								backimg_.image.paste(graph, 0, 0, gsize.width, gsize.height, 0, 0, imgsize.width, imgsize.height);
							break;
						}
					}
				}
			}

			void picture_drawer::_m_draw_background()
			{
				if(graph_)
				{
					if(API::glass_window(widget_->handle()))
					{
						API::make_glass_background(widget_->handle());
					}
					else
					{
						unsigned bkcolor = widget_->background();

						if(runtime_.background_shadow_end == runtime_.background_shadow_start)
							graph_->rectangle(0, 0, graph_->width(), graph_->height(), (runtime_.background_shadow_end ? runtime_.background_shadow_end : bkcolor), true);
						else
							graph_->shadow_rectangle(0, 0, graph_->width(), graph_->height(), runtime_.background_shadow_start, runtime_.background_shadow_end, !runtime_.horizontal);
					}
				}
			}
		//end class picture_drawer
	}//end namespace xpicture

	//class picture
		picture::picture(){}

		picture::picture(window wd)
		{
			create(wd, 0, 0, 0, 0);
		}

		picture::picture(window wd, const nana::rectangle& r)
		{
			create(wd, r.x, r.y, r.width, r.height);
		}

		picture::picture(window wd, int x, int y, unsigned width, unsigned height)
		{
			create(wd, x, y, width, height);
		}

		void picture::load(const nana::char_t* file)
		{
			get_drawer_trigger().load(file);
			API::refresh_window( this->handle());
		}

		void picture::load(const nana::paint::image& img)
		{
			get_drawer_trigger().load(img);
			API::refresh_window( this->handle());
		}

		void picture::bgstyle(bool stretchable, nana::arrange arg, int beg, int end)
		{
			if(this->get_drawer_trigger().bgstyle(stretchable, arg, beg, end))
			{
				API::refresh_window(this->handle());
			}
		}

		void picture::bitblt(int x, int y, unsigned width, unsigned height, const nana::paint::graphics& source, int src_x, int src_y)
		{
			get_drawer_trigger().bitblt(x, y, width, height, source, src_x, src_y);
			API::refresh_window(this->handle());
		}

		void picture::set_shadow_background(unsigned begin_color, unsigned end_color, bool horizontal)
		{
			get_drawer_trigger().set_shadow_background(begin_color, end_color, horizontal);
		}

		void picture::transparent(bool v)
		{
			if(API::glass_window(this->handle(), v))
				API::refresh_window(this->handle());
		}

		bool picture::transparent() const
		{
			return API::glass_window(this->handle());
		}
	//end class picture
}//end namespace gui
}//end namespace nana
