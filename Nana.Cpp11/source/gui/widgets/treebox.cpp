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

				item_locator(trigger& drawer, int item_pos, int x, int y)
					:drawer_(drawer), item_pos_(item_pos), item_ypos_(1), pos_(x, y), what_(component::end), node_(nullptr)
				{}

				int operator()(node_type &node, int affect)
				{
					auto & node_desc = drawer_.node_desc_;

					switch(affect)
					{
					case 0: break;
					case 1: item_pos_ += node_desc.indent_size; break;
					default:
						if(affect >= 2)
							item_pos_ -= node_desc.indent_size * (affect - 1);
					}

					if((pos_.y - item_ypos_) < static_cast<int>(drawer_._m_node_height()))
					{
						node_ = &node;

						if((item_pos_ < pos_.x) && (pos_.x < item_pos_ + node_desc.item_offset))
						{
							what_ = (node.child ? component::expender : component::end);
							return 0;
						}

						int comp_pos = item_pos_ + node_desc.item_offset;

						if(node_desc.crook_pixels && (comp_pos <= pos_.x) && (pos_.x < comp_pos + node_desc.crook_pixels))
						{
							what_ = component::crook;
							return 0;
						}

						comp_pos += node_desc.crook_pixels;
						if(node_desc.image_pixels && (comp_pos <= pos_.x) && (pos_.x < comp_pos + node_desc.image_pixels))
							what_ = component::icon;
						else if((item_pos_ + drawer_.node_desc_.item_offset <= pos_.x) && (pos_.x < item_pos_ + drawer_.node_desc_.item_offset + static_cast<int>(drawer_.node_width(&node))))
							what_ = component::text;
						else
							what_ = component::end;

						return 0;
					}

					item_ypos_ += drawer_._m_node_height();

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
					return nana::point(item_pos_ + drawer_.node_desc_.item_offset, item_ypos_);
				}

				nana::size size() const
				{
					return nana::size(static_cast<int>(drawer_.node_width(node_)), drawer_._m_node_height());
				}
			private:
				trigger& drawer_;
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

				item_renderer(trigger& drawer, const nana::point& pos)
					:drawer_(drawer), pos_(pos), node_height_pixels_(drawer._m_node_height())
				{
					bgcolor_ = drawer_.widget_->background();
					fgcolor_ = drawer_.widget_->foreground();
				}

				//affect
				//0 = Sibling, the last is a sibling of node
				//1 = Owner, the last is the owner of node
				//>=2 = Children, the last is a child of a node that before this node.
				int operator()(const node_type& node, int affect)
				{
					iterated_node_ = &node;
					switch(affect)
					{
					case 1:
						pos_.x += drawer_.node_desc_.indent_size;
						break;
					default:
						if(affect >= 2)
							pos_.x -= drawer_.node_desc_.indent_size * (affect - 1);
					}

					item_attr_.expended = node.value.second.expanded;
					item_attr_.text = node.value.second.text;
					item_attr_.checked = false;
					item_attr_.mouse_pointed = (drawer_.node_state_.highlight == iterated_node_);
					item_attr_.selected = (drawer_.node_state_.selected == iterated_node_);

					drawer_.renderer_->refer().render(*drawer_.graph_, bgcolor_, fgcolor_, this); 

					pos_.y += node_height_pixels_;

					if(pos_.y > static_cast<int>(drawer_.graph_->height()))
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
						attr.area.x = pos_.x + drawer_.node_desc_.item_offset;
						attr.area.y = pos_.y;
						attr.area.width = drawer_.node_width(iterated_node_);
						attr.area.height = node_height_pixels_;
						return true;
					case component::crook:
						if(drawer_.node_desc_.crook_pixels)
						{
							attr.area.x = pos_.x + drawer_.node_desc_.item_offset + 2;
							attr.area.y = pos_.y;
							attr.area.width = drawer_.node_desc_.crook_pixels;
							attr.area.height = node_height_pixels_;
							return true;
						}
						return false;
					case component::icon:
						if(drawer_.node_desc_.image_pixels)
						{
							attr.area.x = pos_.x + drawer_.node_desc_.item_offset + drawer_.node_desc_.crook_pixels + 2;
							attr.area.y = pos_.y + 2;
							attr.area.width = drawer_.node_desc_.image_pixels;
							attr.area.height = node_height_pixels_ - 2;
							return true;
						}
						return false;
					case component::text:
						attr.area.x = pos_.x + drawer_.node_desc_.item_offset + drawer_.node_desc_.image_pixels + drawer_.node_desc_.text_offset;
						attr.area.y = pos_.y;
						attr.area.width = drawer_.node_width(iterated_node_) - ( attr.area.x - pos_.x);
						attr.area.height = node_height_pixels_;
						return true;
					default:
						break;
					}
					return false;
				}
			private:
				trigger& drawer_;
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
					:graph_(nullptr), widget_(nullptr)
				{
					renderer_ = nana::pat::cloneable<internal_renderer, renderer_interface>().clone();
					adjust_.timer.enable(false);
					adjust_.timer.make_tick(std::bind(&trigger::_m_deal_adjust, this));
					adjust_.timer.interval(10);
				}

				trigger::~trigger()
				{
					renderer_->self_delete();
				}

				void trigger::renderer(const nana::pat::cloneable_interface<renderer_interface>& rd)
				{
					auto new_renderer = rd.clone();
					if(new_renderer)
					{
						if(renderer_)
							renderer_->self_delete();

						renderer_ = new_renderer;
					}
				}

				const nana::pat::cloneable_interface<renderer_interface> & trigger::renderer() const
				{
					return *renderer_;
				}

				void trigger::auto_draw(bool ad)
				{
					if(attr_.auto_draw != ad)
					{
						attr_.auto_draw = ad;
						if(ad)
							API::update_window(widget_->handle());
					}
				}

				auto trigger::tree() -> tree_cont_type &
				{
					return attr_.tree_cont;
				}

				auto trigger::tree() const -> tree_cont_type const &
				{
					return attr_.tree_cont;
				}

				nana::any & trigger::value(node_type* node) const
				{
					if(attr_.tree_cont.check(node) == false)
						throw std::invalid_argument("Nana.GUI.treebox.value() invalid node");

					return node->value.second.value;
				}

				trigger::node_type* trigger::insert(node_type* node, const nana::string& key, const nana::string& title, const nana::any& v)
				{
					node_type * p = attr_.tree_cont.node(node, key);
					if(p)
					{
						p->value.second.text = title;
						p->value.second.value = v;
					}
					else
						p = attr_.tree_cont.insert(node, key, treebox_node_type(title, v));

					if(p && attr_.auto_draw && _m_draw(true))
						API::update_window(widget_->handle());
					return p;
				}

				trigger::node_type* trigger::insert(const nana::string& path, const nana::string& title, const nana::any& v)
				{
					auto x = attr_.tree_cont.insert(path, treebox_node_type(title, v));
					if(x && attr_.auto_draw && _m_draw(true))
						API::update_window(widget_->handle());
					return x;
				}

				bool trigger::check(node_type* parent, node_type* child) const
				{
					if(false == (parent && child)) return false;

					while(child && (child != parent))
						child = child->owner;

					return (nullptr != child);
				}

				void trigger::remove(node_type* node)
				{
					if(check(node, node_state_.event_node))
						node_state_.event_node = nullptr;

					if(check(node, node_desc_.first))
						node_desc_.first = nullptr;

					if(check(node, node_state_.selected))
						node_state_.selected = nullptr;

					attr_.tree_cont.remove(node);
				}

				trigger::node_type* trigger::selected() const
				{
					return node_state_.selected;
				}

				void trigger::selected(node_type* node)
				{
					if(attr_.tree_cont.check(node) && _m_set_selected(node))
					{
						_m_draw(true);
						API::update_window(*widget_);
					}
				}

				void trigger::set_expand(node_type* node, bool exp)
				{
					if(widget_ && _m_set_expanded(node, exp))
					{
						_m_draw(true);
						API::update_window(widget_->handle());
					}
				}

				void trigger::set_expand(const nana::string& path, bool exp)
				{
					if(_m_set_expanded(attr_.tree_cont.find(path), exp))
					{
						_m_draw(true);
						API::update_window(widget_->handle());
					}
				}

				unsigned trigger::visual_item_size() const
				{
					return attr_.tree_cont.child_size_if(STR(""), pred_allow_child());
				}

				void trigger::image(const nana::string& id, const node_image_tag& img)
				{
					shape_.image_table[id] = img;
				}

				node_image_tag& trigger::image(const nana::string& id) const
				{
					auto i = shape_.image_table.find(id);
					if(i != shape_.image_table.end())
						return i->second;
					throw std::invalid_argument("Nana.GUI.treebox.image() invalid image identifier");
				}

				void trigger::image_erase(const nana::string& id)
				{
					shape_.image_table.erase(id);
				}

				void trigger::node_image(node_type* node, const nana::string& id)
				{
					if(tree().check(node))
					{
						node->value.second.img_idstr = id;
						auto i = shape_.image_table.find(id);
						if((i != shape_.image_table.end()) && _m_draw(true))
								API::update_window(widget_->handle());
					}
				}

				unsigned long trigger::node_width(const node_type *node) const
				{
					return (static_cast<int>(graph_->text_extent_size(node->value.second.text).width) + node_desc_.text_offset * 2 + node_desc_.crook_pixels + node_desc_.image_pixels);
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
					return attr_.ext_event;
				}

				void trigger::bind_window(widget_reference widget)
				{
					widget.background(0xFFFFFF);
					widget_ = &widget;
					widget.caption(STR("Nana Treebox"));
				}

				void trigger::attached(graph_reference graph)
				{
					graph_ = &graph;
					window wd = widget_->handle();
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
					API::dev::umake_drawer_event(widget_->handle());
				}

				void trigger::refresh(graph_reference)
				{
					_m_draw(false);
				}

				void trigger::dbl_click(graph_reference, const eventinfo& ei)
				{
					int xpos = attr_.tree_cont.indent_size(node_desc_.first) * node_desc_.indent_size - node_desc_.offset_x;
					item_locator nl(*this, xpos, ei.mouse.x, ei.mouse.y);
					attr_.tree_cont.for_each<item_locator&>(node_desc_.first, nl);

					if(nl.node() && (nl.what() == component::text || nl.what() == component::icon))
					{
						node_state_.event_node = nl.node();
						_m_set_expanded(node_state_.event_node, !node_state_.event_node->value.second.expanded);
						_m_draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_down(graph_reference graph, const eventinfo& ei)
				{
					int xpos = attr_.tree_cont.indent_size(node_desc_.first) * node_desc_.indent_size - node_desc_.offset_x;
					item_locator nl(*this, xpos, ei.mouse.x, ei.mouse.y);
					attr_.tree_cont.for_each<item_locator&>(node_desc_.first, nl);

					bool has_redraw = false;

					node_state_.event_node = nullptr;

					if(nl.node())
					{
						node_state_.event_node = nl.node();
						if(nl.what() != component::end)
						{
							if(nl.what() ==  component::expender)
							{
								if(_m_set_expanded(node_state_.event_node, !node_state_.event_node->value.second.expanded))
									_m_adjust(node_state_.event_node, 0);

								has_redraw = true;
							}
							else if(nl.item_body())
							{
								if(node_state_.selected != node_state_.event_node)
								{
									_m_set_selected(node_state_.event_node);
									has_redraw = true;
								}
							}
						}
						else if(node_state_.selected != node_state_.event_node)
						{
							_m_set_selected(node_state_.event_node);
							has_redraw = true;
						}
					}

					if(has_redraw)
					{
						_m_draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_up(graph_reference, const eventinfo& ei)
				{
					int xpos = attr_.tree_cont.indent_size(node_desc_.first) * node_desc_.indent_size - node_desc_.offset_x;
					item_locator nl(*this, xpos, ei.mouse.x, ei.mouse.y);
					attr_.tree_cont.for_each<item_locator&>(node_desc_.first, nl);

					if(nl.node() && (node_state_.selected != nl.node()) && nl.item_body())
					{
						_m_set_selected(nl.node());
						if(_m_adjust(node_state_.selected, 1))
							adjust_.scroll_timestamp = 1;

						_m_draw(true);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_move(graph_reference graph, const eventinfo& ei)
				{
					if(_m_track_mouse(ei.mouse.x, ei.mouse.y))
					{
						_m_draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::mouse_wheel(graph_reference, const eventinfo& ei)
				{
					unsigned prev = shape_.prev_first_value;

					shape_.scrollbar.make_step(!ei.wheel.upwards);

					_m_event_scrollbar(ei);

					if(prev != shape_.prev_first_value)
					{
						_m_track_mouse(ei.wheel.x, ei.wheel.y);

						_m_draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::resize(graph_reference, const eventinfo&)
				{
					_m_draw(false);
					API::lazy_refresh();
					_m_show_scrollbar();
					if(shape_.scrollbar.empty() == false)
					{
						shape_.scrollbar.move(graph_->width() - 16, 0);
						shape_.scrollbar.size(16, graph_->height());
					}
				}

				void trigger::key_down(graph_reference, const eventinfo& ei)
				{
					bool redraw = false;
					bool scroll = false; //Adjust the scrollbar

					switch(ei.keyboard.key)
					{
					case keyboard::os_arrow_up:
						if(node_state_.selected && node_state_.selected != attr_.tree_cont.get_root()->child)
						{
							node_type * prev = node_state_.selected->owner;
							if(prev->child != node_state_.selected)
							{
								prev = prev->child;
								while(prev->next != node_state_.selected)
									prev = prev->next;

								while(prev->child && prev->value.second.expanded)
								{
									prev = prev->child;
									while(prev->next)
										prev = prev->next;
								}
							}

							_m_set_selected(prev);

							if(_m_adjust(prev, 4))
								scroll = true;

							redraw = true;
						}
						break;
					case keyboard::os_arrow_down:
						if(node_state_.selected)
						{
							node_type * node = node_state_.selected;
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
								_m_set_selected(node);
								redraw = true;
								if(_m_adjust(node_state_.selected, 4))
									scroll = true;
							}
						}
						break;
					case keyboard::os_arrow_left:
						if(node_state_.selected)
						{
							if(node_state_.selected->value.second.expanded == false)
							{
								if(node_state_.selected->owner != attr_.tree_cont.get_root())
								{
									_m_set_selected(node_state_.selected->owner);
									_m_adjust(node_state_.selected, 4);
								}
							}
							else
								_m_set_expanded(node_state_.selected, false);

							redraw = true;
							scroll = true;
						}
						break;
					case keyboard::os_arrow_right:
						if(node_state_.selected)
						{
							if(node_state_.selected->value.second.expanded == false)
							{
								_m_set_expanded(node_state_.selected, true);
								redraw = true;
								scroll = true;
							}
							else if(node_state_.selected->child)
							{
								_m_set_selected(node_state_.selected->child);
								_m_adjust(node_state_.selected, 4);
								redraw = true;
								scroll = true;
							}
						}
						break;
					}

					if(redraw)
					{
						_m_draw(scroll);
						API::lazy_refresh();
					}
				}

				void trigger::key_char(graph_reference, const eventinfo& ei)
				{
					auto node = const_cast<node_type*>(_m_find_track_node(ei.keyboard.key));

					if(node && (node != node_state_.selected))
					{
						_m_set_selected(node);
						_m_adjust(node, 4);
						_m_draw(false);
						API::lazy_refresh();
					}
				}

				void trigger::_m_find_first(unsigned long offset)
				{
					node_desc_.first = attr_.tree_cont.advance_if(nullptr, offset, pred_allow_child());
				}

				unsigned trigger::_m_node_height() const
				{
					return graph_->text_extent_size(STR("jh{")).height + 8;
				}

				unsigned trigger::_m_max_allow() const
				{
					return graph_->height() / _m_node_height();
				}

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

				const trigger::node_type* find_track_child_node(const trigger::node_type* node, const trigger::node_type * end, const nana::char_t* pattern, std::size_t len, bool &finish)
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

				const trigger::node_type* trigger::_m_find_track_node(nana::char_t key)
				{
					nana::string pattern;

					if('a' <= key && key <= 'z') key -= 'a' - 'A';

					unsigned long now = nana::system::timestamp();

					if(now - track_node_.key_time > 1000)
						track_node_.key_buf.clear();

					if(track_node_.key_buf.length())
					{
						if(track_node_.key_buf[track_node_.key_buf.length() - 1] != key)
							pattern = track_node_.key_buf;
					}

					track_node_.key_time = now;
					pattern += key;
					track_node_.key_buf += key;

					const node_type *begin = node_state_.selected ? node_state_.selected : attr_.tree_cont.get_root()->child;

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
									node = attr_.tree_cont.get_root()->child;
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
									node = attr_.tree_cont.get_root()->child;
									end = begin;
								}
							}
							else
								node = node->next;
						}
					}
					return nullptr;
				}

				nana::paint::image* trigger::_m_image(const node_type* node)
				{
					const nana::string& idstr = node->value.second.img_idstr;
					if(idstr.size())
					{
						auto i = shape_.image_table.find(idstr);
						if(i == shape_.image_table.end())
							return nullptr;

						unsigned long state = 0xFFFFFFFF;
						if(node_state_.highlight == node && (node_state_.comp_highlighted == component::text || node_state_.comp_highlighted == component::crook || node_state_.comp_highlighted == component::icon))
							state = (node_state_.highlight != node_state_.selected ? 0: 1);
						else if(node_state_.selected == node)
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

				bool trigger::_m_track_mouse(int x, int y)
				{
					int xpos = attr_.tree_cont.indent_size(node_desc_.first) * node_desc_.indent_size - node_desc_.offset_x;
					item_locator nl(*this, xpos, x, y);
					attr_.tree_cont.for_each<item_locator&>(node_desc_.first, nl);

					bool redraw = false;
					node_state_.event_node = nl.node();

					if(nl.node() && (nl.what() != component::end))
					{
						if((nl.what() != node_state_.comp_highlighted || nl.node() != node_state_.highlight))
						{
							node_state_.comp_highlighted = nl.what();
							node_state_.highlight = nl.node();
							redraw = (node_state_.comp_highlighted != component::end);
						}
					}
					else if(node_state_.highlight)
					{
						redraw = true;
						node_state_.comp_highlighted = component::end;
						node_state_.highlight = nullptr;
						_m_close_tooltip_window();
					}

					if(redraw)
					{
						if(node_state_.comp_highlighted == component::text)
						{
							_m_adjust(node_state_.highlight, 2);
							adjust_.scroll_timestamp = 1;

							_m_tooltip_window(node_state_.highlight, nl.pos(), nl.size());
						}
					}

					return redraw;
				}

				void trigger::_m_tooltip_window(node_type* node, const nana::point& pos, const nana::size& size)
				{
					_m_close_tooltip_window();

					if((nullptr == node_state_.tooltip) && (pos.x + size.width > _m_visible_width()))
					{
						node_state_.tooltip = new tooltip_window(widget_->handle(), pos, size);
						node_state_.tooltip->show_text(node->value.second.text, node_desc_.text_offset + node_desc_.image_pixels + node_desc_.crook_pixels, (node == node_state_.selected), this->_m_image(node));

						node_state_.tooltip->make_event<events::mouse_leave>(*this, &trigger::_m_close_tooltip_window);
						node_state_.tooltip->make_event<events::mouse_move>(*this, &trigger::_m_mouse_move_tooltip_window);

						auto click_fn = nana::make_fun(*this, &trigger::_m_click_tooltip_window);
						node_state_.tooltip->make_event<events::mouse_down>(click_fn);
						node_state_.tooltip->make_event<events::mouse_up>(click_fn);
						node_state_.tooltip->make_event<events::dbl_click>(click_fn);
					}
				}

				void trigger::_m_close_tooltip_window()
				{
					if(node_state_.tooltip)
					{
						tooltip_window * x = node_state_.tooltip;
						node_state_.tooltip = nullptr;
						delete x;
					}
				}

				void trigger::_m_mouse_move_tooltip_window()
				{
					nana::point pos = API::cursor_position();
					API::calc_window_point(widget_->handle(), pos);

					if(pos.x >= static_cast<int>(_m_visible_width()))
						_m_close_tooltip_window();
				}

				void trigger::_m_click_tooltip_window(const eventinfo& ei)
				{
					bool redraw = false;
					switch(ei.identifier)
					{
					case events::mouse_down::identifier:
						if(_m_adjust(node_state_.highlight, 1))
							adjust_.scroll_timestamp = 1;
						break;
					case events::mouse_up::identifier:
						if(node_state_.selected != node_state_.highlight)
						{
							_m_set_selected(node_state_.highlight);
							redraw = true;
						}
						break;
					default:
						_m_set_expanded(node_state_.selected, !node_state_.selected->value.second.expanded);
						redraw = true;
					}

					if(redraw)
					{
						_m_draw(false);
						API::update_window(widget_->handle());
					}
				}

				bool trigger::_m_draw(bool scrollbar_react)
				{
					if(graph_ && (false == dwflags_.pause))
					{
						if(scrollbar_react)
							_m_show_scrollbar();

						//draw background
						graph_->rectangle(widget_->background(), true);

						_m_draw_tree();
						return true;
					}
					return false;
				}

				void trigger::_m_draw_tree()
				{
					attr_.tree_cont.for_each(node_desc_.first, item_renderer(*this, nana::point(static_cast<int>(attr_.tree_cont.indent_size(node_desc_.first) * node_desc_.indent_size) - node_desc_.offset_x, 1)));
				}

				unsigned trigger::_m_visible_width() const
				{
					if(nullptr == graph_)
						return 0;

					return graph_->width() - (shape_.scrollbar.empty() ? 0 : shape_.scrollbar.size().width);
				}

				void trigger::_m_show_scrollbar()
				{
					if(nullptr == graph_) return;

					unsigned max_allow = _m_max_allow();
					unsigned visual_items = visual_item_size();

					if(visual_items <= max_allow)
					{
						if(false == shape_.scrollbar.empty())
						{
							shape_.scrollbar.close();
							node_desc_.first = nullptr;
						}
					}
					else
					{
						if(shape_.scrollbar.empty())
						{
							shape_.prev_first_value = 0;
							shape_.scrollbar.create(*widget_, nana::rectangle(graph_->width() - 16, 0, 16, graph_->height()));
							auto scroll_fn = nana::make_fun(*this, &trigger::_m_event_scrollbar);
							shape_.scrollbar.make_event<events::mouse_down>(scroll_fn);
							shape_.scrollbar.make_event<events::mouse_move>(scroll_fn);
							shape_.scrollbar.make_event<events::mouse_wheel>(scroll_fn);
						}

						shape_.scrollbar.amount(visual_items);
						shape_.scrollbar.range(max_allow);
					}

					shape_.scrollbar.value(attr_.tree_cont.distance_if(node_desc_.first, pred_allow_child()));
				}

				void trigger::_m_event_scrollbar(const eventinfo& ei)
				{
					if(ei.identifier == events::mouse_wheel::identifier || ei.mouse.left_button)
					{
						if(shape_.prev_first_value != shape_.scrollbar.value())
						{
							shape_.prev_first_value = static_cast<unsigned long>(shape_.scrollbar.value());
							adjust_.scroll_timestamp = nana::system::timestamp();
							adjust_.timer.enable(true);

							node_desc_.first = attr_.tree_cont.advance_if(0, shape_.prev_first_value, pred_allow_child());

							if(ei.window == shape_.scrollbar.handle())
							{
								_m_draw(false);
								API::update_window(widget_->handle());
							}
						}
					}
				}

				bool trigger::_m_adjust(node_type * node, int reason)
				{
					if(nullptr == node) return false;

					switch(reason)
					{
					case 0:
						//adjust if the node expanded and the number of its children are over the max number allowed
						if(node_desc_.first != node)
						{
							unsigned child_size = attr_.tree_cont.child_size_if(*node, pred_allow_child());
							const unsigned max_allow = _m_max_allow();

							if(child_size < max_allow)
							{
								unsigned off1 = attr_.tree_cont.distance_if(node_desc_.first, pred_allow_child());
								unsigned off2 = attr_.tree_cont.distance_if(node, pred_allow_child());
								const unsigned size = off2 - off1 + child_size + 1;
								if(size > max_allow)
									node_desc_.first = attr_.tree_cont.advance_if(node_desc_.first, size - max_allow, pred_allow_child());
							}
							else
								node_desc_.first = node;
						}
						break;
					case 1:
					case 2:
					case 3:
						//param is the begin pos of an item in absolute.
						{
							int beg = static_cast<int>(attr_.tree_cont.indent_size(node) * node_desc_.indent_size) - node_desc_.offset_x;
							int end = beg + static_cast<int>(this->node_width(node)) + node_desc_.item_offset;

							bool adjust = false;
							if(reason == 1)
								adjust = (beg < 0 || (beg > 0 && end > static_cast<int>(_m_visible_width())));
							else if(reason == 2)
								adjust = (beg < 0);
							else if(reason == 3)
								return (beg > 0 && end > static_cast<int>(_m_visible_width()));

							if(adjust)
							{
								adjust_.offset_x_adjust = node_desc_.offset_x + beg;
								adjust_.node = node;
								adjust_.timer.enable(true);
								return true;
							}
						}
						break;
					case 4:
						if(node_desc_.first != node)
						{
							unsigned off_first = attr_.tree_cont.distance_if(node_desc_.first, pred_allow_child());
							unsigned off_node = attr_.tree_cont.distance_if(node, pred_allow_child());
							if(off_node < off_first)
							{
								node_desc_.first = node;
								return true;
							}
							else if(off_node - off_first > _m_max_allow())
							{
								node_desc_.first = attr_.tree_cont.advance_if(0, off_node - _m_max_allow() + 1, pred_allow_child());
								return true;
							}
						}
						break;
					}
					return false;
				}

				bool trigger::_m_set_selected(trigger::node_type * node)
				{
					if(node_state_.selected != node)
					{
						dwflags_.pause = true;
						if(node_state_.selected)
							attr_.ext_event.selected(widget_->handle(), reinterpret_cast<ext_event_type::node_type>(node_state_.selected), false);

						node_state_.selected = node;
						if(node)
							attr_.ext_event.selected(widget_->handle(), reinterpret_cast<ext_event_type::node_type>(node), true);
						dwflags_.pause = false;
						return true;
					}
					return false;
				}

				bool trigger::_m_set_expanded(node_type* node, bool value)
				{
					if(node && node->value.second.expanded != value)
					{
						if(value == false)
						{
							//if contracting a parent of the selected node, select the contracted node.
							if(check(node, node_state_.selected))
								_m_set_selected(node);
						}

						node->value.second.expanded = value;
						if(node->child)
						{
							dwflags_.pause = true;
							attr_.ext_event.expand(widget_->handle(), reinterpret_cast<ext_event_type::node_type>(node), value);
							dwflags_.pause = false;
						}
						return true;
					}
					return false;
				}

				void trigger::_m_deal_adjust()
				{
					if(adjust_.scroll_timestamp && (nana::system::timestamp() - adjust_.scroll_timestamp >= 500))
					{
						if(adjust_.offset_x_adjust == 0)
						{
							if(false == _m_adjust(adjust_.node ? adjust_.node : node_desc_.first, 1))
							{
								adjust_.offset_x_adjust = 0;
								adjust_.node = nullptr;
								adjust_.scroll_timestamp = 0;
								adjust_.timer.enable(false);
								return;
							}
						}

						const int delta = 5;
						int old = node_desc_.offset_x;
						if(node_desc_.offset_x < adjust_.offset_x_adjust)
						{
							node_desc_.offset_x += delta;
							if(node_desc_.offset_x > adjust_.offset_x_adjust)
								node_desc_.offset_x = adjust_.offset_x_adjust;
						}
						else if(node_desc_.offset_x > adjust_.offset_x_adjust)
						{
							node_desc_.offset_x -= delta;
							if(node_desc_.offset_x < adjust_.offset_x_adjust)
								node_desc_.offset_x = adjust_.offset_x_adjust;
						}

						_m_draw(false);
						API::update_window(widget_->handle());

						if(node_state_.tooltip)
						{
							nana::point pos = node_state_.tooltip->pos();
							pos.x -= (node_desc_.offset_x - old);
							node_state_.tooltip->move(pos.x, pos.y);
						}

						if(node_desc_.offset_x == adjust_.offset_x_adjust)
						{
							adjust_.offset_x_adjust = 0;
							adjust_.node = nullptr;
							adjust_.scroll_timestamp = 0;
							adjust_.timer.enable(false);
						}
					}
				}

				//struct drawing_flags
					trigger::drawing_flags::drawing_flags()
						:pause(false)
					{}
				//end struct drawing_flags;

				//struct shape_data_type
					trigger::shape_data_type::shape_data_type()
						:prev_first_value(0)
					{}
				//end struct shape_data_type

				//struct attribute_type
					trigger::attribute_type::attribute_type()
						: auto_draw(true), visual_item_size(0), button_width(16)
					{}
				//end struct attribute_type

				//struct node_desc_type
					trigger::node_desc_type::node_desc_type()
						:first(nullptr), indent_size(10),
						 offset_x(0),
						 item_offset(16), text_offset(3),
						 crook_pixels(0),
						 image_pixels(0)
					{}
				//end struct node_desc_type

				//struct node_state
					trigger::node_state::node_state()
						:tooltip(nullptr), highlight(nullptr), comp_highlighted(component::end), selected(nullptr), event_node(nullptr)
					{}
				//end struct node_state

				//struct track_node_tag
					trigger::track_node_tag::track_node_tag()
						:key_time(0)
					{}
				//end struct track_node_tag

				//struct adjust_desc_type
					trigger::adjust_desc_type::adjust_desc_type()
						: offset_x_adjust(0), node(nullptr), scroll_timestamp(0)
					{}
				//end struct adjust_desc_type
			//end class trigger
		}//end namespace treebox
	}//end namespace drawerbase
}//end namespace gui
}//end namespace nana
