/*
 *	A Treebox Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/treebox.cpp
 */
#include <nana/gui/widgets/treebox.hpp>
#include <nana/gui/element.hpp>
#include <nana/system/platform.hpp>
#include <stdexcept>

namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		//Here defines some function objects
		namespace treebox
		{
			typedef trigger::node_type node_type;

				bool no_sensitive_compare(const nana::string& text, const nana::char_t *pattern, std::size_t len)
				{
					if(len <= text.length())
					{
						const nana::char_t * s = text.c_str();
						for(std::size_t i = 0; i < len; ++i)
						{
							if('a' <= s[i] && s[i] <= 'z')
							{
								if(pattern[i] != s[i] - ('a' - 'A'))
									return false;
							}
							else
								if(pattern[i] != s[i]) return false;
						}
						return true;
					}
					return false;
				}

				const node_type* find_track_child_node(const node_type* node, const node_type * end, const nana::char_t* pattern, std::size_t len, bool &finish)
				{
					if(node->value.second.expanded)
					{
						node = node->child;

						while(node)
						{
							if(no_sensitive_compare(node->value.second.text, pattern, len)) return node;

							if(node == end) break;

							if(node->value.second.expanded)
							{
								auto t = find_track_child_node(node, end, pattern, len, finish);
								if(t || finish)
									return t;
							}
							node = node->next;
						}
					}

					finish = (node && (node == end));
					return nullptr;
				}

			//struct implement
			//@brief:	some data for treebox trigger
			template<typename Renderer>
			struct trigger::basic_implement
			{
				typedef trigger::node_type node_type;

				struct essence_tag
				{
					nana::paint::graphics * graph;
					widget * widget_ptr;
					nana::pat::cloneable_interface<renderer_interface> * renderer;
					bool stop_drawing;
				}data;

				struct shape_tag
				{
					nana::upoint border;
					gui::scroll<true> scroll;
					std::size_t	prev_first_value;

					mutable std::map<nana::string, node_image_tag> image_table;

					tree_cont_type::node_type * first;
					int indent_pixels;
					int offset_x;
					int item_offset;	//the offset of a item that is a distance from start pos
					int text_offset;	//the offset of text that is a distance from the item

					std::size_t crook_pixels;
					std::size_t image_pixels;
				}shape;

				struct attribute_tag
				{
					bool auto_draw;
					mutable ext_event_type ext_event;
					//mutable std::size_t visual_item_size;
					std::size_t button_pixels;
					tree_cont_type tree_cont;
				}attr;

				struct node_state_tag
				{
					tooltip_window * tooltip;
					component	comp_pointed;
					tree_cont_type::node_type * pointed;
					tree_cont_type::node_type * selected;
					tree_cont_type::node_type * event_node;
				}node_state;

				struct track_node_tag
				{
					nana::string key_buf;
					std::size_t key_time;
				}track_node;

				struct adjust_tag
				{
					int offset_x_adjust;	//It is a new value of offset_x, and offset_x will be djusted to the new value
					tree_cont_type::node_type * node;
					std::size_t scroll_timestamp;
					gui::timer timer;
				}adjust;
			public:
				basic_implement()
				{
					data.graph = nullptr;
					data.widget_ptr = nullptr;
					data.renderer = nullptr;
					data.stop_drawing = false;

					shape.prev_first_value = 0;
					shape.first = nullptr;
					shape.indent_pixels = 10;
					shape.offset_x = 0;
					shape.item_offset = 16;
					shape.text_offset = 3;
					shape.crook_pixels = 0;
					shape.image_pixels = 0;

					attr.auto_draw = true;
					//attr.visual_item_size = 0;
					attr.button_pixels = 16;

					node_state.tooltip = nullptr;
					node_state.comp_pointed = component::end;
					node_state.pointed = nullptr;
					node_state.selected = nullptr;
					node_state.event_node = nullptr;

					track_node.key_time = 0;

					adjust.offset_x_adjust = 0;
					adjust.node = nullptr;
					adjust.scroll_timestamp = 0;
				}

				bool draw(bool scrollbar_react)
				{
					if(data.graph && (false == data.stop_drawing))
					{
						if(scrollbar_react)
							show_scrollbar();

						//Draw background
						data.graph->rectangle(data.widget_ptr->background(), true);

						//Draw tree
						attr.tree_cont.for_each(shape.first, Renderer(this, nana::point(static_cast<int>(attr.tree_cont.indent_size(shape.first) * shape.indent_pixels) - shape.offset_x, 1)));
						return true;
					}
					return false;
				}

				const trigger::node_type* find_track_node(nana::char_t key)
				{
					nana::string pattern;

					if('a' <= key && key <= 'z') key -= 'a' - 'A';

					unsigned long now = nana::system::timestamp();

					if(now - track_node.key_time > 1000)
						track_node.key_buf.clear();

					if(track_node.key_buf.length())
					{
						if(track_node.key_buf[track_node.key_buf.length() - 1] != key)
							pattern = track_node.key_buf;
					}

					track_node.key_time = now;
					pattern += key;
					track_node.key_buf += key;

					const node_type *begin = node_state.selected ? node_state.selected : attr.tree_cont.get_root()->child;

					if(begin)
					{
						const node_type *node = begin;
						const node_type *end = nullptr;
						if(pattern.length() == 1)
						{
							if(node->value.second.expanded && node->child)
							{
								node = node->child;
							}
							else if(nullptr == node->next)
							{
								if(nullptr == node->owner->next)
								{
									end = begin;
									node = attr.tree_cont.get_root()->child;
								}
								else
									node = node->owner->next;
							}
							else
								node = node->next;
						}

						while(node)
						{
							if(no_sensitive_compare(node->value.second.text, pattern.c_str(), pattern.length())) return node;

							bool finish;
							const node_type *child = find_track_child_node(node, end, pattern.c_str(), pattern.length(), finish);
							if(child)
								return child;

							if(finish || (node == end))
								return nullptr;

							if(nullptr == node->next)
							{
								node = (node->owner ? node->owner->next : nullptr);
								if(nullptr == node)
								{
									node = attr.tree_cont.get_root()->child;
									end = begin;
								}
							}
							else
								node = node->next;
						}
					}
					return nullptr;
				}

				static bool check_kinship(const node_type* parent, const node_type * child)
				{
					if(nullptr == parent || nullptr == child)
						return false;

					while(child && (child != parent))
						child = child->owner;

					return (nullptr != child);
				}

				bool make_adjust(node_type * node, int reason)
				{
					if(nullptr == node) return false;

					auto & tree_container = attr.tree_cont;

					switch(reason)
					{
					case 0:
						//adjust if the node expanded and the number of its children are over the max number allowed
						if(shape.first != node)
						{
							unsigned child_size = tree_container.child_size_if(*node, pred_allow_child());
							const std::size_t max_allow = max_allowed();

							if(child_size < max_allow)
							{
								unsigned off1 = tree_container.distance_if(shape.first, pred_allow_child());
								unsigned off2 = tree_container.distance_if(node, pred_allow_child());
								const unsigned size = off2 - off1 + child_size + 1;
								if(size > max_allow)
									shape.first = tree_container.advance_if(shape.first, size - max_allow, pred_allow_child());
							}
							else
								shape.first = node;
						}
						break;
					case 1:
					case 2:
					case 3:
						//param is the begin pos of an item in absolute.
						{
							int beg = static_cast<int>(tree_container.indent_size(node) * shape.indent_pixels) - shape.offset_x;
							int end = beg + static_cast<int>(node_w_pixels(node)) + shape.item_offset;

							bool take_adjust = false;
							if(reason == 1)
								take_adjust = (beg < 0 || (beg > 0 && end > static_cast<int>(visible_w_pixels())));
							else if(reason == 2)
								take_adjust = (beg < 0);
							else if(reason == 3)
								return (beg > 0 && end > static_cast<int>(visible_w_pixels()));

							if(take_adjust)
							{
								adjust.offset_x_adjust = shape.offset_x + beg;
								adjust.node = node;
								adjust.timer.enable(true);
								return true;
							}
						}
						break;
					case 4:
						if(shape.first != node)
						{
							unsigned off_first = tree_container.distance_if(shape.first, pred_allow_child());
							unsigned off_node = tree_container.distance_if(node, pred_allow_child());
							if(off_node < off_first)
							{
								shape.first = node;
								return true;
							}
							else if(off_node - off_first > max_allowed())
							{
								shape.first = tree_container.advance_if(0, off_node - max_allowed() + 1, pred_allow_child());
								return true;
							}
						}
						break;
					}
					return false;
				}

				bool set_selected(node_type * node)
				{
					if(node_state.selected != node)
					{
						data.stop_drawing = true;
						if(node_state.selected)
							attr.ext_event.selected(data.widget_ptr->handle(), reinterpret_cast<ext_event_type::node_type>(node_state.selected), false);

						node_state.selected = node;
						if(node)
							attr.ext_event.selected(data.widget_ptr->handle(), reinterpret_cast<ext_event_type::node_type>(node), true);
						data.stop_drawing = false;
						return true;
					}
					return false;
				}

				bool set_expanded(node_type* node, bool value)
				{
					if(node && node->value.second.expanded != value)
					{
						if(value == false)
						{
							//if contracting a parent of the selected node, select the contracted node.
							if(check_kinship(node, node_state.selected))
								set_selected(node);
						}

						node->value.second.expanded = value;
						if(node->child)
						{
							data.stop_drawing = true;
							attr.ext_event.expand(data.widget_ptr->handle(), reinterpret_cast<ext_event_type::node_type>(node), value);
							data.stop_drawing = false;
						}
						return true;
					}
					return false;
				}

				void show_scrollbar()
				{
					paint::graphics * graph = data.graph;

					if(nullptr == graph) return;

					unsigned max_allow = max_allowed();
					unsigned visual_items = visual_item_size();

					auto & scroll = shape.scroll;
					if(visual_items <= max_allow)
					{
						if(false == scroll.empty())
						{
							scroll.close();
							shape.first = nullptr;
						}
					}
					else
					{
						if(scroll.empty())
						{
							shape.prev_first_value = 0;
							scroll.create(*data.widget_ptr, nana::rectangle(graph->width() - 16, 0, 16, graph->height()));
							auto scroll_fn = nana::make_fun(*this, &basic_implement::event_scrollbar);
							scroll.make_event<events::mouse_down>(scroll_fn);
							scroll.make_event<events::mouse_move>(scroll_fn);
							scroll.make_event<events::mouse_wheel>(scroll_fn);
						}

						scroll.amount(visual_items);
						scroll.range(max_allow);
					}

					scroll.value(attr.tree_cont.distance_if(shape.first, pred_allow_child()));
				}

				void event_scrollbar(const eventinfo& ei)
				{
					if(ei.identifier == events::mouse_wheel::identifier || ei.mouse.left_button)
					{
						if(shape.prev_first_value != shape.scroll.value())
						{
							shape.prev_first_value = static_cast<unsigned long>(shape.scroll.value());
							adjust.scroll_timestamp = nana::system::timestamp();
							adjust.timer.enable(true);

							shape.first = attr.tree_cont.advance_if(nullptr, shape.prev_first_value, pred_allow_child());

							if(ei.window == shape.scroll.handle())
							{
								draw(false);
								API::update_window(data.widget_ptr->handle());
							}
						}
					}
				}

				std::size_t visual_item_size() const
				{
					return attr.tree_cont.child_size_if(STR(""), pred_allow_child());
				}

				unsigned visible_w_pixels() const
				{
					if(nullptr == data.graph)
						return 0;

					return data.graph->width() - (shape.scroll.empty() ? 0 : shape.scroll.size().width);
				}

				unsigned node_w_pixels(const node_type *node) const
				{
					return (static_cast<int>(data.graph->text_extent_size(node->value.second.text).width) + shape.text_offset * 2 + static_cast<unsigned>(shape.crook_pixels + shape.image_pixels));
				}

				unsigned node_h_pixels() const
				{
					return data.graph->text_extent_size(STR("jh{")).height + 8;
				}

				std::size_t max_allowed() const
				{
					return (data.graph->height() / node_h_pixels());
				}

				nana::paint::image* image(const node_type* node)
				{
					const nana::string& idstr = node->value.second.img_idstr;
					if(idstr.size())
					{
						auto i = shape.image_table.find(idstr);
						if(i == shape.image_table.end())
							return nullptr;

						unsigned long state = 0xFFFFFFFF;
						if(node_state.pointed == node && (node_state.comp_pointed == component::text || node_state.comp_pointed == component::crook || node_state.comp_pointed == component::icon))
							state = (node_state.pointed != node_state.selected ? 0: 1);
						else if(node_state.selected == node)
							state = 2;

						node_image_tag & nodeimg = i->second;
						if(node->value.second.expanded || (state == 1 || state == 2))
							if(nodeimg.expanded.empty() == false)	return &nodeimg.expanded;

						if(node->value.second.expanded == false && state == 0)
							if(nodeimg.highlighted.empty() == false)	return &nodeimg.highlighted;

						return &nodeimg.normal;
					}
					return nullptr;
				}

				bool track_mouse(int x, int y)
				{
					int xpos = attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(this, xpos, x, y);
					attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					bool redraw = false;
					node_state.event_node = nl.node();

					if(nl.node() && (nl.what() != component::end))
					{
						if((nl.what() != node_state.comp_pointed || nl.node() != node_state.pointed))
						{
							node_state.comp_pointed = nl.what();
							node_state.pointed = nl.node();
							redraw = (node_state.comp_pointed != component::end);
						}
					}
					else if(node_state.pointed)
					{
						redraw = true;
						node_state.comp_pointed = component::end;
						node_state.pointed = nullptr;
						close_tooltip_window();
					}

					if(redraw)
					{
						if(node_state.comp_pointed == component::text)
						{
							make_adjust(node_state.pointed, 2);
							adjust.scroll_timestamp = 1;

							show_tooltip_window(node_state.pointed, nl.pos(), nl.size());
						}
					}

					return redraw;
				}

				void show_tooltip_window(node_type* node, const nana::point& pos, const nana::size& size)
				{
					close_tooltip_window();

					if((nullptr == node_state.tooltip) && (pos.x + size.width > visible_w_pixels()))
					{
						node_state.tooltip = new tooltip_window(data.widget_ptr->handle(), pos, size);
						node_state.tooltip->show_text(node->value.second.text, shape.text_offset + static_cast<int>(shape.image_pixels + shape.crook_pixels), (node == node_state.selected), this->image(node));

						node_state.tooltip->make_event<events::mouse_leave>(*this, &basic_implement::close_tooltip_window);
						node_state.tooltip->make_event<events::mouse_move>(*this, &basic_implement::mouse_move_tooltip_window);

						auto click_fn = nana::make_fun(*this, &basic_implement::click_tooltip_window);
						node_state.tooltip->make_event<events::mouse_down>(click_fn);
						node_state.tooltip->make_event<events::mouse_up>(click_fn);
						node_state.tooltip->make_event<events::dbl_click>(click_fn);
					}
				}

				void close_tooltip_window()
				{
					if(node_state.tooltip)
					{
						tooltip_window * x = node_state.tooltip;
						node_state.tooltip = nullptr;
						delete x;
					}
				}

				void mouse_move_tooltip_window()
				{
					nana::point pos = API::cursor_position();
					API::calc_window_point(data.widget_ptr->handle(), pos);

					if(pos.x >= static_cast<int>(visible_w_pixels()))
						close_tooltip_window();
				}

				void click_tooltip_window(const eventinfo& ei)
				{
					bool redraw = false;
					switch(ei.identifier)
					{
					case events::mouse_down::identifier:
						if(make_adjust(node_state.pointed, 1))
							adjust.scroll_timestamp = 1;
						break;
					case events::mouse_up::identifier:
						if(node_state.selected != node_state.pointed)
						{
							set_selected(node_state.pointed);
							redraw = true;
						}
						break;
					default:
						set_expanded(node_state.selected, !node_state.selected->value.second.expanded);
						redraw = true;
					}

					if(redraw)
					{
						draw(false);
						API::update_window(data.widget_ptr->handle());
					}
				}
			}; //end struct trigger::implement;

			class internal_renderer
				: public renderer_interface
			{
				void render(graph_reference graph, nana::color_t bgcolor, nana::color_t fgcolor, const compset_interface * compset) const override
				{
					comp_attribute_t attr;
					if(compset->comp_attribute(component::expender, attr))
					{
						using namespace nana::paint;

						uint32_t style = 1;
						gadget::directions::t dir = gadget::directions::to_southeast;
						if(! compset->item_attribute().expended)
						{
							style = 0;
							dir = gadget::directions::to_east;
						}
						gadget::arrow_16_pixels(graph, attr.area.x, attr.area.y + (attr.area.height - 16) / 2, (attr.mouse_pointed ? 0x1CC4F7 : 0x0), style, dir);
					}

					if(compset->comp_attribute(component::bground, attr))
					{
						const nana::color_t color_table[][2] = {	{0xE8F5FD, 0xD8F0FA}, //highlighted
															{0xC4E8FA, 0xB6E6FB}, //Selected and highlighted
															{0xD5EFFC, 0x99DEFD}  //Selected but not highlighted
														};

						const nana::color_t *colptr = nullptr;
						if(compset->item_attribute().mouse_pointed)
						{
							if(compset->item_attribute().selected)
								colptr = color_table[1];
							else
								colptr = color_table[0];
						}
						else if(compset->item_attribute().selected)
							colptr = color_table[2];

						if(colptr)
						{
							graph.rectangle(attr.area, colptr[1], false);
							graph.rectangle(attr.area.pare_off(1), *colptr, true);
						}
					}

					if(compset->comp_attribute(component::crook, attr))
					{
						crook_.draw(graph, bgcolor, fgcolor, attr.area, attr.mouse_pointed ? element_state::hovered : element_state::normal);
					}

					if(compset->comp_attribute(component::icon, attr))
					{
						compset->item_attribute().icon.paste(graph, attr.area.x, attr.area.y);
					}

					if(compset->comp_attribute(component::text, attr))
					{
						graph.string(attr.area.x, attr.area.y + 3, fgcolor, compset->item_attribute().text);
					}
				}

			private:
				mutable facade<element::crook> crook_;
			};

			class trigger::item_locator
			{
			public:
				typedef tree_cont_type::node_type node_type;

				item_locator(implement * impl, int item_pos, int x, int y)
					:impl_(impl), item_pos_(item_pos), item_ypos_(1), pos_(x, y), what_(component::end), node_(nullptr)
				{}

				int operator()(node_type &node, int affect)
				{
					auto & node_desc = impl_->shape;

					switch(affect)
					{
					case 0: break;
					case 1: item_pos_ += static_cast<int>(node_desc.indent_pixels); break;
					default:
						if(affect >= 2)
							item_pos_ -= static_cast<int>(node_desc.indent_pixels) * (affect - 1);
					}

					if((pos_.y - item_ypos_) < static_cast<int>(impl_->node_h_pixels()))
					{
						node_ = &node;

						if((item_pos_ < pos_.x) && (pos_.x < item_pos_ + node_desc.item_offset))
						{
							what_ = (node.child ? component::expender : component::end);
							return 0;
						}

						int comp_pos = item_pos_ + node_desc.item_offset;

						if(node_desc.crook_pixels && (comp_pos <= pos_.x) && (pos_.x < comp_pos + static_cast<int>(node_desc.crook_pixels)))
						{
							what_ = component::crook;
							return 0;
						}

						comp_pos += static_cast<int>(node_desc.crook_pixels);
						if(node_desc.image_pixels && (comp_pos <= pos_.x) && (pos_.x < comp_pos + static_cast<int>(node_desc.image_pixels)))
							what_ = component::icon;
						else if((item_pos_ + node_desc.item_offset <= pos_.x) && (pos_.x < item_pos_ + node_desc.item_offset + static_cast<int>(impl_->node_w_pixels(&node))))
							what_ = component::text;
						else
							what_ = component::end;

						return 0;
					}

					item_ypos_ += impl_->node_h_pixels();

					if(node.value.second.expanded && node.child)
						return 1;

					return 2;
				}

				node_type * node() const
				{
					return node_;
				}

				component what() const
				{
					return what_;
				}

				bool item_body() const
				{
					return (component::text == what_ || component::icon == what_);
				}

				nana::point pos() const
				{
					return nana::point(item_pos_ + impl_->shape.item_offset, item_ypos_);
				}

				nana::size size() const
				{
					return nana::size(static_cast<int>(impl_->node_w_pixels(node_)), impl_->node_h_pixels());
				}
			private:
				trigger::implement * impl_;
				int item_pos_;
				int item_ypos_;
				nana::point pos_;
				component	what_;
				node_type * node_;
			};

			class trigger::item_renderer
				: public compset_interface
			{
			public:
				typedef tree_cont_type::node_type node_type;

				item_renderer(implement * impl, const nana::point& pos)
					:impl_(impl), pos_(pos), node_height_pixels_(impl->node_h_pixels())
				{
					bgcolor_ = impl_->data.widget_ptr->background();
					fgcolor_ = impl_->data.widget_ptr->foreground();
				}

				//affect
				//0 = Sibling, the last is a sibling of node
				//1 = Owner, the last is the owner of node
				//>=2 = Children, the last is a child of a node that before this node.
				int operator()(const node_type& node, int affect)
				{
					implement * draw_impl = impl_;

					iterated_node_ = &node;
					switch(affect)
					{
					case 1:
						pos_.x += draw_impl->shape.indent_pixels;
						break;
					default:
						if(affect >= 2)
							pos_.x -= draw_impl->shape.indent_pixels * (affect - 1);
					}

					item_attr_.expended = node.value.second.expanded;
					item_attr_.text = node.value.second.text;
					item_attr_.checked = false;
					item_attr_.mouse_pointed = (draw_impl->node_state.pointed == iterated_node_);
					item_attr_.selected = (draw_impl->node_state.selected == iterated_node_);

					draw_impl->data.renderer->refer().render(*draw_impl->data.graph, bgcolor_, fgcolor_, this); 

					pos_.y += node_height_pixels_;

					if(pos_.y > static_cast<int>(draw_impl->data.graph->height()))
						return 0;

					return (node.child && node.value.second.expanded ? 1 : 2);
				}
			private:
				//Overrides compset_interface
				virtual const item_attribute_t& item_attribute() const override
				{
					return item_attr_;
				}

				virtual bool comp_attribute(component_t comp, comp_attribute_t& attr) const override
				{
					implement * draw_impl = impl_;
					switch(comp)
					{
					case component::expender:
						if(iterated_node_->child)
						{
							attr.area = pos_;
							attr.area.width = 16;
							attr.area.height = node_height_pixels_;
							return true;
						}
						return false;
					case component::bground:
						attr.area.x = pos_.x + draw_impl->shape.item_offset;
						attr.area.y = pos_.y;
						attr.area.width = impl_->node_w_pixels(iterated_node_);
						attr.area.height = node_height_pixels_;
						return true;
					case component::crook:
						if(draw_impl->shape.crook_pixels)
						{
							attr.area.x = pos_.x + draw_impl->shape.item_offset + 2;
							attr.area.y = pos_.y;
							attr.area.width = static_cast<unsigned>(draw_impl->shape.crook_pixels);
							attr.area.height = node_height_pixels_;
							return true;
						}
						return false;
					case component::icon:
						if(draw_impl->shape.image_pixels)
						{
							attr.area.x = pos_.x + draw_impl->shape.item_offset + static_cast<int>(draw_impl->shape.crook_pixels) + 2;
							attr.area.y = pos_.y + 2;
							attr.area.width = static_cast<unsigned>(draw_impl->shape.image_pixels);
							attr.area.height = node_height_pixels_ - 2;
							return true;
						}
						return false;
					case component::text:
						attr.area.x = pos_.x + draw_impl->shape.item_offset + static_cast<int>(draw_impl->shape.image_pixels) + draw_impl->shape.text_offset;
						attr.area.y = pos_.y;
						attr.area.width = impl_->node_w_pixels(iterated_node_) - ( attr.area.x - pos_.x);
						attr.area.height = node_height_pixels_;
						return true;
					default:
						break;
					}
					return false;
				}
			private:
				trigger::implement * impl_;
				nana::color_t bgcolor_;
				nana::color_t fgcolor_;
				nana::point pos_;
				const unsigned node_height_pixels_;
				const node_type * iterated_node_;
				item_attribute_t item_attr_;
			};

			struct pred_allow_child
			{
				bool operator()(const trigger::tree_cont_type::node_type& node)
				{
					return node.value.second.expanded;
				}
			};
		}

		//Treebox Implementation
		namespace treebox
		{

			class tlwnd_drawer
				: public drawer_trigger
			{
			public:
				void text(const nana::string& text, int text_off, bool selected, const nana::paint::image* img)
				{
					text_off_ = text_off;
					selected_ = selected;
					text_ = text;
					draw(img);
				}

				void draw(const nana::paint::image* img)
				{
					nana::rectangle r(graph_->size());

					graph_->rectangle(r, (selected_ ? 0xB6E6FB : 0xD8F0FA), false);
					graph_->rectangle(r.pare_off(1), (selected_ ? 0xC4E8FA : 0xE8F5FD), true);

					graph_->string(text_off_, 3, 0x0, text_);

					if(img) img->paste(*graph_, 2, 2);
				}
			private:
				void bind_window(widget_reference wd)
				{
					widget_ = &wd;
				}

				void attached(graph_reference graph)
				{
					graph_ = &graph;
				}
			private:
				nana::paint::graphics * graph_;
				widget	*widget_;
				int text_off_;
				bool selected_;
				nana::string	text_;
			};//end class tlwnd_drawer

			class tooltip_window
				: public widget_object<category::root_tag, tlwnd_drawer>
			{
			public:
				tooltip_window(window wd, const nana::point& pos, const nana::size& size)
					: widget_object<category::root_tag, tlwnd_drawer>(wd, false, rectangle(pos, size), appear::bald<appear::floating>())
				{
					API::take_active(handle(), false, nullptr);
				}

				void show_text(const nana::string& text, int text_off, bool selected, nana::paint::image* img)
				{
					get_drawer_trigger().text(text, text_off, selected, img);
					show();
				}
			};//end class tooltip_window


			//class trigger
				//struct treebox_node_type
					trigger::treebox_node_type::treebox_node_type()
						:expanded(false)
					{}

					trigger::treebox_node_type::treebox_node_type(const nana::any& v)
						:value(v), expanded(false)
					{}

					trigger::treebox_node_type::treebox_node_type(const nana::string& text, const nana::any& v)
						:text(text), value(v), expanded(false)
					{}

					trigger::treebox_node_type& trigger::treebox_node_type::operator=(const treebox_node_type& rhs)
					{
						if(this != &rhs)
						{
							text = rhs.text;
							value = rhs.value;
							img_idstr = rhs.img_idstr;
						}
						return *this;
					}
				//end struct treebox_node_type

				trigger::trigger()
					:	impl_(new implement)
				{
					impl_->data.renderer = nana::pat::cloneable<internal_renderer, renderer_interface>().clone();
					impl_->adjust.timer.enable(false);
					impl_->adjust.timer.make_tick(std::bind(&trigger::_m_deal_adjust, this));
					impl_->adjust.timer.interval(10);
				}

				trigger::~trigger()
				{
					impl_->data.renderer->self_delete();
					delete impl_;
				}

				void trigger::renderer(const nana::pat::cloneable_interface<renderer_interface>& rd)
				{
					auto new_renderer = rd.clone();
					if(new_renderer)
					{
						if(impl_->data.renderer)
							impl_->data.renderer->self_delete();

						impl_->data.renderer = new_renderer;
					}
				}

				const nana::pat::cloneable_interface<renderer_interface> & trigger::renderer() const
				{
					return *impl_->data.renderer;
				}

				void trigger::auto_draw(bool ad)
				{
					if(impl_->attr.auto_draw != ad)
					{
						impl_->attr.auto_draw = ad;
						if(ad)
							API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				auto trigger::tree() -> tree_cont_type &
				{
					return impl_->attr.tree_cont;
				}

				auto trigger::tree() const -> tree_cont_type const &
				{
					return impl_->attr.tree_cont;
				}

				nana::any & trigger::value(node_type* node) const
				{
					if(impl_->attr.tree_cont.check(node) == false)
						throw std::invalid_argument("Nana.GUI.treebox.value() invalid node");

					return node->value.second.value;
				}

				trigger::node_type* trigger::insert(node_type* node, const nana::string& key, const nana::string& title, const nana::any& v)
				{
					node_type * p = impl_->attr.tree_cont.node(node, key);
					if(p)
					{
						p->value.second.text = title;
						p->value.second.value = v;
					}
					else
						p = impl_->attr.tree_cont.insert(node, key, treebox_node_type(title, v));

					if(p && impl_->attr.auto_draw && impl_->draw(true))
						API::update_window(impl_->data.widget_ptr->handle());
					return p;
				}

				trigger::node_type* trigger::insert(const nana::string& path, const nana::string& title, const nana::any& v)
				{
					auto x = impl_->attr.tree_cont.insert(path, treebox_node_type(title, v));
					if(x && impl_->attr.auto_draw && impl_->draw(true))
						API::update_window(impl_->data.widget_ptr->handle());
					return x;
				}

				bool trigger::check_kinship(node_type* parent, node_type* child) const
				{
					if(false == (parent && child)) return false;

					while(child && (child != parent))
						child = child->owner;

					return (nullptr != child);
				}

				void trigger::remove(node_type* node)
				{
					auto & shape = impl_->shape;
					auto & node_state = impl_->node_state;

					if(check_kinship(node, node_state.event_node))
						node_state.event_node = nullptr;

					if(check_kinship(node, shape.first))
						shape.first = nullptr;

					if(check_kinship(node, node_state.selected))
						node_state.selected = nullptr;

					impl_->attr.tree_cont.remove(node);
				}

				trigger::node_type* trigger::selected() const
				{
					return impl_->node_state.selected;
				}

				void trigger::selected(node_type* node)
				{
					if(impl_->attr.tree_cont.check(node) && impl_->set_selected(node))
					{
						impl_->draw(true);
						API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				void trigger::set_expand(node_type* node, bool exp)
				{
					if((impl_->data.widget_ptr) && impl_->set_expanded(node, exp))
					{
						impl_->draw(true);
						API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				void trigger::set_expand(const nana::string& path, bool exp)
				{
					if(impl_->set_expanded(impl_->attr.tree_cont.find(path), exp))
					{
						impl_->draw(true);
						API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				void trigger::image(const nana::string& id, const node_image_tag& img)
				{
					impl_->shape.image_table[id] = img;
				}

				node_image_tag& trigger::image(const nana::string& id) const
				{
					auto i = impl_->shape.image_table.find(id);
					if(i != impl_->shape.image_table.end())
						return i->second;
					throw std::invalid_argument("Nana.GUI.treebox.image() invalid image identifier");
				}

				void trigger::image_erase(const nana::string& id)
				{
					impl_->shape.image_table.erase(id);
				}

				void trigger::node_image(node_type* node, const nana::string& id)
				{
					if(tree().check(node))
					{
						node->value.second.img_idstr = id;
						auto i = impl_->shape.image_table.find(id);
						if((i != impl_->shape.image_table.end()) && impl_->draw(true))
							API::update_window(impl_->data.widget_ptr->handle());
					}
				}

				unsigned trigger::node_width(const node_type *node) const
				{
					return (static_cast<int>(impl_->data.graph->text_extent_size(node->value.second.text).width) + impl_->shape.text_offset * 2 + static_cast<unsigned>(impl_->shape.crook_pixels + impl_->shape.image_pixels));
				}

				bool trigger::rename(node_type *node, const nana::char_t* key, const nana::char_t* name)
				{
					if((key || name ) && tree().check(node))
					{
						if(key && (key != node->value.first))
						{
							node_type * element = node->owner->child;
							for(; element; element = element->next)
							{
								if((element->value.first == key) && (node != element))
									return false;
							}
							node->value.first = key;
						}

						if(name)
							node->value.second.text = name;

						return (key || name);
					}
					return false;

				}

				trigger::ext_event_type& trigger::ext_event() const
				{
					return impl_->attr.ext_event;
				}

				void trigger::bind_window(widget_reference widget)
				{
					impl_->data.widget_ptr = & widget;
					widget.background(0xFFFFFF);
					widget.caption(STR("Nana Treebox"));
				}

				void trigger::attached(graph_reference graph)
				{
					impl_->data.graph = &graph;
					window wd = impl_->data.widget_ptr->handle();
					using namespace API::dev;
					make_drawer_event<events::dbl_click>(wd);
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_up>(wd);
					make_drawer_event<events::mouse_wheel>(wd);
					make_drawer_event<events::size>(wd);
					make_drawer_event<events::key_down>(wd);
					make_drawer_event<events::key_char>(wd);
				}

				void trigger::detached()
				{
					API::dev::umake_drawer_event(impl_->data.widget_ptr->handle());
				}

				void trigger::refresh(graph_reference)
				{
					impl_->draw(false);
				}

				void trigger::dbl_click(graph_reference, const eventinfo& ei)
				{
					auto & shape = impl_->shape;

					int xpos = impl_->attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(impl_, xpos, ei.mouse.x, ei.mouse.y);
					impl_->attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					if(nl.node() && (nl.what() == component::text || nl.what() == component::icon))
					{
						impl_->node_state.event_node = nl.node();
						impl_->set_expanded(impl_->node_state.event_node, !impl_->node_state.event_node->value.second.expanded);
						impl_->draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_down(graph_reference graph, const eventinfo& ei)
				{
					auto & shape = impl_->shape;

					int xpos = impl_->attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(impl_, xpos, ei.mouse.x, ei.mouse.y);
					impl_->attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					bool has_redraw = false;

					auto & node_state = impl_->node_state;
					node_state.event_node = nullptr;

					if(nl.node())
					{
						node_state.event_node = nl.node();
						if(nl.what() != component::end)
						{
							if(nl.what() ==  component::expender)
							{
								if(impl_->set_expanded(node_state.event_node, !node_state.event_node->value.second.expanded))
									impl_->make_adjust(node_state.event_node, 0);

								has_redraw = true;
							}
							else if(nl.item_body())
							{
								if(node_state.selected != node_state.event_node)
								{
									impl_->set_selected(node_state.event_node);
									has_redraw = true;
								}
							}
						}
						else if(node_state.selected != node_state.event_node)
						{
							impl_->set_selected(node_state.event_node);
							has_redraw = true;
						}
					}

					if(has_redraw)
					{
						impl_->draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_up(graph_reference, const eventinfo& ei)
				{
					auto & shape = impl_->shape;

					int xpos = impl_->attr.tree_cont.indent_size(shape.first) * shape.indent_pixels - shape.offset_x;
					item_locator nl(impl_, xpos, ei.mouse.x, ei.mouse.y);
					impl_->attr.tree_cont.for_each<item_locator&>(shape.first, nl);

					if(nl.node() && (impl_->node_state.selected != nl.node()) && nl.item_body())
					{
						impl_->set_selected(nl.node());
						if(impl_->make_adjust(impl_->node_state.selected, 1))
							impl_->adjust.scroll_timestamp = 1;

						impl_->draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_move(graph_reference graph, const eventinfo& ei)
				{
					if(impl_->track_mouse(ei.mouse.x, ei.mouse.y))
					{
						impl_->draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_wheel(graph_reference, const eventinfo& ei)
				{
					auto & shape = impl_->shape;
					std::size_t prev = shape.prev_first_value;

					shape.scroll.make_step(!ei.wheel.upwards);

					impl_->event_scrollbar(ei);

					if(prev != shape.prev_first_value)
					{
						impl_->track_mouse(ei.wheel.x, ei.wheel.y);

						impl_->draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::resize(graph_reference, const eventinfo&)
				{
					impl_->draw(false);
					API::lazy_refresh();
					impl_->show_scrollbar();
					if(false == impl_->shape.scroll.empty())
					{
						nana::size s = impl_->data.graph->size();
						impl_->shape.scroll.move(s.width - 16, 0, 16, s.height);
					}
				}

				void trigger::key_down(graph_reference, const eventinfo& ei)
				{
					bool redraw = false;
					bool scroll = false; //Adjust the scrollbar

					auto & node_state = impl_->node_state;

					switch(ei.keyboard.key)
					{
					case keyboard::os_arrow_up:
						if(node_state.selected && node_state.selected != impl_->attr.tree_cont.get_root()->child)
						{
							node_type * prev = node_state.selected->owner;
							if(prev->child != node_state.selected)
							{
								prev = prev->child;
								while(prev->next != node_state.selected)
									prev = prev->next;

								while(prev->child && prev->value.second.expanded)
								{
									prev = prev->child;
									while(prev->next)
										prev = prev->next;
								}
							}

							impl_->set_selected(prev);

							if(impl_->make_adjust(prev, 4))
								scroll = true;

							redraw = true;
						}
						break;
					case keyboard::os_arrow_down:
						if(node_state.selected)
						{
							node_type * node = node_state.selected;
							if(node->value.second.expanded)
							{
								node = node->child;
							}
							else if(node->next)
							{
								node = node->next;
							}
							else
							{
								node = node->owner;
								while(node && (nullptr == node->next))
									node = node->owner;

								if(node)
									node = node->next;
							}

							if(node)
							{
								impl_->set_selected(node);
								redraw = true;
								if(impl_->make_adjust(node_state.selected, 4))
									scroll = true;
							}
						}
						break;
					case keyboard::os_arrow_left:
						if(node_state.selected)
						{
							if(node_state.selected->value.second.expanded == false)
							{
								if(node_state.selected->owner != impl_->attr.tree_cont.get_root())
								{
									impl_->set_selected(node_state.selected->owner);
									impl_->make_adjust(node_state.selected, 4);
								}
							}
							else
								impl_->set_expanded(node_state.selected, false);

							redraw = true;
							scroll = true;
						}
						break;
					case keyboard::os_arrow_right:
						if(node_state.selected)
						{
							if(node_state.selected->value.second.expanded == false)
							{
								impl_->set_expanded(node_state.selected, true);
								redraw = true;
								scroll = true;
							}
							else if(node_state.selected->child)
							{
								impl_->set_selected(node_state.selected->child);
								impl_->make_adjust(node_state.selected, 4);
								redraw = true;
								scroll = true;
							}
						}
						break;
					}

					if(redraw)
					{
						impl_->draw(scroll);
						API::lazy_refresh();
					}
				}

				void trigger::key_char(graph_reference, const eventinfo& ei)
				{
					auto node = const_cast<node_type*>(impl_->find_track_node(ei.keyboard.key));

					if(node && (node != impl_->node_state.selected))
					{
						impl_->set_selected(node);
						impl_->make_adjust(node, 4);
						impl_->draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::_m_deal_adjust()
				{
					auto & adjust = impl_->adjust;
					if(adjust.scroll_timestamp && (nana::system::timestamp() - adjust.scroll_timestamp >= 500))
					{
						if(adjust.offset_x_adjust == 0)
						{
							if(false == impl_->make_adjust(adjust.node ? adjust.node : impl_->shape.first, 1))
							{
								adjust.offset_x_adjust = 0;
								adjust.node = nullptr;
								adjust.scroll_timestamp = 0;
								adjust.timer.enable(false);
								return;
							}
						}

						auto & shape = impl_->shape;
						const int delta = 5;
						int old = shape.offset_x;
						if(shape.offset_x < adjust.offset_x_adjust)
						{
							shape.offset_x += delta;
							if(shape.offset_x > adjust.offset_x_adjust)
								shape.offset_x = adjust.offset_x_adjust;
						}
						else if(shape.offset_x > adjust.offset_x_adjust)
						{
							shape.offset_x -= delta;
							if(shape.offset_x < adjust.offset_x_adjust)
								shape.offset_x = adjust.offset_x_adjust;
						}

						impl_->draw(false);
						API::update_window(impl_->data.widget_ptr->handle());

						if(impl_->node_state.tooltip)
						{
							nana::point pos = impl_->node_state.tooltip->pos();
							pos.x -= (shape.offset_x - old);
							impl_->node_state.tooltip->move(pos.x, pos.y);
						}

						if(shape.offset_x == adjust.offset_x_adjust)
						{
							adjust.offset_x_adjust = 0;
							adjust.node = nullptr;
							adjust.scroll_timestamp = 0;
							adjust.timer.enable(false);
						}
					}
				}
			//end class trigger
		}//end namespace treebox
	}//end namespace drawerbase
}//end namespace gui
}//end namespace nana
