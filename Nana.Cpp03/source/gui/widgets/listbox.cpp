/*
 *	A List Box Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/listbox.cpp
 *	@patchs:
 *		Jan 03 2012, unsigned to std::size_t conversion fail for x64, Hiroshi Seki
 */

#include <nana/gui/widgets/listbox.hpp>
#include <nana/gui/widgets/scroll.hpp>
#include <nana/gui/cursor.hpp>
#include <list>
#include <deque>
#include <stdexcept>
#include <nana/paint/gadget.hpp>

namespace nana{ namespace gui{
	namespace drawerbase
	{
		namespace listbox
		{
			const std::size_t npos = static_cast<size_t>(-1);

			class es_header
			{
			public:
				typedef std::size_t size_type;
				struct item_t
				{
					nana::string text;
					unsigned pixels;
					bool visible;
					size_type index;
				};

				typedef std::vector<item_t> container;

				es_header()
					:visible_(true)
				{}

				bool visible() const
				{
					return visible_;
				}

				bool visible(bool v)
				{
					if(visible_ != v)
					{
						visible_ = v;
						return true;
					}
					return false;
				}

				void create(const nana::string& text, unsigned pixels)
				{
					item_t m;
					m.text = text;
					m.pixels = pixels;
					m.visible = true;
					m.index = static_cast<size_type>(cont_.size());
					cont_.push_back(m);
				}

				void item_width(size_type index, unsigned width)
				{
					if(index < cont_.size())
					{
						for(container::iterator i = cont_.begin(); i != cont_.end(); ++i)
						{
							if(i->index == index)
								i->pixels = width;
						}
					}
				}

				unsigned pixels() const
				{
					unsigned pixels = 0;
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(i->visible)
							pixels += i->pixels;
					}
					return pixels;
				}

				size_type index(size_type n) const
				{
					return (n < cont_.size() ? cont_[n].index : npos);
				}

				const container& get() const
				{
					return cont_;
				}

				const item_t& get_item(size_type index) const
				{
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(i->index == index)
							return *i;
					}

					throw std::out_of_range("listbox, wrong header index.");
				}

				size_type item_by_x(int x) const
				{
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(x < static_cast<int>(i->pixels))
							return i->index;
						x -= i->pixels;
					}
					return npos;
				}

				bool item_pos(size_type index, int& xpos, unsigned& pixels) const
				{
					xpos = 0;
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(i->index == index)
						{
							pixels = i->pixels;
							return true;
						}
						else if(i->visible)
							xpos += i->pixels;
					}
					return true;
				}

				int xpos(size_type index) const
				{
					int x = 0;
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(i->index == index)
							break;
						else if(i->visible)
							x += i->pixels;
					}

