/*
 *	A Scroll Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/scroll.cpp
 */

#include <nana/gui/widgets/scroll.hpp>

namespace nana{ namespace gui{

	namespace drawerbase
	{
		namespace scroll
		{
		//struct metrics_type
			metrics_type::metrics_type()
				:peak(1), range(1), step(1), value(0),
				what(-1), scroll_length(0), scroll_pos(0), trace(0), mouse_pos(0)
			{}
		//end struct metrics_type

		//class drawer
			drawer::drawer(metrics_type& m)
				:metrics_(m)
			{}

			void drawer::set_vertical(bool v)
			{
				vertical_ = v;
			}

			int drawer::what(drawer::graph_reference graph, int x, int y)
			{
				unsigned scale;
				int pos;

				if(vertical_)
				{
					scale = graph.height();
					pos = y;
				}
				else
				{
					scale = graph.width();
					pos = x;
				}

				if(scale >= fixedsize * 2)
				{
					if(pos < static_cast<int>(fixedsize))
						return buttons::first;
					if(pos > static_cast<int>(scale - fixedsize))
						return buttons::second;
				}
				else
				{
					if(pos < static_cast<int>(scale / 2))
						return buttons::first;
					if(pos > static_cast<int>(scale / 2))
						return buttons::second;
				}

				if(metrics_.scroll_length)
				{
					//int spos = (metrics_.value * (scale - fixedsize * 2 - metrics_.scroll_length)) /(metrics_.peak - metrics_.range);
					if(metrics_.scroll_pos + static_cast<int>(fixedsize) <= pos && pos < metrics_.scroll_pos + static_cast<int>(fixedsize + metrics_.scroll_length))
						return buttons::scroll;
				}

				return buttons::none;
			}
		//public:
			void drawer::draw(drawer::graph_reference graph, int what, bool moused, int delta_pos)
			{
				if(false == _m_auto_scroll(graph))
				{
					_m_adjust_scroll(graph, delta_pos);
					_m_background(graph, true);
				}
				else
					_m_background(graph, false);

				unsigned width, height;
				int x, y;
				if(vertical_)
				{
					x = 0;
					y = graph.height() - fixedsize;
					width = graph.width();
					height = fixedsize;
				}
				else
				{
					x = graph.width() - fixedsize;
					y = 0;
					width = fixedsize;
					height = graph.height();
				}

				int state = ((_m_check() == false || what == -1) ? states::none : states::highlight);
				int moused_state = (_m_check() ? (moused ? states::selected : states::actived) : states::none);

				//draw first
				_m_draw_button(graph, 0, 0, width, height, buttons::first, (buttons::first == what ? moused_state : state));

				//draw second
				_m_draw_button(graph, x, y, width, height, buttons::second, (buttons::second == what ? moused_state : state));

				//draw scroll
				_m_draw_scroll(graph, (buttons::scroll == what ? moused_state : states::highlight), delta_pos);
				
			}
		//private:
			bool drawer::_m_auto_scroll(drawer::graph_reference graph)
			{
				if((2 == metrics_.trace) && _m_check())
				{
					if(metrics_.mouse_pos <= metrics_.scroll_pos + static_cast<int>(fixedsize))
					{	//backward
						if(metrics_.value <= metrics_.range)
							metrics_.value = 0;
						else
							metrics_.value -= metrics_.range;
					}
					else
					{
						if(metrics_.peak - metrics_.range - metrics_.value <= metrics_.range)
							metrics_.value = metrics_.peak - metrics_.range;
						else
							metrics_.value += metrics_.range;
					}
					return true;
				}
				
				return false;
			}

			void drawer::_m_background(drawer::graph_reference graph, bool normal)
			{
				graph.rectangle(0, 0, graph.width(), graph.height(), 0xF0F0F0, true);

				if(false == normal && _m_check())
				{
					int x = 0, y = 0;
					unsigned width = graph.width(), height = graph.height();

					if(metrics_.mouse_pos <= metrics_.scroll_pos + static_cast<int>(fixedsize))
					{
						*(vertical_ ? &y : &x) = fixedsize;
						*(vertical_ ? &height: &width) = metrics_.scroll_pos;
					}
					else
					{
						*(vertical_ ? &y : &x) = static_cast<int>(fixedsize + metrics_.scroll_pos + metrics_.scroll_length);
						*(vertical_ ? &height: &width) = static_cast<unsigned>((vertical_ ? graph.height() : graph.width()) - (fixedsize * 2 + metrics_.scroll_pos + metrics_.scroll_length));
					}

					if(width && height)
						graph.rectangle(x, y, width, height, 0xDCDCDC, true);
				}
			}

