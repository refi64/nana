
#include <nana/gui/widgets/menu.hpp>
#include <nana/system/platform.hpp>
#include <nana/paint/gadget.hpp>
#include <nana/gui/wvl.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace menu
		{
			//struct menu_item_type
				//class item_proxy
				//@brief: this class is used as parameter of menu event function.
					menu_item_type::item_proxy::item_proxy(unsigned long index, menu_item_type &item)
						:index_(index), item_(item)
					{}

					void menu_item_type::item_proxy::enabled(bool v)
					{
						item_.flags.enabled = v;
					}

					bool menu_item_type::item_proxy::enabled() const
					{
						return item_.flags.enabled;
					}

					unsigned long menu_item_type::item_proxy::index() const
					{
						return index_;
					}
				//end class item_proxy

				//Default constructor initializes the item as a splitter
				menu_item_type::menu_item_type()
					:sub_menu(0), style(gui::menu::check_none), hotkey(0)
				{
					flags.enabled = true;
					flags.splitter = true;
					flags.checked = false;
				}

				menu_item_type::menu_item_type(const nana::string& text, const functor_type& f)
					:sub_menu(0), text(text), functor(f), style(gui::menu::check_none), hotkey(0)
				{
					flags.enabled = true;
					flags.splitter = false;
					flags.checked = false;
				}
			//end class menu_item_type

			//struct menu_type
				menu_type::menu_type()
					: max_pixels(API::screen_size().width * 2 / 3)
				{}
			//end struct menu_type

			//class menu_builder
				menu_builder::~menu_builder()
				{
					this->destroy();
				}

				void menu_builder::append(const nana::string& text, const menu_builder::functor_type& f)
				{
					root_.items.push_back(menu_item_type(text, f));
				}

				void menu_builder::append_splitter()
				{
					root_.items.push_back(menu_item_type());
				}

				void menu_builder::clear()
				{
					root_.items.clear();
				}

				void menu_builder::check_style(unsigned index, int style)
				{
					if(gui::menu::check_none <= style && style <= gui::menu::check_highlight)
					{
						if(root_.items.size() > index)
							root_.items[index].style = style;
					}
				}

				void menu_builder::checked(unsigned index, bool check)
				{
					if(root_.items.size() > index)
					{
						item_type & m = root_.items[index];
						if(m.style == gui::menu::check_option)
						{
							if(check)
							{
								if(index)
								{
									unsigned i = index;
									do
									{
										--i;
										if(root_.items[i].flags.splitter) break;

										if(root_.items[i].style == gui::menu::check_option)
											root_.items[i].flags.checked = false;
									}while(i);
								}

								for(unsigned i = index + 1; i < root_.items.size(); ++i)
								{
									if(root_.items[i].flags.splitter) break;

									if(root_.items[i].style == gui::menu::check_option)
										root_.items[i].flags.checked = false;
								}
							}
						}
						m.flags.checked = check;
					}
				}

				bool menu_builder::checked(unsigned index) const
				{
					if(root_.items.size() > index)
						return root_.items[index].flags.checked;
					return false;
				}

				menu_type& menu_builder::get_root()
				{
					return root_;
				}

				void menu_builder::insert(unsigned long pos, const nana::string& text, const menu_builder::functor_type& f)
				{
					if(pos < root_.items.size())
						root_.items.insert(root_.items.begin() + pos, menu_item_type(text, f));
					else
						append(text, f);
				}

				void menu_builder::answerer(size_t index, const menu_builder::functor_type& fn)
				{
					if(index < root_.items.size())
						root_.items[index].functor = fn;
				}

				void menu_builder::enabled(unsigned long index, bool enable)
				{
					if(root_.items.size() > index)
						root_.items[index].flags.enabled = enable;
				}

				bool menu_builder::enabled(unsigned long index) const
				{
					if(root_.items.size() > index)
						return root_.items[index].flags.enabled;
					return false;
				}

				void menu_builder::erase(unsigned long index)
				{
					if(index < root_.items.size())
						root_.items.erase(root_.items.begin() + index);
				}

				void menu_builder::image(size_t index, const paint::image& img)
				{
					if(index < root_.items.size())
						root_.items[index].image = img;
				}

				bool menu_builder::set_sub_menu(unsigned long pos, menu_type &sub)
				{
					if(root_.items.size() > pos)
					{
						menu_item_type & item = root_.items[pos];

						if(item.sub_menu == 0)
						{
							item.sub_menu = &sub;
							sub.owner.push_back(&root_);
							return true;
						}
					}
					return false;
				}

				void menu_builder::remove_sub_menu(unsigned long pos)
				{
					if(root_.items.size() <= pos) return;

					menu_item_type & item = root_.items[pos];
					if(item.sub_menu)
					{
						std::vector<menu_type*>::iterator it = std::find(item.sub_menu->owner.begin(), item.sub_menu->owner.end(), &root_);
						if(it != item.sub_menu->owner.end())
							item.sub_menu->owner.erase(it);

						item.sub_menu = 0;
					}
				}

				void menu_builder::destroy()
				{
					for(std::vector<menu_type*>::iterator i = root_.owner.begin(); i != root_.owner.end(); ++i)
					{
						for(std::vector<menu_item_type>::iterator m = (*i)->items.begin(); m != (*i)->items.end(); ++m)
						{
							if(m->sub_menu == &root_)
								m->sub_menu = 0;
						}
					}

					for(std::vector<menu_item_type>::iterator i = root_.items.begin(); i != root_.items.end(); ++i)
					{
						if(i->sub_menu)
						{
							for(std::vector<menu_type*>::iterator m = i->sub_menu->owner.begin(); m != i->sub_menu->owner.end();)
							{
								if((*m) == &root_)
									m = i->sub_menu->owner.erase(m);
								else
									++m;
							}
						}
					}

				}

				unsigned menu_builder::size() const
				{
					return static_cast<unsigned>(root_.items.size());
				}

				menu_builder::iterator menu_builder::begin()
				{
					return root_.items.begin();
				}

				menu_builder::const_iterator menu_builder::begin() const
				{
					return root_.items.begin();
				}

				menu_builder::iterator menu_builder::end()
				{
					return root_.items.end();
				}

				menu_builder::const_iterator menu_builder::end() const
				{
					return root_.items.end();
				}

				void menu_builder::max_pixels(unsigned px)
				{
					if(px < 100)
						root_.max_pixels = 100;
					else
						root_.max_pixels = px;
				}

				unsigned menu_builder::max_pixels() const
				{
					return root_.max_pixels;
				}
			//end class menu_builder


			//class item_renderer

				item_renderer::item_renderer(graph_reference graph)
					: graph_(graph)
				{}

				item_renderer::~item_renderer()
				{}

				item_renderer::graph_reference item_renderer::graph() const
				{
					return graph_;
				}

				void item_renderer::background(const nana::point& pos, const nana::size& size, const item_renderer::state_type& state)
				{
					if(state.state == STATE_ACTIVE)
					{
						graph_.rectangle(pos.x, pos.y, size.width, size.height, 0xA8D8EB, false);
						nana::point points[4] = {
							nana::point(pos.x, pos.y),
							nana::point(pos.x + size.width - 1, pos.y),
							nana::point(pos.x, pos.y + size.height - 1),
							nana::point(pos.x + size.width - 1, pos.y + size.height - 1)
						};
						for(int i = 0; i < 4; ++i)
							graph_.set_pixel(points[i].x, points[i].y, 0xC0DDFC);

						if(state.enabled)
							graph_.shadow_rectangle(pos.x + 1, pos.y + 1, size.width - 2, size.height - 2, 0xE8F0F4, 0xDBECF4, true);
					}

					if(state.checked && (state.check_style != gui::menu::check_none))
					{
						graph_.rectangle(pos.x, pos.y, size.height, size.height, 0xCDD3E6, false);
						graph_.rectangle(pos.x + 1, pos.y + 1, size.height - 2, size.height - 2, 0xE6EFF4, true);

						nana::paint::gadget::checker(graph_, pos.x, pos.y, size.height, (state.check_style == gui::menu::check_option ? nana::paint::gadget::checkers::radio : nana::paint::gadget::checkers::clasp), 0x0);
					}
				}

				void item_renderer::image(int x, int y, const paint::image& img)
				{
					img.paste(graph_, x, y);
				}

				void item_renderer::caption(int x, int y, const nana::string &text, const item_renderer::state_type& state, unsigned strpixels)
				{
					nana::size ts;
					graph_.string(x, y, (state.enabled ? 0x0 : nana::gui::color::gray_border), text, strpixels, true, ts);
				}
			//end class item_renderer

			//class menu_drawer
				menu_drawer::menu_drawer()
					:widget_(0), graph_(0), menu_(0)
				{}

				void menu_drawer::bind_window(menu_drawer::widget_reference widget)
				{
					widget_ = &widget;
				}

				void menu_drawer::attached(menu_drawer::graph_reference graph)
				{
					graph_ = &graph;
					window wd = widget_->handle();
					using namespace API::dev;
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_leave>(wd);
				}

				void menu_drawer::detached()
				{
					API::dev::umake_drawer_event(widget_->handle());
				}

				void menu_drawer::mouse_move(menu_drawer::graph_reference, const nana::gui::eventinfo& ei)
				{
					state_.nullify_mouse = false;
					if(track_mouse(ei.mouse.x, ei.mouse.y))
					{
						draw();
						nana::gui::API::lazy_refresh();
					}
				}

				void menu_drawer::mouse_leave(menu_drawer::graph_reference, const nana::gui::eventinfo& ei)
				{
					state_.nullify_mouse = false;
					if(track_mouse(ei.mouse.x, ei.mouse.y))
					{
						draw();
						nana::gui::API::lazy_refresh();
					}
				}

				void menu_drawer::mouse_down(menu_drawer::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					state_.nullify_mouse = false;
				}

				void menu_drawer::refresh(menu_drawer::graph_reference)
				{
					draw();
				}

				unsigned long menu_drawer::active() const
				{
					return state_.active;
				}

				bool menu_drawer::goto_next(bool forword)
				{
					state_.nullify_mouse = true;
					if(menu_->items.size())
					{
						unsigned long index = state_.active;

						bool end = false;
						while(true)
						{
							if(forword)
							{
								if(index == menu_->items.size() - 1)
								{
									if(end == false)
									{
										end = true;
										index = 0;
									}
									else
									{
										index = npos;
										break;
									}
								}
								else
									++index;
							}
							else
							{
								if(index == 0 || index == npos)
								{
									if(end == false)
									{
										end = true;
										index = static_cast<unsigned long>(menu_->items.size() - 1);
									}
									else
										break;
								}
								else
									--index;
							}
							if(menu_->items.at(index).flags.splitter == false)
								break;
						}

						if(index != npos && index != state_.active)
						{
							state_.active = index;
							state_.sub_window = false;

							draw();
							return true;
						}
					}
					return false;
				}

				bool menu_drawer::track_mouse(int x, int y)
				{
					if(state_.nullify_mouse == false)
					{
						unsigned long index = _m_get_index_by_pos(x, y);
						if(index != state_.active)
						{
							if(index == npos && menu_->items.at(state_.active).sub_menu && state_.sub_window)
								return false;

							state_.active = (index != npos && menu_->items.at(index).flags.splitter) ? npos : index;

							state_.active_timestamp = nana::system::timestamp();
							return true;
						}
					}
					return false;
				}

				void menu_drawer::set_menu_drawer(menu_type & menu)
				{
					menu_ = & menu;
				}

				menu_type* menu_drawer::get_menu() const
				{
					return menu_;
				}

				void menu_drawer::set_sub_window(bool subw)
				{
					state_.sub_window = subw;
				}

				menu_type* menu_drawer::retrive_sub_menu(nana::point& pos, unsigned long interval) const
				{
					if(state_.active != npos && (nana::system::timestamp() - state_.active_timestamp >= interval))
					{
						pos.x = graph_->width() - 2;
						pos.y = 2;

						unsigned long index = 0;
						for(menu_builder::const_iterator it = menu_->items.begin(); it != menu_->items.end(); ++it, ++index)
						{
							if(false == it->flags.splitter)
							{
								if(index == state_.active)
									break;

								pos.y += _m_item_height() + 1;
							}
							else
								pos.y += 2;
						}
						return (menu_->items.at(state_.active).sub_menu);
					}
					return 0;
				}

				//send_shortkey has 3 states, 0 = UNKNOWN KEY, 1 = ITEM, 2 = GOTO SUBMENU
				int menu_drawer::send_shortkey(nana::char_t key)
				{
					unsigned long index = 0;
					for(menu_builder::iterator it = menu_->items.begin(); it != menu_->items.end(); ++it, ++index)
					{
						if(it->hotkey == key)
						{
							if(it->flags.splitter == false)
							{
								if(it->sub_menu)
								{
									state_.active = static_cast<unsigned long>(std::distance(it, menu_->items.begin()));
									state_.active_timestamp = nana::system::timestamp();

									this->draw();

									nana::gui::API::update_window(widget_->handle());

									return 2;
								}
								else if(it->flags.enabled)
								{
									item_proxy ip(index, *it);
									it->functor.operator()(ip);
									return 1;
								}
							}
							break;
						}
					}

					return 0;
				}

				void menu_drawer::draw() const
				{
					if(menu_ == 0) return;

					_m_adjust_window_size();
					//Draw background

					graph_->rectangle(0, 0, graph_->width(), graph_->height(), nana::gui::color::gray_border, false);
					graph_->rectangle(1, 1, 28, graph_->height() - 2, 0xF6F6F6, true);
					graph_->rectangle(29, 1, graph_->width() - 30, graph_->height() - 2, 0xFFFFFF, true);

					item_renderer ird(*graph_);

					nana::point item_pos(2, 2);
					nana::size item_size(graph_->width() - 4, _m_item_height());

					unsigned strpixels = item_size.width - 60;

					unsigned item_height = _m_item_height();
					int text_top_off = (_m_item_height() - graph_->text_extent_size(STR("jh({[")).height) / 2;

					unsigned long index = 0;
					for(menu_builder::const_iterator it = menu_->items.begin(); it != menu_->items.end(); ++it, ++index)
					{
						if(false == it->flags.splitter)
						{
							item_renderer::state_type state = _m_make_renderer_state(index == state_.active, *it);
							//Draw background
							ird.background(item_pos, item_size, state);

							//Draw text, the text is transformed from orignal for hotkey character
							nana::string::value_type hotkey;
							nana::string::size_type hotkey_pos;
							nana::string text = nana::gui::API::transform_shortkey_text(it->text, hotkey, &hotkey_pos);

							if(it->image.empty() == false)
								ird.image(item_pos.x + 5, item_pos.y + (item_height - it->image.size().height) / 2, it->image);

							ird.caption(item_pos.x + 40, item_pos.y + text_top_off, text, state, strpixels);

							if(hotkey)
							{
								it->hotkey = hotkey;
								if(it->flags.enabled)
								{
									unsigned off_w = (hotkey_pos ? graph_->text_extent_size(text, static_cast<unsigned>(hotkey_pos)).width : 0);
									nana::size hotkey_size = graph_->text_extent_size(text.c_str() + hotkey_pos, 1);
									int x = item_pos.x + 40 + off_w;
									int y = item_pos.y + text_top_off + hotkey_size.height;
									graph_->line(x, y, x + hotkey_size.width - 1, y, 0x0);
								}
							}

							if(it->sub_menu)
								nana::paint::gadget::arrow_16_pixels(*graph_, graph_->width() - 20, item_pos.y + 4, 0x0, 0, nana::paint::gadget::directions::to_east);

							item_pos.y += item_size.height + 1;
						}
						else
						{
							graph_->line(item_pos.x + 40, item_pos.y, graph_->width() - 1, item_pos.y, nana::gui::color::gray_border);
							item_pos.y += 2;
						}
					}
				}

				item_renderer::state_type menu_drawer::_m_make_renderer_state(bool active, const menu_item_type & item)
				{
					item_renderer::state_type state;
					state.state = (active ? item_renderer::STATE_ACTIVE : item_renderer::STATE_NORMAL);
					state.enabled = item.flags.enabled;
					state.checked = item.flags.checked;
					state.check_style = item.style;
					return state;
				}

				unsigned long menu_drawer::_m_get_index_by_pos(int x, int y) const
				{
					if(	(x < static_cast<int>(detail_.border.x)) ||
						(x > static_cast<int>(graph_->width() - detail_.border.x)) ||
						(y < static_cast<int>(detail_.border.y)) ||
						(y > static_cast<int>(graph_->height() - detail_.border.y)))
						return npos;

					int pos = detail_.border.y;
					unsigned long index = 0;
					for(menu_type::const_iterator it = menu_->items.begin(); it != menu_->items.end(); ++it)
					{
						unsigned h = (it->flags.splitter ? 1 : _m_item_height());
						if(pos <= y && y < static_cast<int>(pos + h))
							return index;

						else if(y < pos) return npos;

						++index;
						pos += (h + 1);
					}
					return npos;
				}

				unsigned menu_drawer::_m_item_height() const
				{
					return 24;
				}

				nana::size menu_drawer::_m_client_size() const
				{
					nana::size size;

					if(menu_->items.size())
					{
						for(menu_type::const_iterator it = menu_->items.begin(); it != menu_->items.end(); ++it)
						{
							if(it->flags.splitter)
								++size.height;
							else
							{
								nana::size item_size = graph_->text_extent_size(it->text);
								if(size.width < item_size.width)
									size.width = item_size.width;
							}
						}

						size.width += (35 + 40);
						size.height = static_cast<unsigned>(menu_->items.size() - size.height) * _m_item_height() + size.height + (menu_->items.size() - 1);
					}

					if(size.width > menu_->max_pixels)
						size.width = menu_->max_pixels;

					return size;
				}

				void menu_drawer::_m_adjust_window_size() const
				{
					nana::size size = _m_client_size();

					size.width += detail_.border.x * 2;
					size.height += detail_.border.y * 2;

					if(widget_->size() != size)
						widget_->size(size.width, size.height);

					//get the screen coordinates of the widget pos.
					nana::point pos;
					nana::gui::API::calc_screen_point(widget_->handle(), pos);
					nana::size ss = nana::gui::API::screen_size();

					if(pos.x + size.width > ss.width)
						pos.x = static_cast<int>(ss.width - size.width);
					if(pos.x < 0) pos.x = 0;

					if(pos.y + size.height > ss.height)
						pos.y = static_cast<int>(ss.height - size.height);
					if(pos.y < 0) pos.y = 0;
					nana::gui::window owner = nana::gui::API::get_owner_window(widget_->handle());
					nana::gui::API::calc_window_point(owner, pos);
					widget_->move(pos.x, pos.y);
				}

				//struct state
					menu_drawer::state::state()
						:active(npos), sub_window(false), nullify_mouse(false)
					{}
				//end struct state

				//struct widget_detail
					menu_drawer::widget_detail::widget_detail():border(2, 2)
					{}
				//end struct widget_detail
			//end class menu_drawer

			//class menu_window
				menu_window::menu_window(nana::gui::widget& widget, int x, int y)
					:	base_type(widget, false, x, y, 2, 2, nana::gui::appear::bald<nana::gui::appear::floating>())
				{
					_m_make_mouse_event();
				}

				menu_window::menu_window(nana::gui::window wnd, int x, int y)
					:	base_type(wnd, false, x, y, 2, 2, nana::gui::appear::bald<nana::gui::appear::floating>())
				{
					_m_make_mouse_event();
				}

				menu_window::menu_window(int x, int y)
					:	base_type(x, y, 2, 2, nana::gui::appear::bald<nana::gui::appear::floating>())
				{
					_m_make_mouse_event();
				}

				void menu_window::popup(menu_type& menu, bool owner_menubar)
				{
					this->get_drawer_trigger().set_menu_drawer(menu);
					nana::gui::API::take_active(this->handle(), false, 0);

					if(submenu_.parent == 0)
					{
						state_.owner_menubar = owner_menubar;
						nana::gui::API::register_menu_window(this->handle(), !owner_menubar);
					}

					timer_.interval(100);
					timer_.make_tick(nana::functor<void()>(*this, &self_type::_m_check_repeatly));

					this->make_event<nana::gui::events::destroy>(*this, &self_type::_m_destroy);
					this->make_event<nana::gui::events::key_down>(*this, &self_type::_m_key_down);
					this->make_event<nana::gui::events::mouse_up>(*this, &self_type::_m_strike);

					show();
				}

				void menu_window::goto_next(bool forward)
				{
					menu_window * object = this;

					while(object->submenu_.child)
						object = object->submenu_.child;

					state_.auto_popup_submenu = false;

					if(object->get_drawer_trigger().goto_next(forward))
						nana::gui::API::update_window(object->handle());
				}

				bool menu_window::goto_submenu()
				{
					menu_window * object = this;
					while(object->submenu_.child)
						object = object->submenu_.child;

					state_.auto_popup_submenu = false;

					nana::point pos;
					menu_type * sbm = object->get_drawer_trigger().retrive_sub_menu(pos, 0);
					return object->_m_show_submenu(sbm, pos, true);
				}

				bool menu_window::exit_submenu()
				{
					menu_window * object =this;
					while(object->submenu_.child)
						object = object->submenu_.child;

					state_.auto_popup_submenu = false;

					menu_window * parent = object->submenu_.parent;

					if(parent)
					{
						parent->submenu_.child = 0;
						parent->submenu_.object = 0;
						object->close();
						return true;
					}
					return false;
				}

				int menu_window::send_shortkey(nana::char_t key)
				{
					menu_window * object = this;
					while(object->submenu_.child)
						object = object->submenu_.child;

					return object->get_drawer_trigger().send_shortkey(key);
				}

				//_m_destroy just destroys the children windows.
				//If the all window including parent windows want to be closed call the _m_close_all() instead of close()
				void menu_window::_m_destroy()
				{
					if(this->submenu_.parent)
					{
						this->submenu_.parent->submenu_.child = 0;
						this->submenu_.parent->submenu_.object = 0;
					}

					if(this->submenu_.child)
					{
						menu_window * tail = this;
						while(tail->submenu_.child) tail = tail->submenu_.child;

						while(tail != this)
						{
							menu_window * junk = tail;
							tail = tail->submenu_.parent;
							junk->close();
						}
					}
				}

				void menu_window::_m_close_all()
				{

					menu_window * root = this;
					while(root->submenu_.parent)
						root = root->submenu_.parent;

					if(root != this)
					{
						//Disconnect the menu chain at this menu, and delete the menus before this.
						this->submenu_.parent->submenu_.child = 0;
						this->submenu_.parent->submenu_.object = 0;
						this->submenu_.parent = 0;
						root->close();
						//The submenu is treated its parent menu as an owner window,
						//if the root is closed, the all submenus will be closed
					}
					else
					{
						//Then, delete the menus from this.
						this->close();
					}
				}

				void menu_window::_m_strike()
				{
					menu_window * object = this;
					while(object->submenu_.child)
						object = object->submenu_.child;

					unsigned long active = object->get_drawer_trigger().active();
					if(active != npos)
					{
						menu_type * menu = object->get_drawer_trigger().get_menu();
						if(menu)
						{
							menu_item_type & item = menu->items.at(active);
							if(item.flags.splitter == false && item.sub_menu == 0)
							{
								//There is a situation that menu will not call functor if the item style is check_option
								//and it is checked before clicking.
								bool call_functor = true;

								if(item.style == gui::menu::check_highlight)
								{
									item.flags.checked = !item.flags.checked;
								}
								else if(gui::menu::check_option == item.style)
								{
									if(active > 0)
									{
										//clear the checked state in front of active if it is check_option.
										unsigned i = active;
										do
										{
											--i;
											menu_item_type & im = menu->items.at(i);
											if(im.flags.splitter) break;

											if(im.style == gui::menu::check_option && im.flags.checked)
												im.flags.checked = false;
										}while(i);
									}

									for(unsigned i = active + 1; i < menu->items.size(); ++i)
									{
										menu_item_type & im = menu->items.at(i);
										if(im.flags.splitter) break;

										if(im.style == gui::menu::check_option && im.flags.checked)
											im.flags.checked = false;
									}

									item.flags.checked = true;
								}

								this->_m_close_all();	//means deleting this;
								//The deleting operation has moved here, because item.functor.operator()(ip)
								//may create a window, which make a killing focus for menu window, if so the close_all
								//operation preformences after item.functor.operator()(ip), that would be deleting this object twice!

								if(call_functor && item.flags.enabled)
								{
									item_type::item_proxy ip(active, item);
									item.functor.operator()(ip);
								}
							}
						}
					}
				}

				void menu_window::_m_key_down(const nana::gui::eventinfo& ei)
				{
					switch(ei.keyboard.key)
					{
					case nana::gui::keyboard::up:
						this->goto_next(false);
						break;
					case nana::gui::keyboard::down:
						this->goto_next(true);
						break;
					case nana::gui::keyboard::left:
						this->exit_submenu();
						break;
					case nana::gui::keyboard::right:
						this->goto_submenu();
						break;
					case nana::gui::keyboard::enter:
						this->_m_strike();
						break;
					default:
						{
							nana::gui::window wnd = this->handle();
							if(2 != this->send_shortkey(ei.keyboard.key))
							{
								if(nana::gui::API::empty_window(wnd) == false)
									this->close();
							}
							else
								this->goto_submenu();
						}
					}
				}

				void menu_window::_m_make_mouse_event()
				{
					using namespace nana::gui;
					state_.mouse_pos = nana::gui::API::cursor_position();
					this->make_event<events::mouse_move>(*this, &self_type::_m_mouse_event);
				}

				void menu_window::_m_mouse_event()
				{
					nana::point pos = nana::gui::API::cursor_position();
					if(pos != state_.mouse_pos)
					{
						menu_window * root = this;
						while(root->submenu_.parent)
							root = root->submenu_.parent;
						root->state_.auto_popup_submenu = true;

						state_.mouse_pos = pos;
					}
				}

				bool menu_window::_m_show_submenu(menu_type* sbm, nana::point pos, bool forced)
				{
					if(submenu_.object && (sbm != submenu_.object))
					{
						this->get_drawer_trigger().set_sub_window(false);
						submenu_.child->close();
						submenu_.child = 0;
						submenu_.object = 0;
					}

					if(sbm)
					{
						menu_window * root = this;
						while(root->submenu_.parent)
							root = root->submenu_.parent;

						if((submenu_.object == 0) && sbm && (forced || root->state_.auto_popup_submenu))
						{
							menu_window & mwnd = nana::gui::form_loader<menu_window>()(this->handle(), pos.x, pos.y);
							mwnd.state_.self_submenu = true;
							submenu_.child = & mwnd;
							submenu_.child->submenu_.parent = this;
							submenu_.object = sbm;

							nana::gui::API::set_window_z_order(this->handle(), mwnd.handle(), z_order_action::none);

							mwnd.popup(*sbm, state_.owner_menubar);
							this->get_drawer_trigger().set_sub_window(true);
							if(forced)
								mwnd.goto_next(true);

							return true;
						}
					}
					return false;
				}

				void menu_window::_m_check_repeatly()
				{
					if(state_.auto_popup_submenu)
					{
						nana::point pos = nana::gui::API::cursor_position();

						drawer_type& drawer = this->get_drawer_trigger();
						nana::gui::API::calc_window_point(this->handle(), pos);
						drawer.track_mouse(pos.x, pos.y);
						menu_type* sbm = drawer.retrive_sub_menu(pos, 500);
						_m_show_submenu(sbm, pos, false);
					}
				}

				//struct state_type
					menu_window::state_type::state_type()
						:self_submenu(false), owner_menubar(false), auto_popup_submenu(true)
					{}
				//end struct state_type

				//struct submenu_type
					menu_window::submenu_type::submenu_type():parent(0), child(0), object(0){}
				//end struct submenu_type
			//end class menu_window

		}//end namespace menu


	}//end namespace drawerbase

	//class menu
		menu::menu()
			:window_(0)
		{}

		menu::~menu()
		{
			for(std::map<unsigned long, menu_info>::reverse_iterator i = sub_container_.rbegin(); i != sub_container_.rend(); ++i)
			{
				if(i->second.kill)
					delete i->second.handle;
			}
		}

		void menu::append(const nana::string& text, const menu::functor_type& f)
		{
			mbuilder_.append(text, f);
		}

		void menu::append_splitter()
		{
			mbuilder_.append_splitter();
		}

		void menu::clear()
		{
			mbuilder_.clear();
		}

		void menu::enabled(unsigned long index, bool enable)
		{
			mbuilder_.enabled(index, enable);
		}

		bool menu::enabled(unsigned long index) const
		{
			return mbuilder_.enabled(index);
		}

		void menu::erase(unsigned long index)
		{
			mbuilder_.erase(index);
		}

		void menu::image(size_t index, const paint::image& img)
		{
			mbuilder_.image(index, img);
		}

		bool menu::link(unsigned long index, menu& menu_obj)
		{
			if(mbuilder_.set_sub_menu(index, menu_obj.mbuilder_.get_root()))
			{
				menu_info& minfo = sub_container_[index];
				minfo.handle = &menu_obj;
				minfo.kill = false;
				return true;
			}
			return false;
		}

		menu* menu::link(unsigned long index)
		{
			std::map<unsigned long, menu_info>::iterator i = sub_container_.find(index);
			if(i == sub_container_.end())
				return 0;
			return i->second.handle;
		}

		menu *menu::create_sub_menu(unsigned long index)
		{
			self_type * sub = new self_type;

			if(link(index, *sub))
			{
				menu_info& minfo = sub_container_[index];
				minfo.handle = sub;
				minfo.kill = true;
				return sub;
			}

			delete sub;
			return 0;
		}

		void menu::popup(nana::gui::window wd, int x, int y, bool owner_menubar)
		{
			if(mbuilder_.size())
			{
				close();

				typedef drawerbase::menu::menu_window menu_window;
				window_ = &(nana::gui::form_loader<menu_window>()(wd, x, y));
				window_->make_event<nana::gui::events::destroy>(*this, &self_type::_m_destroy_menu_window);
				window_->popup(mbuilder_.get_root(), owner_menubar);
			}
		}

		void menu::close()
		{
			if(window_)
			{
				window_->close();
				window_ = 0;
			}
		}

		void menu::check_style(unsigned index, check_t style)
		{
			mbuilder_.check_style(index, style);
		}

		void menu::checked(unsigned index, bool check)
		{
			this->mbuilder_.checked(index, check);
		}

		bool menu::checked(unsigned index) const
		{
			return mbuilder_.checked(index);
		}

		void menu::answerer(size_t index, const menu::functor_type& fn)
		{
			mbuilder_.answerer(index, fn);
		}

		void menu::destroy_answer(const nana::functor<void()>& func)
		{
			functor_ = func;
		}

		void menu::goto_next(bool forward)
		{
			if(window_)
			{
				window_->goto_next(forward);
			}
		}

		bool menu::goto_submen()
		{
			return (window_ ? window_->goto_submenu() : false);
		}

		bool menu::exit_submenu()
		{
			return (window_ ? window_->exit_submenu() : false);
		}

		unsigned menu::size() const
		{
			return this->mbuilder_.size();
		}

		int menu::send_shortkey(nana::char_t key)
		{
			return (window_ ? window_->send_shortkey(key) : 0);
		}

		menu& menu::max_pixels(unsigned px)
		{
			mbuilder_.max_pixels(px);
			return *this;
		}

		unsigned menu::max_pixels() const
		{
			return mbuilder_.max_pixels();
		}

		void menu::_m_destroy_menu_window()
		{
			window_ = 0;
			functor_();
		}
	//end class menu

	//class menu_popuper
		menu_popuper::menu_popuper(menu& mobj)
			: mobj_(mobj), owner_(0), take_mouse_pos_(true), mouse_(mouse_right)
		{}

		menu_popuper::menu_popuper(menu& mobj, mouse_t mouse)
			: mobj_(mobj), owner_(0), take_mouse_pos_(true), mouse_(mouse)
		{}

		menu_popuper::menu_popuper(menu& mobj, window owner, int x, int y)
			: mobj_(mobj), owner_(owner), take_mouse_pos_(false), pos_(x, y), mouse_(mouse_right)
		{}

		menu_popuper::menu_popuper(menu& mobj, window owner, int x, int y, mouse_t mouse)
			: mobj_(mobj), owner_(owner), take_mouse_pos_(false), pos_(x, y), mouse_(mouse)
		{}

		void menu_popuper::operator()(const eventinfo& ei)
		{
			if(take_mouse_pos_)
			{
				switch(ei.identifier)
				{
				case events::click::identifier:
				case events::mouse_down::identifier:
				case events::mouse_up::identifier:
					owner_ = ei.window;
					pos_.x = ei.mouse.x;
					pos_.y = ei.mouse.y;
					break;
				default:
					return;
				}
			}
			bool popup = false;
			switch(mouse_)
			{
			case mouse_left:
				popup = (ei.mouse.left_button);
				break;
			case mouse_middle:
				popup = ei.mouse.mid_button;
				break;
			case mouse_right:
				popup = ei.mouse.right_button;
				break;
			case mouse_all:
				popup = true;
			}

			if(popup)
				mobj_.popup(owner_, pos_.x, pos_.y, false);
		}
	//end class
}//end namespace gui
}//end namespace nana