					return x;
				}

				size_type neighbor(size_type index, bool front) const
				{
					size_type n = npos;
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(i->index == index)
						{
							if(front)	return n;
							for(++i; i != cont_.end(); ++i)
							{
								if(i->visible) return i->index;
							}
							break;
						}
						else if(i->visible)
							n = i->index;
					}
					return npos;
				}
				size_type begin() const
				{
					for(container::const_iterator i = cont_.begin(); i != cont_.end(); ++i)
					{
						if(i->visible) return i->index;
					}
					return npos;
				}

				size_type last() const
				{
					for(container::const_reverse_iterator i = cont_.rbegin(); i != cont_.rend(); ++i)
					{
						if(i->visible) return i->index;
					}
					return npos;
				}

				bool less(size_type a, size_type b) const
				{
					if(a != b && a < cont_.size() && b < cont_.size())
					{
						for(container::const_iterator i = cont_.begin();i != cont_.end();  ++i)
						{
							if(a == i->index) return true;
							if(b == i->index) return false;
						}
					}
					return false;
				}

				void move(size_type index, size_type to, bool front)
				{
					if((index != to) && (index < cont_.size()) && (to < cont_.size()))
					{
						item_t from;
						for(container::iterator i = cont_.begin();i != cont_.end();  ++i)
						{
							if(index == i->index)
							{
								from = *i;
								cont_.erase(i);
								break;
							}
						}

						for(container::iterator i = cont_.begin();i != cont_.end();  ++i)
						{
							if(to == i->index)
							{
								if(front)
									cont_.insert(i, from);
								else
									cont_.insert(++i, from);
								break;
							}
						}
					}
				}

				size_type size() const
				{
					return cont_.size();
				}
			private:
				bool visible_;
				container cont_;
			};

			class es_lister
			{
			public:
				struct item_t
				{
					typedef std::vector<nana::string> container;

					container texts;
					color_t bkcolor;
					color_t fgcolor;
					nana::paint::image img;

					struct flags_tag
					{
						bool select:1;
						bool checked:1;

						flags_tag():select(false), checked(false)
						{}
					}flags;
					nana::any * anyobj;

					item_t()
						:bkcolor(0xFF000000), fgcolor(0xFF000000), anyobj(0)
					{}

					item_t(const item_t& r)
						:	texts(r.texts), bkcolor(r.bkcolor), fgcolor(r.fgcolor), img(r.img),
							flags(r.flags), anyobj(r.anyobj ? new nana::any(*r.anyobj) : 0)
					{}

					~item_t()
					{
						delete anyobj;
					}

					item_t& operator=(const item_t& r)
					{
						if(this != &r)
						{
							texts = r.texts;
							flags = r.flags;
							anyobj = (r.anyobj? new nana::any(*r.anyobj) : 0);
							bkcolor = r.bkcolor;
							fgcolor = r.fgcolor;
							img = r.img;
						}
						return *this;
					}

				};

				struct category
				{
					typedef std::deque<item_t> container;

					nana::string text;
					container items;
					bool expand;

					bool select() const
					{
						for(container::const_iterator i = items.begin(); i != items.end(); ++i)
						{
							if(i->flags.select == false) return false;
						}
						return (items.size() != 0);
					}
				};
			public:
				typedef std::size_t size_type;
				typedef std::list<category> container;

				es_lister()
				{
					category cg;
					cg.expand = true;
					list_.push_back(cg);
				}

				void object(size_type categ, size_type index, const nana::any& anyobj)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
						{
							item_t & item = i->items[index];
							if(item.anyobj)
								*item.anyobj = anyobj;
							else
								item.anyobj = new nana::any(anyobj);
						}
					}
				}

				nana::any* object(size_type categ, size_type index)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							return i->items[index].anyobj;
					}
					return 0;
				}

				nana::any* object(size_type categ, size_type index) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							return i->items[index].anyobj;
					}
					return 0;
				}

				void create(const nana::string& text)
				{
					category cg;
					cg.expand = true;
					cg.text = text;
					list_.push_back(cg);
				}

				bool push_back(size_type categ, const nana::string& text)
				{
					if(categ < list_.size())
					{
						item_t item;
						item.texts.push_back(text);
						container::iterator i = list_.begin();
						std::advance(i, categ);
						i->items.push_back(item);
						return true;
					}
					return false;
				}

				bool insert(size_type categ, size_type index, const nana::string& text)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						item_t item;
						item.texts.push_back(text);
						if(index < i->items.size())
							i->items.insert(i->items.begin() + index, item);
						else if(index == i->items.size())
							i->items.push_back(item);
						else
							return false;
						return true;
					}
					return false;
				}

				void icon(size_type categ, size_type item, const nana::paint::image& img)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(item < i->items.size())
							i->items[item].img = img;
					}				
				}

				nana::paint::image icon(size_type categ, size_type index) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							return i->items[index].img;
					}
					return nana::paint::image();
				}

				void bkcolor(size_type categ, size_type item, nana::color_t color)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(item < i->items.size())
							i->items[item].bkcolor = color;
					}
				}

				nana::color_t bkcolor(size_type categ, size_type item) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						return (item < i->items.size() ? i->items[item].bkcolor : 0x0);
					}
					return 0x0;
				}

				void fgcolor(size_type categ, size_type item, nana::color_t color)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(item < i->items.size())
							i->items[item].fgcolor = color;
					}
				}

				nana::color_t fgcolor(size_type categ, size_type item) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						return (item < i->items.size() ? i->items[item].fgcolor : 0x0);
					}
					return 0x0;
				}

				bool clear(size_type categ)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						i->items.clear();
						return true;
					}
					return false;
				}

				void clear()
				{
					for(container::iterator i = list_.begin(); i != list_.end(); ++i)
						i->items.clear();
				}

				std::pair<size_type, size_type> advance(size_type categ, size_type index, size_type n)
				{
					std::pair<size_type, size_type> dpos(npos, npos);
					if(categ >= this->size_categ() || (index != npos && index >= this->size_item(categ))) return dpos;

					dpos.first = categ;
					dpos.second = index;

					while(n)
					{
						if(dpos.second == npos)
						{
							if(this->expand(dpos.first) == false)
							{
								if(dpos.first + 1 == size_categ())
									break;
								++dpos.first;
							}
							else
								dpos.second = 0;
							--n;
						}
						else
						{
							size_type rest = size_item(dpos.first) - dpos.second - 1;
							if(rest == 0)
							{
								if(dpos.first + 1 == size_categ())
									break;
								++dpos.first;
								dpos.second = npos;
								--n;
							}
							else
							{
								if(rest >= n)
								{
									dpos.second += n;
									break;
								}
								else
								{
									n -= rest;
									if(dpos.first + 1 >= size_categ())
									{
										dpos.second += rest;
										break;
									}
									dpos.second = npos;
									++dpos.first;
								}
							}
						}

					}
					return dpos;
				}

				size_type distance(size_type categ, size_type index, size_type to_categ, size_type to_index) const
				{
					if(categ == to_categ && index == to_index) return 0;

					if(to_categ == categ)
					{
						if(index > to_index && index != npos)
							std::swap(index, to_index);

						return (index == npos ? to_index + 1 : to_index - index);
					}
					else if(to_categ < categ)
					{
						std::swap(categ, to_categ);
						std::swap(index, to_index);
					}

					size_type n = 0;
					container::const_iterator i = list_.begin();
					std::advance(i, categ);
					if(index == npos)
					{
						if(i->expand)
							n = i->items.size();
					}
					else
						n = i->items.size() - (index + 1);

					for(++i, ++categ; i != list_.end(); ++i, ++categ)
					{
						++n; //this is a category
						if(categ != to_categ)
						{
							if(i->expand)
								n += i->items.size();
						}
						else
						{
							if(to_index != npos)
								n += (to_index + 1);
							break;
						}
					}
					return n;
				}
				void text(size_type categ, size_type index, size_type subitem, const nana::string& str, size_type header_size)
				{
					if(subitem < header_size && categ < list_.size())
					{
						std::list<category>::iterator i = list_.begin();
						std::advance(i, categ);

						if(index < i->items.size())
						{
							std::vector<nana::string> & cont = i->items[index].texts;
							if(subitem >= cont.size())
							{	//If the index of specified sub item is over the number of sub items that item contained,
								//it fills the non-exist items.
								cont.resize(subitem);
								cont.push_back(str);
							}
							else
								cont[subitem] = str;
						}
					}
				}

				void erase(size_type categ, size_type index)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							i->items.erase(i->items.begin() + index);
					}
				}

				void erase(size_type categ)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						if(categ)
						{
							std::advance(i, categ);
							list_.erase(i);
						}
						else
							i->items.clear();
					}
				}

				void erase()
				{
					container::iterator i = list_.begin();
					i->items.clear();
					for(++i; i != list_.end();)
					{
						i = list_.erase(i);
					}
				}

				bool expand(size_type categ, bool exp)
				{
					if(categ && (categ < list_.size()))
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);

						if(i->expand != exp)
						{
							i->expand = exp;
							return true;
						}
					}
					return false;
				}

				bool expand(size_type categ) const
				{
					if(categ && (categ < list_.size()))
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						return i->expand;
					}
					return false;
				}
				const category& get(size_type i) const
				{
					if(i < list_.size())
					{
						std::list<category>::const_iterator it = list_.begin();
						std::advance(it, i);
						return *it;
					}

					throw std::out_of_range("listbox: wrong category index");
				}

				const std::list<category>& get() const
				{
					return list_;
				}
				size_type numbers() const
				{
					size_type n = list_.size() - 1;

					for(std::list<category>::const_iterator i = list_.begin(); i != list_.end(); ++i)
					{
						if(i->expand)
							n += i->items.size();
					}
					return n;
				}

				void check_for_all(bool chk)
				{
					for(container::iterator i = list_.begin(); i != list_.end(); ++i)
					{
						for(category::container::iterator u = i->items.begin(); u != i->items.end(); ++u)
						{
							if(u->flags.checked != chk)	u->flags.checked = chk;
						}
					}
				}

				bool item_checked(size_type categ, size_type index) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							return (i->items)[index].flags.checked;
					}
					return false;
				}

				bool item_checked(size_type categ, size_type index, bool chk)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							if((i->items)[index].flags.checked != chk)
							{
								(i->items)[index].flags.checked = chk;
								return true;
							}
					}
					return false;
				}

				bool item_checked_reverse(size_type categ, size_type index)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
						{
							(i->items)[index].flags.checked = ! (i->items)[index].flags.checked;
							return true;
						}
					}
					return false;
				}

				void item_checked(std::vector<std::pair<size_type, size_type> >& vec) const
				{
					size_type categ_idx = 0;
					size_type index = 0;
					for(container::const_iterator i = list_.begin(); i != list_.end(); ++i, ++categ_idx)
					{
						index = 0;
						const category& categ = *i;
						for(category::container::const_iterator u = categ.items.begin(); u != categ.items.end(); ++u, ++index)
						{
							if(u->flags.checked)
								vec.push_back(std::pair<size_type, size_type>(categ_idx, index));
						}
					}
				}

				void select_for_all(bool sel)
				{
					for(container::iterator i = list_.begin(); i != list_.end(); ++i)
					{
						for(category::container::iterator u = i->items.begin(); u != i->items.end(); ++u)
						{
							if(u->flags.select != sel)	u->flags.select = sel;
						}
					}
				}

				bool item_selected(size_type categ, size_type index) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							return (i->items)[index].flags.select;
					}
					return false;
				}

				bool item_selected(size_type categ, size_type index, bool sel)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
							if((i->items)[index].flags.select != sel)
							{
								(i->items)[index].flags.select = sel;
								return true;
							}
					}
					return false;
				}

				void item_selected(std::vector<std::pair<size_type, size_type> >& vec) const
				{
					size_type categ_idx = 0;
					size_type index = 0;
					for(container::const_iterator i = list_.begin(); i != list_.end(); ++i, ++categ_idx)
					{
						index = 0;
						const category& categ = *i;
						for(category::container::const_iterator u = categ.items.begin(); u != categ.items.end(); ++u, ++index)
						{
							if(u->flags.select)
								vec.push_back(std::pair<size_type, size_type>(categ_idx, index));
						}
					}
				}

				void move_select(bool upwards)
				{
					std::vector<std::pair<size_type, size_type> > svec;
					this->item_selected(svec);

					//get the start pos for moving.
					std::pair<size_type, size_type> spos;
					if(svec.size())
					{
						this->select_for_all(false);
						spos = svec[0];
					}
					else
					{
						bool good = false;
						for(std::size_t i = 0; i < list_.size(); ++i)
						{
							if(this->size_item(i))
							{
								spos.first = i;
								spos.second = 0;
								good = true;
								break;
							}
						}
						if(good == false) return;
					}

					//start moving
					do{
						if(upwards == false)
						{
							if(size_item(spos.first) > spos.second)
							{
								++spos.second;
							}
							else if(size_categ() > spos.first)
							{
								++spos.first;
								spos.second = 0;
							}
							else
							{
								spos.first = 0;
								spos.second = 0;
							}
						}
						else
						{
							if(spos.second == 0)
							{
								//there is an item at least definitely, because the start pos is an available item.
								do
								{
									if(spos.first == 0)
										spos.first = this->size_categ() - 1;
									else
										--spos.first;
								}
								while(0 == size_item(spos.first));

								spos.second = size_item(spos.first) - 1;
							}
							else
								--spos.second;
						}
						if(this->expand(spos.first) == false)
							this->expand(spos.first, true);
					}while(this->item_selected(spos.first, spos.second, true) == false);
				}

				size_type size_categ() const
				{
					return list_.size();
				}

				size_type size_item(size_type categ) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						return i->items.size();
					}
					return 0;
				}

				nana::string item_text(size_type categ, size_type index, size_type sub) const
				{
					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size() && (sub < i->items[index].texts.size()))
							return i->items[index].texts[sub];
					}
					return nana::string();
				}

				bool categ_checked(size_type categ) const
				{
					if(list_.size() > categ)
					{
						container::const_iterator i_categ = list_.begin();
						std::advance(i_categ, categ);

						for(category::container::const_iterator i = i_categ->items.begin(); i != i_categ->items.end(); ++i)
						{
							if(i->flags.checked == false)
								return false;
						}
						return true;
					}
					return false;
				}

				bool categ_checked(size_type categ, bool chk)
				{
					bool changed = false;
					if(list_.size() > categ)
					{
						container::iterator i_categ = list_.begin();
						std::advance(i_categ, categ);
						category::container::iterator end = i_categ->items.end();

						for(category::container::iterator i = i_categ->items.begin(); i != end; ++i)
						{
							if(i->flags.checked != chk)
							{
								i->flags.checked = chk;
								changed = true;
							}
						}
					}
					return changed;
				}

				bool categ_checked_reverse(size_type categ)
				{
					if(list_.size() > categ)
					{
						bool chk = categ_checked(categ);
						return categ_checked(categ, !chk);
					}
					return false;
				}

				bool categ_selected(size_type categ) const
				{
					if(list_.size() > categ)
					{
						container::const_iterator i_categ = list_.begin();
						std::advance(i_categ, categ);

						for(category::container::const_iterator i = i_categ->items.begin(); i != i_categ->items.end(); ++i)
						{
							if(i->flags.select == false)
								return false;
						}
						return true;
					}
					return false;
				}

				bool categ_selected(size_type categ, bool sel)
				{
					bool changed = false;
					if(list_.size() > categ)
					{
						container::iterator i_categ = list_.begin();
						std::advance(i_categ, categ);
						category::container::iterator end = i_categ->items.end();

						for(category::container::iterator i = i_categ->items.begin(); i != end; ++i)
						{
							if(i->flags.select != sel)
							{
								i->flags.select = sel;
								changed = true;
							}
						}
					}
					return changed;
				}

				void reverse_item_select(size_type categ, size_type index)
				{
					if(categ < list_.size())
					{
						container::iterator i = list_.begin();
						std::advance(i, categ);
						if(index < i->items.size())
						{
							(i->items)[index].flags.select = !(i->items)[index].flags.select;
						}
					}
				}

				void reverse_categ_selected(size_type categ)
				{
					categ_selected(categ, ! categ_selected(categ));
				}

				std::pair<size_type, size_type> last() const
				{
					container::const_reverse_iterator i = list_.rbegin();
					size_type n = i->items.size();
					size_type categ = list_.size() - 1;
					if(categ == 0)
					{
						if(n)	--n;
					}
					else
					{
						if(i->expand)
						{
							n = i->items.size();
							if(n)
								--n;
							else
								n = npos;
						}
						else
							n = npos;
					}
					return std::pair<size_type, size_type>(categ, n);
				}

				bool check_item(size_type categ, size_type index) const
				{
					if(categ < list_.size())
					{
						if(index == npos) return true;
						container::const_iterator i = list_.begin();
						std::advance(i, categ);
						return (index < i->items.size());
					}
					return false;
				}

				bool good_item(size_type categ, size_type index, std::pair<size_type, size_type>& item) const
				{
					if(categ == 0 && index == npos)	index = 0;

					if(categ < list_.size())
					{
						if(index != npos)
						{
							container::const_iterator i = list_.begin();
							std::advance(i, categ);

							if(index >= i->items.size())
							{
								if(++i != list_.end())
								{
									++categ;
									index = npos;
								}
								else
									return false;
							}
						}

						item.first = categ;
						item.second = index;
						return true;
					}
					return false;
				}

				bool forward(size_type categ, size_type index, size_type offs, std::pair<size_type, size_type>& item) const
				{
					std::pair<size_type, size_type> good;
					if(good_item(categ, index, good) == false)
						return false;

					categ = good.first;
					index = good.second;

					if(offs == 0)
					{
						item.first = categ;
						item.second = index;
						return true;
					}

					if(list_.size() <= categ) return false;

					bool begin_from_categ = (index == npos);
					//this is a category, so offs should increase.
					if(begin_from_categ)
					{
						//because the first is a category, and offs must not be 0, the category would not be candidated.
						//the algorithm above to calc the offset item is always starting with a item.
						--offs;
						index = 0;
					}

					container::const_iterator i_categ = list_.begin();
					std::advance(i_categ, categ);

					if(i_categ->items.size() <= index) return false;

					if(i_categ->expand)
					{
						container::value_type::container::const_iterator i = i_categ->items.begin() + index;
						for(; i != i_categ->items.end(); ++i, ++index)
						{
							if(offs-- == 0)
							{
								item.first = categ;
								item.second = index;
								return true;
							}
						}


					}

					++categ;
					++i_categ;
					for(index = 0; i_categ != list_.end(); ++i_categ, ++categ, index = 0)
					{
						if(offs-- == 0)
						{
							item.first = categ;
							item.second = npos;
							return true;
						}

						if(i_categ->expand)
						{
							for(container::value_type::container::const_iterator i = i_categ->items.begin(); i != i_categ->items.end(); ++i, ++index)
							{
								if(offs-- == 0)
								{
									item.first = categ;
									item.second = index;
									return true;
								}
							}
						}
					}

					return false;
				}
				bool backward(size_type categ, size_type index, size_type offs, std::pair<size_type, size_type>& item) const
				{
					if(offs == 0)
					{
						item.first = categ;
						item.second = index;
					}

					if(categ < list_.size())
					{
						container::const_iterator i = list_.begin();
						std::advance(i, categ);

						size_type n = (index == npos ? 1 : index + 2);
						if(n <= offs)
						{
							offs -= n;
						}
						else
						{
							n -=offs;
							item.first = categ;
							item.second = (n == 1 ? npos : n - 2);
							return true;
						}

						while(i != list_.begin())
						{
							--i;
							--categ;

							n = (i->expand ? i->items.size() : 0) + 1;

							if(n > offs)
							{
								n -=offs;
								item.first = categ;
								item.second = (n == 1 ? npos : n - 2);
								return true;
							}
							else
								offs -= n;
						}
					}
					return false;
				}
			private:
				container list_;
			};

			//struct essence_t
			//@brief:	this struct gives many data for listbox,
			//			the state of the struct does not effect on member funcions, therefore all data members are public.
			struct essence_t
			{
				typedef std::size_t size_type;

				enum state_t{StateNormal, StateHighlight, StateGrab, StateFloat};
				enum where_t{WhereUnknown = -1, WhereHeader, WhereLister, WhereChecker};

				nana::gui::widget * window;
				nana::paint::graphics *graph;
				nana::gui::cursor	cursor;
				bool auto_draw;
				bool checkable;
				bool if_image;
				unsigned header_size;
				unsigned item_size;
				unsigned text_height;
				unsigned suspension_width;

				es_header header;
				es_lister lister;

				state_t ptr_state;
				nana::upoint pointer_where;	//x stands for which object, such as header and lister, y stands for item
											//if where == header, y indicates the item
											//if where == lister || where == checker, y indicates the offset to the scroll offset_y which stands for the first item displayed in lister.
											//if where == unknown, y ignored.

				struct
				{
					static const unsigned scale = 16;
					int offset_x;
					nana::upoint offset_y;	//x stands for category, y stands for item. "y == WhereUnknown" means that is a category.

					nana::gui::scroll<true> v;
					nana::gui::scroll<false> h;
				}scroll;

				essence_t()
					:	window(0), graph(0), auto_draw(true), checkable(false), if_image(false),
						header_size(25), item_size(24), text_height(0), suspension_width(0),
						ptr_state(StateNormal)
				{
					scroll.offset_x = 0;
					pointer_where.x = WhereUnknown;
				}

				nana::upoint scroll_y() const
				{
					return scroll.offset_y;
				}

				void scroll_y(const nana::upoint& pos)
				{
					if(pos.x < lister.size_categ())
					{
						scroll.offset_y.x = pos.x;
						size_type number = lister.size_item(pos.x);
						if(pos.y < number)
							scroll.offset_y.y = pos.y;
						else if(number)
							scroll.offset_y.y = static_cast<nana::upoint::value_type>(number - 1);
						else
							scroll.offset_y.y = (pos.x > 0 ? WhereUnknown : 0);
					}
				}

				//get the number of items that are contained in pixels
				//parameter with_rest means whether including extra one item that is not completely contained in reset pixels.
				size_type number_of_lister_items(bool with_rest) const
				{
					unsigned lister_s = graph->height() - 2 - (header.visible() ? header_size: 0) - (scroll.h.empty() ? 0 : scroll.scale);
					return (lister_s / item_size) + (with_rest && (lister_s % item_size) ? 1 : 0);
				}

				//keep the first selected item in the display area
				void trace_selected_item()
				{
					std::vector<std::pair<size_type, size_type> > svec;
					this->lister.item_selected(svec);
					if(0 == svec.size()) return;	//no selected, exit.

					std::pair<size_type, size_type> & item = svec[0];
					//Same with current scroll offset item.
					if(item.second == npos && item.first == scroll.offset_y.x && static_cast<int>(scroll.offset_y.y) == WhereUnknown)
						return;

					if(item.first < scroll.offset_y.x || (item.first == scroll.offset_y.x && (static_cast<int>(scroll.offset_y.y) != WhereUnknown) && (item.second == npos || item.second < scroll.offset_y.y)))
					{
						scroll.offset_y.x = static_cast<nana::upoint::value_type>(item.first);
						scroll.offset_y.y = static_cast<nana::upoint::value_type>(item.second);
						if(this->lister.expand(item.first) == false)
						{
							if(lister.categ_selected(item.first))
								scroll.offset_y.y = essence_t::WhereUnknown;
							else
								this->lister.expand(item.first, true);
						}
					}
					else
					{
						size_type numbers = this->number_of_lister_items(false);
						size_type off = this->lister.distance(scroll.offset_y.x, scroll.offset_y.y, item.first, item.second);
						if(numbers > off) return;
						std::pair<size_type, size_type> n_off = this->lister.advance(scroll.offset_y.x, scroll.offset_y.y, (off - numbers) + 1);
						if(n_off.first != npos)
						{
							scroll.offset_y.x = static_cast<nana::upoint::value_type>(n_off.first);
							scroll.offset_y.y = static_cast<nana::upoint::value_type>(n_off.second != npos ? n_off.second : WhereUnknown);
						}
					}

					adjust_scroll_life();
					adjust_scroll_value();
				}

				void adjust_scroll_value()
				{
					if(scroll.h.empty() == false)
					{
						unsigned width = 4 + (scroll.v.empty() ? 0 : scroll.scale - 1);
						if(width >= graph->width()) return;
						scroll.h.amount(header.pixels());
						scroll.h.range(graph->width() - width);
						scroll.h.value(scroll.offset_x);
					}

					if(scroll.v.empty() == false)
					{
						unsigned height = 2 + (scroll.h.empty() ? 0 : scroll.scale);
						if(height >= graph->width()) return;
						scroll.v.amount(lister.numbers());
						scroll.v.range(this->number_of_lister_items(false));
						size_type off = lister.distance(0, 0, scroll.offset_y.x, scroll.offset_y.y);
						scroll.v.value(off);
					}
				}

				void adjust_scroll_life()
				{
					unsigned header_s = header.pixels();
					bool h = false, v = false;

					if(header_s > graph->width() - 4)
						h = true;

					unsigned lister_s = graph->height() - 2 - (header.visible() ? header_size: 0) - (h ? scroll.scale : 0);
					size_type screen_number = (lister_s / item_size);

					if(lister.numbers() > screen_number)
						v = true;

					if(v == true && h == false)
						h = (header_s > (graph->width() - 2 - scroll.scale));

					unsigned width = graph->width() - 2 - (v ? scroll.scale : 0);
					unsigned height = graph->height() - 2 - (h ? scroll.scale : 0);

					if(h)
					{
						if(false == scroll.h.empty())
						{
							scroll.h.move(1, graph->height() - scroll.scale - 1);
							scroll.h.size(width, scroll.scale);
						}
						else
						{
							scroll.h.create(window->handle(), nana::rectangle(1, graph->height() - scroll.scale - 1, width, scroll.scale));
							nana::gui::API::take_active(scroll.h.handle(), false, window->handle());
							scroll.h.make_event<nana::gui::events::mouse_move>(*this, &essence_t::_m_answer_scroll);
							scroll.h.make_event<nana::gui::events::mouse_up>(*this, &essence_t::_m_answer_scroll);

						}
					}
					else if(!scroll.h.empty())
						scroll.h.close();

					if(v)
					{
						if(false == scroll.v.empty())
						{
							scroll.v.move(graph->width() - 1 - scroll.scale, 1);
							scroll.v.size(scroll.scale, height);
						}
						else
						{
							scroll.v.create(window->handle(), nana::rectangle(graph->width() - 1 - scroll.scale, 1, scroll.scale, height));
							nana::gui::API::take_active(scroll.v.handle(), false, window->handle());
							scroll.v.make_event<nana::gui::events::mouse_move>(*this, &essence_t::_m_answer_scroll);
							scroll.v.make_event<nana::gui::events::mouse_up>(*this, &essence_t::_m_answer_scroll);
						}
					}
					else if(!scroll.v.empty())
					{
						scroll.v.close();
						scroll.offset_y.x = scroll.offset_y.y = 0;

						nana::rectangle r;
						if(this->rect_header(r))
						{
							if(header_s > r.width)
							{
								if((header_s - scroll.offset_x) < r.width)
									scroll.offset_x = header_s - r.width;
							}
							else
								scroll.offset_x = 0;
						}


					}

					adjust_scroll_value();
				}

				void set_auto_draw(bool ad)
				{
					if(auto_draw != ad)
					{
						auto_draw = ad;
						if(ad)
						{
							this->adjust_scroll_life();
							nana::gui::API::refresh_window(window->handle());
						}
					}
				}

				nana::rectangle checkarea(int x, int y) const
				{
					return nana::rectangle(x + 4, y + (this->item_size - 16) / 2, 16, 16);
				}

				bool is_checkarea(const nana::point& item_pos, const nana::point& mspos) const
				{
					nana::rectangle r = checkarea(item_pos.x, item_pos.y);
					return ((r.x <= mspos.x && mspos.x <= r.x + static_cast<int>(r.width)) && (r.y <= mspos.y && mspos.y < r.y + static_cast<int>(r.height)));
				}

				int item_xpos(const nana::rectangle& r) const
				{
					std::vector<es_header::size_type> seq;
					this->header_seq(seq, r.width);
					return (seq.size() ? (this->header.xpos(seq[0]) - this->scroll.offset_x + r.x) : 0);
				}

				bool calc_where(int x, int y)
				{
					nana::upoint new_where;

					if(2 < x && x < static_cast<int>(graph->width()) - 2 && 1 < y && y < static_cast<int>(graph->height()) - 1)
					{
						if(header.visible() && y < static_cast<int>(header_size + 1))
						{
							x -= (2 - scroll.offset_x);
							new_where.x = this->WhereHeader;
							new_where.y = static_cast<int>(this->header.item_by_x(x));
						}
						else
						{
							new_where.y = (y - (header.visible() ? header_size : 0) + 1) / item_size;
							new_where.x = this->WhereLister;
							if(this->checkable)
							{
								nana::rectangle r;
								if(this->rect_lister(r))
								{
									int top = new_where.y * item_size + (header.visible() ? header_size : 0);
									if(this->is_checkarea(nana::point(this->item_xpos(r), top), nana::point(x, y)))
										new_where.x = this->WhereChecker;
								}
							}
						}
					}
					else
						new_where.x = new_where.y = WhereUnknown;

					if(new_where != pointer_where)
					{
						pointer_where = new_where;
						return true;
					}
					return false;
				}

				void widget_to_header(nana::point& pos)
				{
					--pos.y;
					pos.x += (scroll.offset_x - 2);
				}

				bool rect_header(nana::rectangle& r) const
				{
					if(header.visible())
					{
						const unsigned ex_width = 4 + (scroll.v.empty() ? 0 : scroll.scale - 1);
						if(graph->width() > ex_width)
						{
							r.x = 2;
							r.y = 1;
							r.width = graph->width() - ex_width;
							r.height = header_size;
							return true;
						}
					}
					return false;
				}
				bool rect_lister(nana::rectangle& r) const
				{
					unsigned width = 4 + (scroll.v.empty() ? 0 : scroll.scale - 1);
					unsigned height = 2 + (scroll.h.empty() ? 0 : scroll.scale) + (header.visible() ? header_size : 0);

					if(graph->width() <= width || graph->height() <= height) return false;

					r.x = 2;
					r.y = (header.visible() ? header_size + 1 : 1);
					r.width = graph->width() - width;
					r.height = graph->height() - height;
					return true;
				}

				bool wheel(bool upwards)
				{
					std::pair<size_type, size_type> target;

					if(upwards == false)
						lister.forward(scroll.offset_y.x, scroll.offset_y.y, 1, target);
					else
						lister.backward(scroll.offset_y.x, scroll.offset_y.y, 1, target);

					if(target.first != scroll.offset_y.x || target.second != scroll.offset_y.y)
					{
						scroll.offset_y.x = static_cast<nana::upoint::value_type>(target.first);
						scroll.offset_y.y = static_cast<nana::upoint::value_type>(target.second);
						return true;
					}
					return false;
				}

				void header_seq(std::vector<es_header::size_type> &seqs, unsigned lister_w)const
				{
					int x = - (this->scroll.offset_x);
					for(std::vector<es_header::item_t>::const_iterator i = this->header.get().begin(); i != this->header.get().end(); ++i)
					{
						if(false == i->visible) continue;
						x += i->pixels;
						if(x > 0)
							seqs.push_back(i->index);
						if(x >= static_cast<int>(lister_w))
							break;
					}
				}
			private:
				void _m_answer_scroll(const nana::gui::eventinfo& ei)
				{
					if(ei.identifier == nana::gui::events::mouse_move::identifier && ei.mouse.left_button == false) return;

					bool update = false;
					if(ei.window == scroll.v.handle())
					{
						std::pair<size_type, size_type> item;
						if(lister.forward(0, 0, scroll.v.value(), item))
						{
							if(item.second != scroll.offset_y.y || item.first != scroll.offset_y.x)
							{
								scroll.offset_y.x = static_cast<nana::upoint::value_type>(item.first);
								scroll.offset_y.y = static_cast<nana::upoint::value_type>(item.second);
								update = true;
							}
						}
					}
					else if(ei.window == scroll.h.handle())
					{
						if(scroll.offset_x != static_cast<int>(scroll.h.value()))
						{
							scroll.offset_x = static_cast<int>(scroll.h.value());
							update = true;
						}
					}

					if(update)
						nana::gui::API::refresh_window(window->handle());
				}
			};

			class drawer_header_impl
			{
			public:
				typedef es_header::size_type size_type;
				typedef nana::paint::graphics & graph_reference;

				drawer_header_impl(essence_t* es): item_spliter_(npos), essence_(es){}

				size_type item_spliter() const
				{
					return item_spliter_;
				}

				void cancel_spliter()
				{
					item_spliter_ = npos;
				}

				bool mouse_spliter(const nana::rectangle& r, int x)
				{
					if(essence_->ptr_state == essence_->StateHighlight)
					{
						x -= (r.x - essence_->scroll.offset_x);

						for(es_header::container::const_iterator i = essence_->header.get().begin(); i != essence_->header.get().end(); ++i)
						{
							if(i->visible)
							{
								if((static_cast<int>(i->pixels)  - 2 < x) && (x < static_cast<int>(i->pixels) + 3))
								{
									item_spliter_ = i->index;
									return true;
								}
								x -= i->pixels;
							}
						}
					}
					else if(essence_->ptr_state == essence_->StateNormal)
						item_spliter_ = npos;
					return false;
				}

				void grab(const nana::point& pos, bool is_grab)
				{
					if(is_grab)
					{
						ref_xpos_ = pos.x;
						if(item_spliter_ != npos)
							orig_item_width_ = essence_->header.get_item(item_spliter_).pixels;
					}
					else if(grab_terminal_.index != npos && grab_terminal_.index != essence_->pointer_where.y)
							essence_->header.move(essence_->pointer_where.y, grab_terminal_.index, grab_terminal_.place_front);
				}


				//grab_move
				//@brief: draw when an item is grabbing.
				//@return: 0 = no graphics changed, 1 = just update, 2 = refresh
				int grab_move(const nana::rectangle& rect, const nana::point& pos)
				{
					if(item_spliter_ == npos)
					{
						this->draw(rect);
						_m_make_float(rect, pos);

						//Draw the target strip
						grab_terminal_.index = _m_target_strip(pos.x, rect, essence_->pointer_where.y, grab_terminal_.place_front);
						return 1;
					}
					else
					{
						const es_header::item_t& item = essence_->header.get_item(item_spliter_);
						//Resize the item specified by item_spliter_.
						int new_w = orig_item_width_ - (ref_xpos_ - pos.x);
						if(static_cast<int>(item.pixels) != new_w)
						{
							essence_->header.item_width(item_spliter_, (new_w < static_cast<int>(essence_->suspension_width + 20) ? essence_->suspension_width + 20 : new_w));
							unsigned new_w = essence_->header.pixels();
							if(new_w < rect.width + essence_->scroll.offset_x)
							{
								if(new_w > rect.width)
									essence_->scroll.offset_x = new_w - rect.width;
								else
									essence_->scroll.offset_x = 0;
							}
							essence_->adjust_scroll_life();
							return 2;
						}
					}
					return 0;
				}

				void draw(const nana::rectangle& r)
				{
					_m_draw(essence_->header.get(), r);

					const int y = r.y + r.height - 1;
					essence_->graph->line(r.x, y, r.x + r.width, y, 0xDEDFE1);
				}
			private:
				size_type _m_target_strip(int x, const nana::rectangle& rect, size_type grab, bool& place_front)
				{
					//convert x to header logic coordinate.
					if(x < essence_->scroll.offset_x)
						x = essence_->scroll.offset_x;
					else if(x > essence_->scroll.offset_x + static_cast<int>(rect.width))
						x = essence_->scroll.offset_x + rect.width;

					size_type i = essence_->header.item_by_x(x);
					if(i == npos)
					{
						if(essence_->header.xpos(grab) < x)
							i = essence_->header.last();
						else
							i = essence_->header.begin();
					}
					if(grab != i)
					{
						int item_xpos;
						unsigned item_pixels;
						if(essence_->header.item_pos(i, item_xpos, item_pixels))
						{
							int midpos = item_xpos + static_cast<int>(item_pixels / 2);

							//Get the item pos
							//if mouse pos is at left of an item middle, the pos of itself otherwise the pos of the next.

							place_front = (x <= midpos);
							x = (place_front ? item_xpos : essence_->header.xpos(essence_->header.neighbor(i, false)));

							if(i != npos)
								essence_->graph->rectangle(x - essence_->scroll.offset_x + rect.x, rect.y, 2, rect.height, 0xFF0000, true);
						}
						return i;
					}
					return npos;

				}

				template<typename Container>
				void _m_draw(const Container& cont, const nana::rectangle& rect)
				{
					graph_reference graph = *(essence_->graph);
					int x = rect.x - essence_->scroll.offset_x;
					unsigned height = rect.height - 1;

					int txtop = (rect.height - essence_->text_height) / 2 + rect.y;
					nana::color_t txtcolor = essence_->window->foreground();

					int state = essence_->StateNormal;

					//check whether grabing an item, if item_spliter_ != npos, that indicates the grab item is a spliter.
					if(essence_->pointer_where.x == essence_->WhereHeader && (item_spliter_ == npos))
						state = essence_->ptr_state;

					for(typename Container::const_iterator i = cont.begin(); i != cont.end(); ++i)
					{
						if(i->visible)
						{
							if(x + static_cast<int>(i->pixels) > rect.x)
							{
								_m_draw_item(graph, x, rect.y, height, txtop, txtcolor, *i, (i->index == essence_->pointer_where.y ? state : essence_->StateNormal));

								essence_->graph->line(x - 1 + i->pixels, rect.y, x - 1 + i->pixels, static_cast<int>(rect.y + rect.height - 2), 0xDEDFE1);
							}
							x += i->pixels;

							if(x - rect.x > static_cast<int>(rect.width)) break;
						}
					}

					if(x - rect.x < static_cast<int>(rect.width))
						essence_->graph->rectangle(x, rect.y, rect.width - x + rect.x, height, 0xF1F2F4, true);
				}

				template<typename Item>
				void _m_draw_item(graph_reference graph, int x, int y, unsigned height, int txtop, nana::color_t txtcolor, const Item& item, int state)
				{
					nana::color_t bkcolor;
					switch(state)
					{
					case essence_t::StateNormal:	bkcolor = 0xF1F2F4; break;
					case essence_t::StateHighlight:	bkcolor = 0xFFFFFF; break;
					case essence_t::StateGrab:		bkcolor = 0x8BD6F6; break;
					case essence_t::StateFloat:
						bkcolor = 0xBABBBC;
						break;
					}

					graph.rectangle(x, y, item.pixels, height, bkcolor, true);
					graph.string(x + 5, txtop, txtcolor, item.text);
				}

				void _m_make_float(const nana::rectangle& rect, const nana::point& pos)
				{
					const es_header::item_t & item = essence_->header.get_item(essence_->pointer_where.y);

					nana::paint::graphics ext_graph(item.pixels, essence_->header_size);
					ext_graph.typeface(essence_->graph->typeface());

					int txtop = (essence_->header_size - essence_->text_height) / 2;

					this->_m_draw_item(ext_graph, 0, 0, essence_->header_size, txtop, 0xFFFFFF, item, essence_->StateFloat);

					int xpos = essence_->header.xpos(item.index) + pos.x - ref_xpos_;

					ext_graph.blend(*(essence_->graph), xpos - essence_->scroll.offset_x + rect.x, rect.y, 0.5);
				}

			private:
				int			ref_xpos_;
				unsigned	orig_item_width_;
				size_type	item_spliter_;
				struct grab_terminal
				{
					size_type index;
					bool place_front;
				}grab_terminal_;
				essence_t * essence_;
			};

			class drawer_lister_impl
			{
			public:
				typedef es_lister::size_type size_type;

				drawer_lister_impl(essence_t * es): essence_(es){}

				void draw(const nana::rectangle& rect) const
				{
					typedef es_lister::container::value_type::container container;

					size_type n = essence_->number_of_lister_items(true);
					if(0 == n)return;
					nana::color_t bkcolor = essence_->window->background();
					nana::color_t txtcolor = essence_->window->foreground();

					unsigned header_w = essence_->header.pixels();
					if(header_w - essence_->scroll.offset_x < rect.width)
						essence_->graph->rectangle(rect.x + header_w - essence_->scroll.offset_x, rect.y, rect.width - (header_w - essence_->scroll.offset_x), rect.height, bkcolor, true);

					//The Tracker indicates the item where mouse placed.
					std::pair<es_lister::size_type, es_lister::size_type> tracker(npos, npos);
					if((essence_->pointer_where.x == essence_->WhereLister || essence_->pointer_where.x == essence_->WhereChecker) && essence_->pointer_where.y != npos)
						essence_->lister.forward(essence_->scroll.offset_y.x, essence_->scroll.offset_y.y, essence_->pointer_where.y, tracker);

					std::vector<es_header::size_type> subitems;
					essence_->header_seq(subitems, rect.width);

					if(subitems.size() == 0) return;

					int x = essence_->item_xpos(rect);
					int y = rect.y;
					int txtoff = (essence_->item_size - essence_->text_height) / 2;

					es_lister::container::const_iterator i_categ = essence_->lister.get().begin();
					std::advance(i_categ, essence_->scroll.offset_y.x);

					size_type catg_idx = essence_->scroll.offset_y.x;
					size_type item_idx = essence_->scroll.offset_y.y;

					essence_t::state_t state = essence_t::StateNormal;
					//Here draws a root categ or a first drawing is not a categ.
					if(catg_idx == 0 || item_idx != npos)
					{
						if(catg_idx == 0 && item_idx == npos)
						{
							essence_->scroll.offset_y.y = 0;
							item_idx = 0;
						}

						for(container::const_iterator i = i_categ->items.begin() + essence_->scroll.offset_y.y; i != i_categ->items.end(); ++i, ++item_idx)
						{
							if(n-- == 0) break;
							if(tracker.first == catg_idx && tracker.second == item_idx)
								state = essence_->StateHighlight;
							else
								state = essence_->StateNormal;

							_m_draw_item(*i, x, y, txtoff, header_w, rect, subitems, bkcolor, txtcolor, state);
							y += essence_->item_size;
						}
						++i_categ;
						++catg_idx;
					}

					for(; i_categ != essence_->lister.get().end(); ++i_categ, ++catg_idx)
					{
						if(n-- == 0) break;
						item_idx = 0;

						if(tracker.second == npos && tracker.first == catg_idx)
							state = essence_->StateHighlight;
						else
							state = essence_->StateNormal;

						_m_draw_categ(*i_categ, rect.x - essence_->scroll.offset_x, y, txtoff, header_w, rect, bkcolor, state);
						y += essence_->item_size;

						if(false == i_categ->expand) continue;

						for(container::const_iterator i = i_categ->items.begin(); i != i_categ->items.end(); ++i, ++item_idx)
						{
							if(n-- == 0) break;

							if(tracker.first == catg_idx && tracker.second == item_idx)
								state = essence_->StateHighlight;
							else
								state = essence_->StateNormal;

							_m_draw_item(*i, x, y, txtoff, header_w, rect, subitems, bkcolor, txtcolor, state);
							y += essence_->item_size;
						}
					}

					if(y < rect.y + static_cast<int>(rect.height))
						essence_->graph->rectangle(rect.x, y, rect.width, rect.y + rect.height - y, bkcolor, true);
				}
			private:
				void _m_draw_categ(const es_lister::category& categ, int x, int y, int txtoff, unsigned width, const nana::rectangle& r, nana::color_t bkcolor, int state) const
				{
					bool sel = categ.select();
					if(sel && (categ.expand == false))
						bkcolor = 0xD5EFFC;

					if(state == essence_->StateHighlight)
						bkcolor = essence_->graph->mix(bkcolor, 0x99DEFD, 0.8);

					essence_->graph->rectangle(x, y, width, essence_->item_size, bkcolor, true);

					nana::paint::gadget::arrow_16_pixels(*(essence_->graph), x + 5, y + (essence_->item_size - 16) /2, 0x3399, 2, (categ.expand ? nana::paint::gadget::directions::to_north : nana::paint::gadget::directions::to_south));
					nana::size text_s = essence_->graph->text_extent_size(categ.text);
					essence_->graph->string(x + 20, y + txtoff, 0x3399, categ.text);

					std::stringstream ss;
					ss<<'('<<static_cast<unsigned>(categ.items.size())<<')';
					nana::string str = nana::charset(ss.str());

					unsigned str_w = essence_->graph->text_extent_size(str).width;

					essence_->graph->string(x + 25 + text_s.width, y + txtoff, 0x3399, str);

					if(x + 35 + text_s.width + str_w < x + width)
						essence_->graph->line(x + 30 + text_s.width + str_w, y + essence_->item_size / 2, x + width - 5, y + essence_->item_size / 2, 0x3399);

					//Draw selecting inner rectangle
					if(sel && categ.expand == false)
					{
						width -= essence_->scroll.offset_x;
						_m_draw_border(r.x, y, (r.width < width ? r.width : width));
					}
				}

				void _m_draw_item(const es_lister::item_t& item, int x, int y, int txtoff, unsigned width, const nana::rectangle& r, const std::vector<size_type>& seqs, nana::color_t bkcolor, nana::color_t txtcolor, essence_t::state_t state) const
				{
					if(item.flags.select)
						bkcolor = 0xD5EFFC;
					else if((item.bkcolor & 0xFF000000) == 0)
						bkcolor = item.bkcolor;

					if((item.fgcolor & 0xFF000000) == 0)
						txtcolor = item.fgcolor;

					if(state == essence_->StateHighlight)
					{
						bkcolor = essence_->graph->mix(bkcolor, 0x99DEFD, 0.8);
					}

					unsigned show_w = width - essence_->scroll.offset_x;
					if(show_w >= r.width) show_w = r.width;

					//draw the background
					essence_->graph->rectangle(r.x, y, show_w, essence_->item_size, bkcolor, true);

					int img_off = (essence_->if_image ? (essence_->item_size - 16) / 2 : 0);
					
					int item_xpos = x;
					bool first = true;
					for(std::vector<size_type>::const_iterator i = seqs.begin(); i != seqs.end(); ++i)
					{
						size_type index = *i;

						const es_header::item_t & header = essence_->header.get_item(index);

						if((item.texts.size() > index) && (header.pixels > 5))
						{
							int ext_w = 0;
							if(first && essence_->checkable)
							{
								ext_w = 18;
								nana::rectangle chkarea = essence_->checkarea(item_xpos, y);

								nana::gui::mouse_action_t act = nana::gui::mouse_action_normal;

								if(essence_->pointer_where.x == essence_->WhereChecker)
								{
									switch(state)
									{
									case essence_t::StateNormal:
										break;
									case essence_t::StateHighlight:
										act = nana::gui::mouse_action_over;
										break;
									case essence_t::StateGrab:
										act = nana::gui::mouse_action_pressed;
										break;
									default:	break;
									}
								}

								this->chk_renderer_.render(*essence_->graph, chkarea.x, chkarea.y, chkarea.width, chkarea.height, act, chk_renderer_.clasp, item.flags.checked);
							}
							nana::size ts = essence_->graph->text_extent_size(item.texts[index]);

							if((0 == index) && essence_->if_image)
							{
								ext_w += 18;
								if(item.img)
									item.img.stretch(nana::rectangle(), *essence_->graph, nana::rectangle(item_xpos + 5, y + img_off, 16, 16));
							}
							essence_->graph->string(item_xpos + 5 + ext_w, y + txtoff, txtcolor, item.texts[index]);

							if(ts.width + 5 + ext_w > header.pixels)
							{//The text is painted over the next subitem
								int xpos = item_xpos + header.pixels - essence_->suspension_width;
								essence_->graph->rectangle(xpos, y + 2, ts.width + 5 + ext_w - header.pixels + essence_->suspension_width, essence_->item_size - 4, bkcolor, true);
								essence_->graph->string(xpos, y + 2, txtcolor, STR("..."));
							}
						}

						essence_->graph->line(item_xpos - 1, y, item_xpos - 1, y + essence_->item_size - 1, 0xEBF4F9);

						item_xpos += header.pixels;
						first = false;
					}

					//Draw selecting inner rectangle
					if(item.flags.select)
					{
						_m_draw_border(r.x, y, show_w);
					}
				}
				void _m_draw_border(int x, int y, unsigned width) const
				{
					//Draw selecting inner rectangle
					nana::paint::graphics * graph = essence_->graph;
					graph->rectangle(x , y , width, essence_->item_size, 0x99DEFD, false);

					graph->rectangle(x + 1, y + 1, width - 2, essence_->item_size - 2, 0xFFFFFF, false);
					graph->set_pixel(x, y, 0xFFFFFF);
					graph->set_pixel(x, y + essence_->item_size - 1, 0xFFFFFF);
					graph->set_pixel(x + width - 1, y, 0xFFFFFF);
					graph->set_pixel(x + width - 1, y + essence_->item_size - 1, 0xFFFFFF);
				}
			private:
				essence_t * essence_;
				mutable nana::paint::gadget::check_renderer chk_renderer_;
			};

			//class trigger: public nana::gui::drawer_trigger
				trigger::trigger()
					: essence_(new essence_t),
						drawer_header_(new drawer_header_impl(essence_)),
						drawer_lister_(new drawer_lister_impl(essence_))
				{}

				trigger::~trigger()
				{
					delete drawer_lister_;
					delete drawer_header_;
					delete essence_;
				}

				essence_t& trigger::essence()
				{
					return *essence_;
				}

				essence_t& trigger::essence() const
				{
					return *essence_;
				}

				void trigger::draw()
				{
					nana::rectangle rect;

					if(essence_->header.visible())
					{
						essence_->rect_header(rect);
						drawer_header_->draw(rect);
					}
					essence_->rect_lister(rect);

					drawer_lister_->draw(rect);
					_m_draw_border();
				}

				void trigger::update()
				{
					if(essence_->auto_draw)
					{
						essence_->adjust_scroll_life();
						nana::gui::API::refresh_window(essence_->window->handle());
					}
				}

				void trigger::_m_draw_border()
				{
					nana::paint::graphics * graph = essence_->graph;
					//Draw Border
					graph->rectangle(0, 0, graph->width(), graph->height(), 0x9CB6C5, false);
					graph->line(1, 1, 1, graph->height() - 2, 0xFFFFFF);
					graph->line(graph->width() - 2, 1, graph->width() - 2, graph->height() - 2, 0xFFFFFF);

					if((essence_->scroll.h.empty() == false) && (essence_->scroll.v.empty() == false))
						graph->rectangle(graph->width() - 1 - essence_->scroll.scale, graph->height() - 1 - essence_->scroll.scale, essence_->scroll.scale, essence_->scroll.scale, nana::gui::color::button_face, true);
				}

				void trigger::bind_window(trigger::widget_reference wd)
				{
					essence_->window = &wd;
					essence_->cursor.bind(wd);
					wd.background(0xFFFFFF);
				}

				void trigger::attached(trigger::graph_reference graph)
				{
					essence_->graph = &graph;
					essence_->text_height = graph.text_extent_size(STR("jHWn0123456789/<?'{[|\\_")).height;
					essence_->item_size = essence_->text_height + 10;
					essence_->suspension_width = graph.text_extent_size(STR("...")).width;

					window wd = essence_->window->handle();
					using namespace API::dev;
					make_drawer_event<events::mouse_move>(wd);
					make_drawer_event<events::mouse_leave>(wd);
					make_drawer_event<events::mouse_down>(wd);
					make_drawer_event<events::mouse_up>(wd);
					make_drawer_event<events::dbl_click>(wd);
					make_drawer_event<events::size>(wd);
					make_drawer_event<events::mouse_wheel>(wd);
					make_drawer_event<events::key_down>(wd);
				}

				void trigger::detached()
				{
					essence_->graph = 0;
					API::dev::umake_drawer_event(essence_->window->handle());
				}

				void trigger::refresh(trigger::graph_reference)
				{
					this->draw();
				}

				void trigger::mouse_move(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					int update = 0; //0 = nothing, 1 = update, 2 = refresh

					if(essence_->ptr_state == essence_->StateGrab)
					{
						nana::point pos(ei.mouse.x, ei.mouse.y);
						essence_->widget_to_header(pos);

						nana::rectangle r;
						essence_->rect_header(r);
						update = drawer_header_->grab_move(r, pos);
					}
					else if(essence_->calc_where(ei.mouse.x, ei.mouse.y))
					{
						essence_->ptr_state = essence_->StateHighlight;
						update = 2;
					}

					bool set_spliter = false;
					if(essence_->pointer_where.x == essence_->WhereHeader)
					{
						nana::rectangle r;
						if(essence_->rect_header(r))
						{
							if(drawer_header_->mouse_spliter(r, ei.mouse.x))
							{
								set_spliter = true;
								essence_->cursor.load(nana::gui::cursor::predef::size_we);
							}
						}
					}
					if(set_spliter == false && essence_->ptr_state != essence_->StateGrab)
					{
						if((drawer_header_->item_spliter() != npos) || (essence_->cursor.get() == nana::gui::cursor::predef::size_we))
						{
							essence_->cursor.load(nana::gui::cursor::predef::arrow);
							drawer_header_->cancel_spliter();
							update = 2;
						}
					}

					switch(update)
					{
					case 1:
						nana::gui::API::update_window(essence_->window->handle());
						break;
					case 2:
						this->draw();
						nana::gui::API::lazy_refresh();
						break;
					}
				}

				void trigger::mouse_leave(trigger::graph_reference graph, const nana::gui::eventinfo&)
				{
					if(static_cast<int>(essence_->pointer_where.x) != essence_->WhereUnknown || essence_->ptr_state != essence_->StateNormal)
					{
						essence_->pointer_where.x = essence_->WhereUnknown;
						essence_->ptr_state = essence_->StateNormal;
						this->draw();
						nana::gui::API::lazy_refresh();
					}
				}
				void trigger::mouse_down(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					bool update = false;
					if(essence_->pointer_where.x == essence_->WhereHeader && (static_cast<int>(essence_->pointer_where.y) != essence_->WhereUnknown || (drawer_header_->item_spliter() != npos)))
					{
						essence_->ptr_state = essence_->StateGrab;
						nana::point pos(ei.mouse.x, ei.mouse.y);
						essence_->widget_to_header(pos);
						drawer_header_->grab(pos, true);
						nana::gui::API::capture_window(essence_->window->handle(), true);
						update = true;
					}
					else if(essence_->pointer_where.x == essence_->WhereLister || essence_->pointer_where.x == essence_->WhereChecker)
					{
						std::pair<size_type, size_type> item;
						if(essence_->lister.forward(essence_->scroll.offset_y.x, essence_->scroll.offset_y.y, essence_->pointer_where.y, item))
						{
							if(essence_->pointer_where.x == essence_->WhereLister)
							{
								essence_->lister.select_for_all(false);
								if(item.second != npos)
									essence_->lister.item_selected(item.first, item.second, true);
								else
									essence_->lister.categ_selected(item.first, true);
							}
							else
							{
								if(item.second != npos)
									essence_->lister.item_checked_reverse(item.first, item.second);
								else
									essence_->lister.categ_checked_reverse(item.first);
							}

							nana::rectangle r;
							update = essence_->rect_lister(r);
							if(update)	drawer_lister_->draw(r);
						}
					}

					if(update)
					{
						this->_m_draw_border();
						nana::gui::API::lazy_refresh();
					}
				}

				void trigger::mouse_up(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(essence_->ptr_state == essence_->StateGrab)
					{
						essence_->ptr_state = essence_->StateHighlight;
						nana::point pos(ei.mouse.x, ei.mouse.y);
						essence_->widget_to_header(pos);
						drawer_header_->grab(pos, false);
						this->draw();
						nana::gui::API::lazy_refresh();
						nana::gui::API::capture_window(essence_->window->handle(), false);
					}
				}

				void trigger::mouse_wheel(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(essence_->wheel(ei.wheel.upwards))
					{
						this->draw();
						essence_->adjust_scroll_value();
						nana::gui::API::lazy_refresh();
					}
				}

				void trigger::dbl_click(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					if(essence_->pointer_where.x == essence_->WhereLister)
					{
						std::pair<es_lister::size_type, es_lister::size_type> item;
						//Get the item which the mouse is placed.
						if(essence_->lister.forward(essence_->scroll.offset_y.x, essence_->scroll.offset_y.y, essence_->pointer_where.y, item))
						{
							if(item.second == npos)	//being the npos of item.second is a category
							{
								bool do_expand = (essence_->lister.expand(item.first) == false);
								essence_->lister.expand(item.first, do_expand);

								if(do_expand == false)
								{
									std::pair<size_type, size_type> last = essence_->lister.last();
									size_type n = essence_->number_of_lister_items(false);
									if(essence_->lister.backward(last.first, last.second, n, last))
									{
										essence_->scroll.offset_y.x = static_cast<nana::upoint::value_type>(last.first);
										essence_->scroll.offset_y.y = static_cast<nana::upoint::value_type>(last.second);
									}
								}
								essence_->adjust_scroll_life();


								this->draw();
								nana::gui::API::lazy_refresh();
							}
						}
					}
				}
				void trigger::resize(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					essence_->adjust_scroll_life();
					this->draw();
					nana::gui::API::lazy_refresh();
				}

				void trigger::key_down(trigger::graph_reference graph, const nana::gui::eventinfo& ei)
				{
					switch(ei.keyboard.key)
					{
					case nana::gui::keyboard::up:
					case nana::gui::keyboard::down:
						essence_->lister.move_select(ei.keyboard.key == nana::gui::keyboard::up);
						essence_->trace_selected_item();
						this->draw();
						nana::gui::API::lazy_refresh();
						break;
					}
				}
			//end class trigger
		}
	}//end namespace drawerbase

	//class listbox
		listbox::listbox(){}

		listbox::listbox(window wd, bool visible)
		{
			create(wd, rectangle(), visible);
		}

		listbox::listbox(window wd, const rectangle& r, bool visible)
		{
			create(wd, r, visible);
		}

		void listbox::auto_draw(bool ad)
		{
			get_drawer_trigger().essence().set_auto_draw(ad);
		}

		void listbox::append_categ(const nana::string& text)
		{
			get_drawer_trigger().essence().lister.create(text);
			get_drawer_trigger().update();
		}

		void listbox::append_header(const nana::string& text, unsigned width)
		{
			get_drawer_trigger().essence().header.create(text, width);
			get_drawer_trigger().update();
		}

		void listbox::append_item(const nana::string& text)
		{
			append_item(0, text);
		}

		void listbox::append_item(listbox::size_type categ, const nana::string& text)
		{
			drawerbase::listbox::essence_t & essence = this->get_drawer_trigger().essence();
			if(essence.lister.push_back(categ, text))
			{
				if(false == nana::gui::API::empty_window(this->handle()))
				{
					essence.lister.bkcolor(categ, essence.lister.size_item(categ) - 1, nana::gui::API::background(this->handle()));
					essence.lister.fgcolor(categ, essence.lister.size_item(categ) - 1, nana::gui::API::foreground(this->handle()));
					this->get_drawer_trigger().update();
				}
			}
		}

		void listbox::insert(size_type categ, size_type index, const nana::string& text)
		{
			drawerbase::listbox::essence_t & essence = this->get_drawer_trigger().essence();
			if(essence.lister.insert(categ, index, text))
			{
				if(false == nana::gui::API::empty_window(this->handle()))
				{
					essence.lister.bkcolor(categ, essence.lister.size_item(categ) - 1, nana::gui::API::background(this->handle()));
					essence.lister.fgcolor(categ, essence.lister.size_item(categ) - 1, nana::gui::API::foreground(this->handle()));
					this->get_drawer_trigger().update();
				}
			}
		}

		void listbox::checkable(bool chkable)
		{
			if(this->get_drawer_trigger().essence().checkable != chkable)
			{
				get_drawer_trigger().essence().checkable = chkable;
				get_drawer_trigger().update();
			}
		}

		bool listbox::checked(listbox::size_type item) const
		{
			return this->get_drawer_trigger().essence().lister.item_checked(0, item);
		}

		bool listbox::checked(listbox::size_type categ, listbox::size_type item) const
		{
			return this->get_drawer_trigger().essence().lister.item_checked(categ, item);
		}

		void listbox::checked(std::vector<std::pair<listbox::size_type, listbox::size_type> >& vec)
		{
			this->get_drawer_trigger().essence().lister.item_checked(vec);
		}

		void listbox::checked(size_type categ, size_type item, bool value)
		{
			if(this->get_drawer_trigger().essence().lister.item_checked(categ, item, value))
				this->get_drawer_trigger().update();
		}

		void listbox::clear(listbox::size_type categ)
		{
			nana::gui::drawerbase::listbox::essence_t & es = this->get_drawer_trigger().essence();
			if(es.lister.clear(categ))
			{
				nana::upoint pos = es.scroll_y();
				if(pos.x == categ)
				{
					pos.y = (pos.x > 0 ? es.WhereUnknown : 0);
					es.scroll_y(pos);
				}
			}
			this->get_drawer_trigger().update();
		}

		void listbox::clear_all()
		{
			nana::gui::drawerbase::listbox::essence_t & es = this->get_drawer_trigger().essence();
			es.lister.clear();
			nana::upoint pos = es.scroll_y();
			pos.y = (pos.x > 0 ? es.WhereUnknown : 0);
			es.scroll_y(pos);
			this->get_drawer_trigger().update();
		}

		void listbox::erase(listbox::size_type item)
		{
			this->erase(0, item);
		}

		void listbox::erase(listbox::size_type categ, listbox::size_type item)
		{
			nana::gui::drawerbase::listbox::essence_t & es = this->get_drawer_trigger().essence();
			es.lister.erase(categ, item);
			nana::upoint pos = es.scroll_y();
			if((pos.x == categ) && (item <= pos.y))
			{
				if(pos.y == 0)
				{
					if(es.lister.size_item(categ) == 0)
						pos.y = (pos.x > 0 ? es.WhereUnknown : 0);
				}
				else
					--pos.y;
				es.scroll_y(pos);
			}
			this->get_drawer_trigger().update();
		}

		void listbox::erase_categ(listbox::size_type categ)
		{
			nana::gui::drawerbase::listbox::essence_t & es = this->get_drawer_trigger().essence();
			es.lister.erase(categ);
			if(categ)
			{
				nana::upoint pos = es.scroll_y();
				if(categ <= pos.x)
				{
					if(pos.x == es.lister.size_categ())
						--pos.x;
					pos.y = es.WhereUnknown;
					es.scroll_y(pos);
				}
			}
			else
				es.scroll_y(nana::upoint(0, 0));
			this->get_drawer_trigger().update();
		}

		void listbox::erase_all()
		{
			nana::gui::drawerbase::listbox::essence_t & es = this->get_drawer_trigger().essence();
			es.lister.erase();
			es.scroll_y(nana::upoint(0, 0));
			this->get_drawer_trigger().update();
		}

		nana::string listbox::item_text(size_type categ, size_type index, size_type sub) const
		{
			return this->get_drawer_trigger().essence().lister.item_text(categ, index, sub);
		}

		void listbox::set_item_text(listbox::size_type index, listbox::size_type sub, const nana::string& text)
		{
			this->set_item_text(0, index, sub, text);
		}

		void listbox::set_item_text(listbox::size_type categ, listbox::size_type index, listbox::size_type sub, const nana::string& text)
		{
			drawerbase::listbox::essence_t & es = this->get_drawer_trigger().essence();
			es.lister.text(categ, index, sub, text, es.header.size());
			this->get_drawer_trigger().update();
		}

		void listbox::show_header(bool sh)
		{
			this->get_drawer_trigger().essence().header.visible(sh);
			this->get_drawer_trigger().update();
		}

		bool listbox::visible_header() const
		{
			return this->get_drawer_trigger().essence().header.visible();
		}

		bool listbox::selected(listbox::size_type item) const
		{
			return this->get_drawer_trigger().essence().lister.item_selected(0, item);
		}

		bool listbox::selected(listbox::size_type categ, listbox::size_type item) const
		{
			return this->get_drawer_trigger().essence().lister.item_selected(categ, item);
		}

		void listbox::selected(std::vector<std::pair<listbox::size_type, listbox::size_type> >& vec)
		{
			this->get_drawer_trigger().essence().lister.item_selected(vec);
		}

		void listbox::selected(size_type categ, size_type item, bool value)
		{
			if(this->get_drawer_trigger().essence().lister.item_selected(categ, item, value))
				this->get_drawer_trigger().update();
		}

		void listbox::move_select(bool upwards)
		{
			this->get_drawer_trigger().essence().lister.move_select(upwards);
			this->get_drawer_trigger().update();
		}

		void listbox::icon(size_type categ, size_type index, const nana::paint::image& img)
		{
			if(img)
			{
				get_drawer_trigger().essence().lister.icon(categ, index, img);
				get_drawer_trigger().essence().if_image = true;
				get_drawer_trigger().update();
			}
		}

		nana::paint::image listbox::icon(size_type categ, size_type index) const
		{
			return get_drawer_trigger().essence().lister.icon(categ, index);
		}

		void listbox::item_background(listbox::size_type categ, listbox::size_type index, nana::color_t color)
		{
			this->get_drawer_trigger().essence().lister.bkcolor(categ, index, color);
			this->get_drawer_trigger().update();
		}

		nana::color_t listbox::item_background(listbox::size_type categ, listbox::size_type index) const
		{
			return get_drawer_trigger().essence().lister.bkcolor(categ, index);
		}

		void listbox::item_foreground(listbox::size_type categ, listbox::size_type index, nana::color_t color)
		{
			this->get_drawer_trigger().essence().lister.fgcolor(categ, index, color);
			this->get_drawer_trigger().update();
		}

		nana::color_t listbox::item_foreground(listbox::size_type categ, listbox::size_type index) const
		{
			return get_drawer_trigger().essence().lister.fgcolor(categ, index);
		}

		listbox::size_type listbox::size_categ() const
		{
			return this->get_drawer_trigger().essence().lister.size_categ();
		}

		listbox::size_type listbox::size_item() const
		{
			return this->size_item(0);
		}

		listbox::size_type listbox::size_item(listbox::size_type categ) const
		{
			return this->get_drawer_trigger().essence().lister.size_item(categ);
		}

		void listbox::_m_anyobj(listbox::size_type categ, listbox::size_type index, const nana::any& anyobj)
		{
			this->get_drawer_trigger().essence().lister.object(categ, index, anyobj);
		}

		nana::any* listbox::_m_anyobj(listbox::size_type categ, listbox::size_type index)
		{
			return this->get_drawer_trigger().essence().lister.object(categ, index);
		}

		nana::any* listbox::_m_anyobj(listbox::size_type categ, listbox::size_type index) const
		{
			return this->get_drawer_trigger().essence().lister.object(categ, index);
		}
	//end class listbox
}//end namespace gui
}//end namespace nana
