/*
 *	Window Layout Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/detail/window_layout.hpp
 *
 */

#ifndef NANA_GUI_DETAIL_WINDOW_LAYOUT_HPP
#define NANA_GUI_DETAIL_WINDOW_LAYOUT_HPP

#include <nana/gui/basis.hpp>

#include "native_window_interface.hpp"
#include "basic_window.hpp"
#include "../layout_utility.hpp"

namespace nana{	namespace gui{
namespace detail
{

	//class window_layout
	template<typename CoreWindow>
	class window_layout
	{
	public:
		typedef CoreWindow	core_window_t;

		struct wd_rectangle
		{
			core_window_t * window;
			rectangle r;
		};
	public:
		static void paint(core_window_t* wd, bool is_redraw)
		{
			if(wd->flags.glass)
			{
				if(is_redraw)
				{
					for(wd = wd->parent; wd; wd = wd->parent) //to avoid recursion
					{
						if(false == wd->flags.glass)
						{
							paint(wd, true);
							return;
						}
					}
				}
				else
					_m_paint_glass_window(wd, false);
			}
			else
			{
				if(is_redraw)
				{
					if(wd->flags.refreshing)	return;

					wd->flags.refreshing = true;

					wd->drawer.refresh();
					wd->flags.refreshing = false;
				}
				maproot(wd);
			}
		}

		static bool maproot(core_window_t* wd)
		{
			nana::rectangle vr;
			if(read_visual_rectangle(wd, vr))
			{
				//get the root graphics
				nana::paint::graphics& root_graph = *(wd->root_graph);

				if(wd->other.category != category::lite_widget_tag::value)
					root_graph.bitblt(vr, wd->drawer.graphics, nana::point(vr.x - wd->root_x, vr.y - wd->root_y));

				_m_paste_children(wd, vr, root_graph, nana::point());

				if(wd->parent)
				{
					std::vector<wd_rectangle>	blocks;
					blocks.reserve(10);
					if(read_overlaps(wd, vr, blocks))
					{
						nana::point p_src;
						for(auto & el : blocks)
						{
							if(el.window->other.category == category::frame_tag::value)
							{
								native_window_type container = el.window->other.attribute.frame->container;
								native_interface::refresh_window(container);
								root_graph.bitblt(el.r, container);
							}
							else
							{
								p_src.x = el.r.x - el.window->root_x;
								p_src.y = el.r.y - el.window->root_y;
								root_graph.bitblt(el.r, (el.window->drawer.graphics), p_src);
							}

							_m_paste_children(el.window, el.r, root_graph, nana::point());
						}
					}
				}
				_m_notify_glasses(wd, vr);
				return true;
			}
			return false;
		}

		static void paste_children_to_graphics(core_window_t* wd, nana::paint::graphics& graph)
		{
			nana::rectangle vis_rect(wd->root_x, wd->root_y, wd->rect.width, wd->rect.height);
			_m_paste_children(wd, vis_rect, graph, nana::point(wd->root_x, wd->root_y));
		}

		//read_visual_rectangle
		//@brief:	Reads the visual rectangle of a window, the visual rectangle's reference frame is to root widget,
		//			the visual rectangle is a rectangular block that a window should be displayed on screen.
		//			The result is a rectangle that is a visible area for its ancesters.
		static bool read_visual_rectangle(core_window_t* wd, nana::rectangle& visual)
		{
			if(false == wd->visible)	return false;

			visual.x = wd->root_x;
			visual.y = wd->root_y;
			visual.width = wd->rect.width;
			visual.height = wd->rect.height;

			if(wd->root_widget != wd)
			{
				//Test if the root widget is overlapped the specified widget
				//the pos of root widget is (0, 0)
				if(nana::gui::overlap(visual, wd->root_widget->rect) == false)
					return false;
			}

			for(const core_window_t* parent = wd->parent; parent; parent = parent->parent)
			{
				nana::rectangle self_rect = visual;
				nana::rectangle prnt_rect(parent->root_x, parent->root_y,parent->rect.width, parent->rect.height);
				nana::gui::overlap(prnt_rect, self_rect, visual);
			}

			return true;
		}

		//read_overlaps
		//	reads the overlaps that are overlapped a rectangular block
		static bool read_overlaps(core_window_t* wd, const nana::rectangle& vis_rect, std::vector<wd_rectangle>& blocks)
		{
			wd_rectangle block;
			nana::rectangle rect;
			while(wd->parent)
			{
				typename core_window_t::cont_type::value_type *end = &(wd->parent->children[0]) + wd->parent->children.size();
				typename core_window_t::cont_type::value_type *i = std::find(&(wd->parent->children[0]), end, wd);

				++i;	//move to the next widget
				for(; i != end; ++i)
				{
					core_window_t* cover = *i;

					if(cover->visible && (cover->flags.glass == false))
					{
						rect.x = cover->root_x;
						rect.y = cover->root_y;
						rect.width = cover->rect.width;
						rect.height = cover->rect.height;

						if(nana::gui::overlap(vis_rect, rect, block.r))
						{
							block.window = cover;
							blocks.push_back(block);
						}
					}
				}
				wd = wd->parent;
			}

			return (blocks.size() != 0);
		}