			void drawer::_m_button_frame(drawer::graph_reference graph, int x, int y, unsigned width, unsigned height, int state)
			{
				if(state)
				{
					unsigned color = 0x979797; //highlight
					switch(state)
					{
					case states::actived:
						color = 0x86D5FD; break;
					case states::selected:
						color = 0x3C7FB1; break;
					}
					
					int inner_x = x + width - 2;
					int inner_y = y + height - 2;
					
					graph.line(x + 1, y, inner_x, y, color);
					graph.line(x + 1, inner_y + 1, inner_x, inner_y + 1, color);
					graph.line(x, y + 1, x, inner_y, color);
					graph.line(inner_x + 1, y + 1, inner_x + 1, inner_y, color);

					unsigned color_x = graph.mix(color, 0xFFFFFF, 0.5);

					if(vertical_)
					{
						unsigned half = (width - 4) / 2;
						graph.rectangle(x + 2 + (width - 4 - half), y + 2, half, height - 4, color_x, true);
						graph.shadow_rectangle(x + 2, y + 2, width - 4 - half, height - 4, 0xFFFFFF, color_x, false); 
					}
					else
					{
						unsigned half = (height - 4) / 2;
						graph.rectangle(x + 2, y + 2 + height - 4 - half, width - 4, half, color_x, true);
						graph.shadow_rectangle(x + 2, y + 2, width - 4, height - 4 - half, 0xFFFFFF, color_x, true); 				
					}
				}
			}

			bool drawer::_m_check() const
			{
				return (metrics_.scroll_length && metrics_.range && (metrics_.peak > metrics_.range));
			}

			void drawer::_m_adjust_scroll(drawer::graph_reference graph, int delta_pos)
			{
				if(metrics_.range == 0 || metrics_.peak <= metrics_.range) return;

				unsigned scale = vertical_ ? graph.height() : graph.width();

				if(scale > fixedsize * 3)
				{
					scale -= (fixedsize * 2);

					metrics_.scroll_length = (scale * metrics_.range / metrics_.peak);
					
					if(metrics_.scroll_length < fixedsize)
						metrics_.scroll_length = fixedsize;

					if(metrics_.scroll_length)
					{
						if(0 == delta_pos)
						{
							if(metrics_.value == 0)
								metrics_.scroll_pos = 0;
							else if(metrics_.value + metrics_.range >= metrics_.peak)
							{
								metrics_.value = metrics_.peak - metrics_.range;
								metrics_.scroll_pos = static_cast<int>(scale - metrics_.scroll_length);
							}
							else
								metrics_.scroll_pos = static_cast<int>((metrics_.value * (scale - metrics_.scroll_length)) /(metrics_.peak - metrics_.range));
						}
					}
					else
						metrics_.scroll_pos = 0;
				}
				else
				{
					metrics_.scroll_pos = 0;
					metrics_.scroll_length = 0;
				}
			}

			void drawer::_m_draw_scroll(drawer::graph_reference graph, int state, int delta_pos)
			{
				if(_m_check())
				{
					unsigned scale = vertical_ ? graph.height() : graph.width();

					if(scale > fixedsize * 2)
					{
						scale -= (fixedsize * 2);

						if(delta_pos)
						{
							int pos = metrics_.scroll_pos + delta_pos;

							if(pos < 0)
								delta_pos = - metrics_.scroll_pos;
							else if(pos > static_cast<int>(scale - metrics_.scroll_length))
								delta_pos = static_cast<int>(scale - metrics_.scroll_length - metrics_.scroll_pos);
						}

						int x, y;
						unsigned width, height;

						if(vertical_)
						{
							x = 0;
							y = fixedsize + metrics_.scroll_pos + delta_pos;

							width = graph.width();
							height = static_cast<unsigned>(metrics_.scroll_length);
						}
						else
						{
							x = fixedsize + metrics_.scroll_pos + delta_pos;
							y = 0;

							width = static_cast<unsigned>(metrics_.scroll_length);
							height = graph.height();						
						}

						_m_button_frame(graph, x, y, width, height, state);

						if(delta_pos)
						{
							metrics_.value = (metrics_.scroll_pos + delta_pos) * (metrics_.peak - metrics_.range) / (scale - metrics_.scroll_length);
							if(metrics_.value < metrics_.peak - metrics_.range)
							{
								int selfpos = static_cast<int>(metrics_.value * (scale - metrics_.scroll_length) / (metrics_.peak - metrics_.range));
								int nextpos = static_cast<int>((metrics_.value + 1) * (scale - metrics_.scroll_length) / (metrics_.peak - metrics_.range));

								if(selfpos != nextpos && (metrics_.scroll_pos + delta_pos - selfpos > nextpos - (metrics_.scroll_pos + delta_pos)))
									++metrics_.value;
							}
						}
					}
				}
			}

			void drawer::_m_draw_button(drawer::graph_reference graph, int x, int y, unsigned width, unsigned height, int what, int state)
			{
				if(_m_check())
					_m_button_frame(graph, x, y, width, height, state);
				
				using namespace nana::paint::gadget;

				if(buttons::first == what || buttons::second == what)
				{
					directions::t dir;
					if(buttons::second == what)
					{
						if(vertical_)
						{
							y = static_cast<int>(graph.height() - fixedsize);
							dir = directions::to_south;
						}
						else
						{
							x = static_cast<int>(graph.width() - fixedsize);
							dir = directions::to_east;
						}
					}
					else
						dir = vertical_ ? directions::to_north : directions::to_west;

					if(vertical_)
						x = (static_cast<int>(graph.width()) - 16) / 2;
					else
						y = (static_cast<int>(graph.height()) - 16) / 2;
					
					arrow_16_pixels(graph, x, y, _m_check() ? 0x0 : 0x808080, (states::none == state ? 0 : 1), dir);
				}
			}
		//end class drawer
		}//end namespace scroll
	}//end namespace drawerbase
}//end namespace gui
}//end namespace nana