		static bool glass_window(core_window_t * wd, bool isglass)
		{
			if((wd->other.category == category::widget_tag::value) && (wd->flags.glass != isglass))
			{
				auto i = std::find(data_sect.glass_window_cont.begin(), data_sect.glass_window_cont.end(), wd);

				if(i != data_sect.glass_window_cont.end())
				{
					if(false == isglass)
					{
						data_sect.glass_window_cont.erase(i);
						wd->other.glass_buffer.release();
					}
					else
						wd->flags.glass = true;
					return true;
				}

				data_sect.glass_window_cont.push_back(wd);
				wd->flags.glass = true;
				return true;
			}
			return false;
		}

		//make_glass
		//		Used for making a transparent background window.
		//	@copy_from_buffer: To enhance the performance, the background graphics would not be re-evaluated if copying from buffer.
		//	@final_copy: Let wd background update from buffer if it is true. Generally, final_copy always is true, but when window manager is
		//				notifying a glass window whose parent or slibing windows was changed, the argument is false for performance.
		//				That because there is a copying from buffer while notifying in the trigger of widgets.
		static void make_glass(core_window_t* wd, bool copy_from_buffer, bool final_copy)
		{
			if(wd && wd->parent && wd->flags.glass)
			{
				nana::paint::graphics & glass_buffer = wd->other.glass_buffer;
				if(glass_buffer.empty() || glass_buffer.size() != nana::size(wd->rect.width, wd->rect.height))
				{
					glass_buffer.make(wd->rect.width, wd->rect.height);
					copy_from_buffer = false;
				}

				if(false == copy_from_buffer)
				{
					nana::point rpos(wd->root_x, wd->root_y);

					if(wd->parent->other.category == category::lite_widget_tag::value)
					{
						std::vector<core_window_t*> layers;
						core_window_t * beg = wd->parent;
						while(beg && (beg->other.category == category::lite_widget_tag::value))
						{
							layers.push_back(beg);
							beg = beg->parent;
						}

						glass_buffer.bitblt(nana::rectangle(0, 0, wd->rect.width, wd->rect.height), beg->drawer.graphics, nana::point(wd->root_x - beg->root_x, wd->root_y - beg->root_y));

						nana::rectangle r = wd->rect;
						typename std::vector<core_window_t*>::reverse_iterator layers_rend = layers.rend();

						for(typename std::vector<core_window_t*>::reverse_iterator i = layers.rbegin(); i != layers_rend; ++i)
						{
							core_window_t * pre = *i;
							if(pre->visible)
							{
								core_window_t * term = ((i + 1 != layers_rend) ? *(i + 1) : wd);
								r.x = wd->root_x - pre->root_x;
								r.y = wd->root_y - pre->root_y;
								for(auto child: pre->children)
								{
									if(child->index < term->index)
									{
										nana::rectangle ovlp;
										if(child->visible && nana::gui::overlap(r, child->rect, ovlp))
										{
											if(child->other.category != category::lite_widget_tag::value)
												glass_buffer.bitblt(nana::rectangle(ovlp.x - pre->rect.x, ovlp.y - pre->rect.y, ovlp.width, ovlp.height), child->drawer.graphics, nana::point(ovlp.x - child->rect.x, ovlp.y - child->rect.y));
											ovlp.x += pre->root_x;
											ovlp.y += pre->root_y;
											_m_paste_children(child, ovlp, glass_buffer, rpos);
										}
									}
									else
										break;
								}
							}
						}
					}
					else
						glass_buffer.bitblt(nana::rectangle(0, 0, wd->rect.width, wd->rect.height), wd->parent->drawer.graphics,  nana::point(wd->rect.x, wd->rect.y));

					for(auto child : wd->parent->children)
					{
						if(child->index < wd->index)
						{
							nana::rectangle ovlp;
							if(child->visible && nana::gui::overlap(wd->rect, child->rect, ovlp))
							{
								if(child->other.category != category::lite_widget_tag::value)
									glass_buffer.bitblt(nana::rectangle(ovlp.x - wd->rect.x, ovlp.y - wd->rect.y, ovlp.width, ovlp.height), child->drawer.graphics, nana::point(ovlp.x - child->rect.x, ovlp.y - child->rect.y));

								ovlp.x += wd->root_x;
								ovlp.y += wd->root_y;
								_m_paste_children(child, ovlp, glass_buffer, rpos);
							}
						}
						else
							break;
					}
				}

				if(final_copy)
					wd->drawer.graphics.bitblt(0, 0, glass_buffer);
			}
		}
	private:

		//_m_paste_children
		//@brief:paste children window to the root graphics directly. just paste the visual rectangle
		static void _m_paste_children(core_window_t* wd, const nana::rectangle& parent_rect, nana::paint::graphics& graph, const nana::point& graph_rpos)
		{
			nana::rectangle rect;
			nana::rectangle child_rect;

			for(typename core_window_t::container_type::iterator i = wd->children.begin(), end = wd->children.end(); i != end; ++i)
			{
				core_window_t * child = *i;

				//it will not past children if no drawer and visible is false.
				if((false == child->visible) || (child->drawer.graphics.empty() && (child->other.category != category::lite_widget_tag::value)))
					continue;

				if(false == child->flags.glass)
				{
					child_rect.x = child->root_x;
					child_rect.y = child->root_y;
					child_rect.width = child->rect.width;
					child_rect.height = child->rect.height;

					if(nana::gui::overlap(child_rect, parent_rect, rect))
					{
						if(child->other.category != category::lite_widget_tag::value)
						{
							graph.bitblt(nana::rectangle(rect.x - graph_rpos.x, rect.y - graph_rpos.y, rect.width, rect.height),
										child->drawer.graphics, nana::point(rect.x - child->root_x, rect.y - child->root_y));
						}

						_m_paste_children(child, rect, graph, graph_rpos);
					}
				}
				else
					_m_paint_glass_window(child, true);
			}
		}

		static void _m_paint_glass_window(core_window_t* wd, bool is_redraw)
		{
			if(wd->flags.refreshing && is_redraw)	return;

			nana::rectangle vis_rect;
			if(read_visual_rectangle(wd, vis_rect))
			{
				if(is_redraw)
				{
					wd->flags.refreshing = true;
					make_glass(wd, false, false);
					wd->drawer.notify_background_change();
					wd->drawer.refresh();
					wd->flags.refreshing = false;
				}

				nana::paint::graphics& root_graph = *(wd->root_graph);

				//Map Root
				root_graph.bitblt(vis_rect, wd->drawer.graphics, nana::point(vis_rect.x - wd->root_x, vis_rect.y - wd->root_y));
				_m_paste_children(wd, vis_rect, root_graph, nana::point());

				if(wd->parent)
				{
					std::vector<wd_rectangle>	blocks;
					read_overlaps(wd, vis_rect, blocks);
					for(auto & n : blocks)
					{
						root_graph.bitblt(n.r, (n.window->drawer.graphics), nana::point(n.r.x - n.window->root_x, n.r.y - n.window->root_y));
					}
				}
			}
		}

		//_m_notify_glasses
		//@brief:	Notify the glass windows that are overlapped with the specified vis_rect
		static void _m_notify_glasses(core_window_t* const sigwnd, const nana::rectangle& r_visual)
		{
			if(0 == data_sect.glass_window_cont.size())
				return;

			for(auto wd : data_sect.glass_window_cont)
			{
				if(wd == sigwnd || !wd->visible ||
					(false == nana::gui::overlap(wd->root_x, wd->root_y, wd->rect.width, wd->rect.height, sigwnd->root_x, sigwnd->root_y, sigwnd->rect.width, sigwnd->rect.height)))
					continue;

				//Test a parent of the glass window is invisible.
				bool visible = true;
				for(core_window_t *p = wd->parent; p; p = p->parent)
				{
					if(false == p->visible)
					{
						visible = false;
						break;
					}
				}

				if(visible)
				{
					if(sigwnd->parent == wd->parent)
					{
						if(sigwnd->index < wd->index)
							_m_paint_glass_window(wd, true);
					}
					else if(sigwnd == wd->parent)
					{
						_m_paint_glass_window(wd, true);
					}
					else
					{
						//test if sigwnd is a parent of glass window x, or a slibing of the glass window, or a child of the slibing of the glass window.
						core_window_t *p = wd->parent;
						core_window_t * signode = sigwnd;
						while(signode->parent && (signode->parent != p))
							signode = signode->parent;

						if(signode->parent && (signode->index < wd->index))
							_m_paint_glass_window(wd, true);
					}
				}
			}
		}
	private:
		struct data_section
		{
			std::vector<core_window_t*> 	glass_window_cont;
		};
		static data_section	data_sect;
	};//end class window_layout

	template<typename CoreWindow>
	typename window_layout<CoreWindow>::data_section window_layout<CoreWindow>::data_sect;
}//end namespace detail
}//end namespace gui
}//end namespace nana

#endif //NANA_GUI_DETAIL_WINDOW_LAYOUT_HPP

