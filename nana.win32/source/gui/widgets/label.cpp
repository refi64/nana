/*
 *	A Label Control Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: source/gui/widgets/label.cpp
 */

#include <nana/gui/widgets/label.hpp>
#include <stack>
#include <nana/gui/cursor.hpp>
#include <nana/system/platform.hpp>
#include <nana/unicode_bidi.hpp>

namespace nana
{
namespace gui
{
	namespace drawerbase
	{
		namespace label
		{
			class renderer_interface
			{
			public:
				typedef nana::gui::widget	&	widget_reference;
				typedef nana::paint::graphics&	graph_reference;

				virtual ~renderer_interface(){}

				virtual void render(widget_reference, graph_reference, const nana::string&) = 0;
				virtual unsigned extent_size(graph_reference, const nana::string& text) = 0;
				virtual nana::size measure(graph_reference, const nana::string&) = 0;
				virtual void over(int x, int y) = 0;
				virtual void leave() = 0;
				virtual void click() = 0;
			};

			class simple_renderer
				: public renderer_interface
			{
				void render(widget_reference wd, graph_reference graph, const nana::string& text)
				{
					nana::string::size_type length = text.length();
					if(0 == length)	return;

					nana::color_t fgcolor = wd.foreground();

					nana::size txt_size = graph.text_extent_size(STR("jH"));
					nana::string::size_type start = 0;
					nana::string::size_type pos = text.find(nana::char_t('\n'), start);

					nana::unicode_bidi bidi;
					std::vector<nana::unicode_bidi::entity> reordered;
					std::vector<unsigned> width_keeper;
					//[Feature:1]GUI.Label supports that transforms '\n' into new line autometically
					int y = 0;
					while(true)
					{
						if(pos > start)
						{
							nana::string str = text.substr(start, pos - start);
							unsigned str_w = 0;
							width_keeper.clear();
							reordered.clear();
							bidi.linestr(str.c_str(), str.size(), reordered);
							for(std::vector<nana::unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
							{
								unsigned w = graph.text_extent_size(i->begin, i->end - i->begin).width;
								width_keeper.push_back(w);
								str_w += w;
							}

							//Test whether the text needs the new line.
							if(str_w > graph.width())
							{
								int x = 0;
								std::vector<unsigned>::iterator i_width_keeper = width_keeper.begin();
								for(std::vector<nana::unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i, ++i_width_keeper)
								{
									bool beyond_edge = (x + *i_width_keeper > graph.width());
									if(beyond_edge)
									{
										size_t len = i->end - i->begin;
										if(len > 1)
										{
											unsigned * pxbuf = new unsigned[len];
											//Find the char that should be splitted
											graph.glyph_pixels(i->begin, len, pxbuf);
											size_t idx_head = 0, idx_splitted;

											do
											{
												unsigned wd_px = 0;
												idx_splitted = idx_head;
												for(; idx_splitted < len; ++idx_splitted)
												{
													if(x + wd_px + pxbuf[idx_splitted] > graph.width())
													{
														if(idx_splitted == idx_head)
															++idx_splitted;
														break;
													}
													wd_px += pxbuf[idx_splitted];
												}
												if(idx_splitted == len)
												{
													graph.string(x, y, fgcolor, i->begin + idx_head, idx_splitted - idx_head);
													x += static_cast<int>(wd_px);
													break;
												}
												//Check the word whether it is splittable.
												if(_m_splittable(i->begin, idx_splitted))
												{
													graph.string(x, y, fgcolor, i->begin + idx_head, idx_splitted - idx_head);
													idx_head = idx_splitted;
													x = 0;
													y += txt_size.height;
												}
												else
												{
													//Search the splittable character from idx_head to idx_splitted
													const nana::char_t * u = i->begin + idx_splitted;
													const nana::char_t * head = i->begin + idx_head;

													for(; head < u; --u)
													{
														if(_m_splittable(head, u - head))
															break;
													}

													if(u != head)
													{
														graph.string(x, y, fgcolor, head, u - head);
														idx_head += u - head;
														x = 0;
														y += txt_size.height;
													}
													else
													{
														u = i->begin + idx_splitted;
														const nana::char_t * end = i->begin + len;
														for(; u < end; ++u)
														{
															if(_m_splittable(head, u - head))
																break;
														}
														y += txt_size.height;
														size_t splen = u - head;
														graph.string(0, y, fgcolor, head, splen);
														x = 0;
														for(size_t k = idx_head; k < idx_head + splen; ++k)
															x += static_cast<int>(pxbuf[k]);
														if(x >= static_cast<int>(graph.width()))
														{
															x = 0;
															y += txt_size.height;
														}
														idx_head += splen;
													}
												}
											}while(idx_head < len);

											delete [] pxbuf;
										}
										else
										{
											graph.string(0, y += txt_size.height, fgcolor, i->begin, 1);
											x = static_cast<int>(*i_width_keeper);
										}
									}
									else
									{
										graph.string(x, y, fgcolor, i->begin, i->end - i->begin);
										x += static_cast<int>(*i_width_keeper);
									}
								}
							}
							else
								graph.bidi_string(0, y, fgcolor, str.c_str(), str.size());
						}

						if(nana::string::npos == pos)
							break;

						start = pos + 1;
						pos = text.find(nana::char_t('\n'), start );

						y += txt_size.height;
					}
				}

				unsigned extent_size(graph_reference graph, const nana::string& text)
				{
					nana::string::size_type length = text.length();
					if(0 == length)	return 0;

					unsigned txt_size = graph.text_extent_size(STR("jH")).height;
					nana::string::size_type start = 0;
					nana::string::size_type pos = text.find(nana::char_t('\n'), start);

					nana::unicode_bidi bidi;
					std::vector<nana::unicode_bidi::entity> reordered;
					std::vector<unsigned> width_keeper;
					//[Feature:1]GUI.Label supports that transforms '\n' into new line autometically
					int y = txt_size;
					while(true)
					{
						if(pos > start)
						{
							nana::string str = text.substr(start, pos - start);
							unsigned str_w = 0;
							width_keeper.clear();
							reordered.clear();
							bidi.linestr(str.c_str(), str.size(), reordered);
							for(std::vector<nana::unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i)
							{
								unsigned w = graph.text_extent_size(i->begin, i->end - i->begin).width;
								width_keeper.push_back(w);
								str_w += w;
							}

							//Test whether the text needs the new line.
							if(str_w > graph.width())
							{
								int x = 0;
								std::vector<unsigned>::iterator i_width_keeper = width_keeper.begin();
								for(std::vector<nana::unicode_bidi::entity>::iterator i = reordered.begin(); i != reordered.end(); ++i, ++i_width_keeper)
								{
									bool beyond_edge = (x + *i_width_keeper > graph.width());
									if(beyond_edge)
									{
										size_t len = i->end - i->begin;
										if(len > 1)
										{
											unsigned * pxbuf = new unsigned[len];
											//Find the char that should be splitted
											graph.glyph_pixels(i->begin, len, pxbuf);
											size_t idx_head = 0, idx_splitted;

											do
											{
												unsigned wd_px = 0;
												idx_splitted = idx_head;
												for(; idx_splitted < len; ++idx_splitted)
												{
													if(x + wd_px + pxbuf[idx_splitted] > graph.width())
													{
														if(idx_splitted == idx_head)
															++idx_splitted;
														break;
													}
													wd_px += pxbuf[idx_splitted];
												}
												if(idx_splitted == len)
												{
													x += static_cast<int>(wd_px);
													break;
												}
												//Check the word whether it is splittable.
												if(_m_splittable(i->begin, idx_splitted))
												{
													idx_head = idx_splitted;
													x = 0;
													y += txt_size;
												}
												else
												{
													//Search the splittable character from idx_head to idx_splitted
													const nana::char_t * u = i->begin + idx_splitted;
													const nana::char_t * head = i->begin + idx_head;

													for(; head < u; --u)
													{
														if(_m_splittable(head, u - head))
															break;
													}

													if(u != head)
													{
														idx_head += u - head;
														x = 0;
														y += txt_size;
													}
													else
													{
														u = i->begin + idx_splitted;
														const nana::char_t * end = i->begin + len;
														for(; u < end; ++u)
														{
															if(_m_splittable(head, u - head))
																break;
														}
														y += txt_size;

														x = 0;
														size_t splen = u - head;
														for(size_t k = idx_head; k < idx_head + splen; ++k)
															x += static_cast<int>(pxbuf[k]);
														if(x >= static_cast<int>(graph.width()))
														{
															x = 0;
															y += txt_size;
														}
														idx_head += splen;
													}
												}
											}while(idx_head < len);

											delete [] pxbuf;
										}
										else
											x = static_cast<int>(*i_width_keeper);
									}
									else
										x += static_cast<int>(*i_width_keeper);
								}
							}
						}

						if(nana::string::npos == pos)
							break;

						start = pos + 1;
						pos = text.find(nana::char_t('\n'), start );

						y += txt_size;
					}
					return y;
				}

				nana::size measure(graph_reference graph, const nana::string& text)
				{
					nana::size extsize;
					nana::string::size_type length = text.length();
					if(0 == length)	return extsize;

					nana::size txt_size = graph.text_extent_size(STR("jH"));
					nana::string::size_type start = 0;
					nana::string::size_type pos = text.find(nana::char_t('\n'), start);

					//[Feature:1]GUI.Label supports that transforms '\n' into new line autometically
					extsize.height = txt_size.height;
					while(true)
					{
						if(pos > start)
						{
							unsigned px = graph.text_extent_size(text.substr(start, pos - start)).width;
							if(extsize.width < px)
								extsize.width = px;
						}

						if(nana::string::npos == pos)
							break;

						start = pos + 1;
						pos = text.find(nana::char_t('\n'), start );
						extsize.height += txt_size.height;
					}
					return extsize;
				}

				void over(int x, int y)
				{}

				void leave()
				{}

				void click()
				{}

			private:
				static bool _m_splittable(const nana::char_t * str, size_t index)
				{
					nana::char_t ch = str[index];
					if(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
					{
						nana::char_t prch;
						if(index)
						{
							prch = str[index - 1];
							if('0' <= ch && ch <= '9')
								return !(('0' <= prch && prch <= '9') || (str[index - 1] == '-'));

							return (('z' < prch || prch < 'a') && ('Z' < prch || prch < 'A'));
						}
						else
							return false;
					}
					return true;
				}
			};

			class content
			{
				struct tokens
				{
					enum t{string, pure_string, number, beg, end, br, font, bold, size, color, url, equal, comma, backslash, _true, _false, endl, eof};
				};

				struct states
				{
					enum t{init, init_fmt, pstr, escape, eof};
				};

				typedef tokens::t token_t;

				struct section_t
				{
					bool br;
					nana::string font;
					unsigned size;
					bool bold;
					nana::color_t color;
					nana::string url;
					nana::string str;

					std::vector<nana::rectangle> areas;
				};

				struct font_t
				{
					nana::string font;
					unsigned size;
					bool bold;
					nana::color_t color;
				};

				std::vector<std::vector<section_t*> > container_;
			public:
				typedef std::vector<section_t*> line_container;

				~content()
				{
					_m_clear();
				}

				const line_container* getline(unsigned x) const
				{
					if(x < container_.size())
					{
						return &container_[x];
					}
					return 0;
				}

				void parse(const nana::string& txt)
				{
					data_.init(txt);
					_m_clear();
					_m_parse();
				}
			private:
				void _m_push(section_t& s)
				{
					font_t f;
					f.font = s.font;
					f.bold = s.bold;
					f.color = s.color;
					f.size = s.size;
					stack_.push(f);
				}

				void _m_pop(section_t& s)
				{
					if(stack_.size())
						stack_.pop();

					s.str.clear();
					if(stack_.size() == 0)
					{
						s.font.clear();
						s.size = 0xFFFFFFFF;
						s.bold = false;
						s.color = 0xFF000000;
					}
					else
					{
						font_t f = stack_.top();
						s.font = f.font;
						s.size = f.size;
						s.bold = f.bold;
						s.color = f.color;
					}
				}

				void _m_init_section(section_t& s)
				{
					s.br = false;
					s.str.clear();
					if(stack_.size() == 0)
					{
						s.font.clear();
						s.size = 0xFFFFFFFF;
						s.bold = false;
						s.color = 0xFF000000;
					}
					else
					{
						font_t f = stack_.top();
						s.font = f.font;
						s.size = f.size;
						s.bold = f.bold;
						s.color = f.color;
					}
				}

				void _m_append(const section_t& sec)
				{
					if(container_.size() == 0)
						container_.push_back(std::vector<section_t*>());

					(container_.end() - 1)->push_back(new section_t(sec));
				}

				void _m_endl()
				{
					container_.push_back(std::vector<section_t*>());
				}

				void _m_clear()
				{
					for(std::vector<std::vector<section_t*> >::iterator i = container_.begin(); i != container_.end(); ++i)
					{
						line_container & lc = *i;
						for(std::vector<section_t*>::iterator u = lc.begin(); u != lc.end(); ++u)
						{
							delete *u;
						}
					}

					container_.clear();
				}
			private:
				void _m_parse()
				{
					section_t section;
					_m_init_section(section);

					while(true)
					{
						token_t tk = _m_token();
						switch(tk)
						{
						case tokens::string:
							section.str += data_.tokenstr;
							_m_append(section);
							section.str.clear();
							break;
						case tokens::endl:
							_m_endl();
							break;
						case tokens::beg:
							tk = _m_token();
							if(tk == tokens::br)
							{
								_m_endl();
								while(tokens::end != _m_token());
							}
							else
							{
								_m_push(section);
								_m_enter_format(tk);
								_m_pop(section);
							}
							break;
						case tokens::eof:
							return;
						}
					}
				}

				int _m_tokenstr_to_number()
				{
					std::stringstream ss;

					std::string str;
					nana::stringset_cast(str, data_.tokenstr);
					ss<<str;
					if(data_.tokenstr.size() > 2)
					{
						if(data_.tokenstr[0] == '0' && (data_.tokenstr[1] == 'x' || data_.tokenstr[1] == 'X'))
							ss>>std::hex;
					}

					int n;
					ss>>n;

					return n;
				}

				bool _m_enter_format(token_t tk)
				{
					section_t sect;
					_m_init_section(sect);

					if(tk == tokens::backslash)
					{
						tk = _m_token();
						if(tk == tokens::end)
							return false;
					}
					while(tk != tokens::end && tk != tokens::eof)
					{
						switch(tk)
						{
						case tokens::font:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								if(tk == tokens::pure_string)
									sect.font = data_.tokenstr;
							}
							break;
						case tokens::url:
							tk = _m_token();
							if(tokens::equal == tk)
							{
								tk = _m_token();
								if(tk == tokens::pure_string)
									sect.url = data_.tokenstr;
							}
							break;
						case tokens::br:
							tk = _m_token();
							sect.br = true;
							break;
						case tokens::bold:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								switch(tk)
								{
								case tokens::_true:
									sect.bold = true;
									break;
								case tokens::_false:
									sect.bold = false;
									break;
								}
							}
							else
								sect.bold = true;
							break;
						case tokens::size:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								if(tk == tokens::number)
								{
									sect.size = _m_tokenstr_to_number();
								}
							}
							break;
						case tokens::color:
							tk = _m_token();
							if(tk == tokens::equal)
							{
								tk = _m_token();
								if(tk == tokens::number)
								{
									sect.color = _m_tokenstr_to_number();
								}
							}
							break;
						default:
							tk = _m_token();
						}
					}

					tk = _m_token();
					bool loop = true;
					while(loop)
					{
						switch(tk)
						{
						case tokens::string:
							sect.str = data_.tokenstr;
							_m_append(sect);
							sect.str.clear();
							tk = _m_token();
							break;
						case tokens::endl:
							_m_endl();
							tk = _m_token();
							break;
						case tokens::beg:
							tk = _m_token();
							if(tk == tokens::br)
							{
								while(tokens::end != _m_token());
								tk = _m_token();
								_m_endl();
							}
							else
							{
								_m_push(sect);
								if(false == _m_enter_format(tk))
								{
									_m_pop(sect);
									tk = _m_token();
								}
								else
									loop = false;
							}
							break;
						case tokens::eof:
							loop = false;
							break;
						}
					}
					return true;
				}

				token_t _m_token()
				{
					if(data_.pos >= data_.str.size()) return tokens::eof;

					nana::char_t ch = data_.str.c_str()[data_.pos];

					data_.tokenstr.clear();
					states::t state = (data_.init_fmt ? states::init_fmt : states::init);

					while(true)
					{
						switch(state)
						{
						case states::init:
							switch(ch)
							{
							case '\n':
								++data_.pos;
								return tokens::endl;
							case '/':
								++data_.pos;
								return tokens::backslash;
							case '\\':
								state = states::escape;
								break;
							case '<':
								++data_.pos;
								++data_.init_fmt;
								return tokens::beg;
							default:
								while(ch != '\n' && ch != '\\' && ch != '<' && ch != 0)
								{
									data_.tokenstr += ch;
									ch = data_.str.c_str()[++data_.pos];
								}
								return tokens::string;
							}
							break;
						case states::escape:
							switch(ch)
							{
							case '\\':
							case '<':
								++data_.pos;
								data_.tokenstr += ch;
								return tokens::string;
							}
							data_.tokenstr += '\\';
							data_.tokenstr += ch;
							return tokens::string;
						case states::eof:
							return tokens::eof;
						case states::init_fmt:
							while(ch == ' ' || ch == 0) ch = data_.str.c_str()[++data_.pos];

							if(ch == ',')
							{
								++data_.pos;
								return tokens::comma;
							}
							else if(ch == '=')
							{
								++data_.pos;
								return tokens::equal;
							}
							else if(ch == '>')
							{
								if(data_.init_fmt)
									--data_.init_fmt;
								++data_.pos;
								return tokens::end;
							}
							else if(ch == '/')
							{
								++data_.pos;
								return tokens::backslash;
							}
							else if(ch == '"')
							{
								state = states::pstr;
								break;
							}
							else if('0' <= ch && ch <= '9')
							{
								data_.tokenstr += ch;
								if('0' == ch)
								{
									ch = data_.str.c_str()[++data_.pos];
									if((!('0' <= ch && ch <= '9')) && (ch != 'x' && ch != 'X'))
									{
										return tokens::number;
									}
									else if(ch == 'x' || ch == 'X')
									{
										data_.tokenstr += 'x';
										ch = data_.str.c_str()[++data_.pos];
										while(('0' <= ch && ch <= '9') || ('a' <= ch && ch <= 'f') || ('A' <= ch && ch <= 'F'))
										{
											data_.tokenstr += ch;
											ch = data_.str.c_str()[++data_.pos];
										}
										return tokens::number;
									}
								}

								ch = data_.str.c_str()[++data_.pos];
								while('0' <= ch && ch <= '9')
								{
									data_.tokenstr += ch;
									ch = data_.str.c_str()[++data_.pos];
								}

								return tokens::number;
							}
							else
							{
								while(true)
								{
									token_t rtk = tokens::string;
									switch(ch)
									{
									case 0:
									case ',':
									case '=':
									case '>':
									case '"':
									case ' ':
										if(STR("br") == data_.tokenstr)
											return tokens::br;
										else if(STR("font") == data_.tokenstr)
											return tokens::font;
										else if(STR("bold") == data_.tokenstr)
											return tokens::bold;
										else if(STR("size") == data_.tokenstr)
											return tokens::size;
										else if(STR("color") == data_.tokenstr)
											return tokens::color;
										else if(STR("true") == data_.tokenstr)
											return tokens::_true;
										else if(STR("url") == data_.tokenstr)
											return tokens::url;
										else if(STR("false") == data_.tokenstr)
											return tokens::_false;
										else
											return tokens::string;
									}

									if(rtk == tokens::string)
									{
										data_.tokenstr += ch;
										ch = data_.str.c_str()[++data_.pos];
									}
									else
										break;
								}
							}
							break;
						case states::pstr:
							while(ch != '"' && ch != 0)
							{
								data_.tokenstr += ch;
								ch = data_.str.c_str()[++data_.pos];
							}
							if(ch != 0)
								++data_.pos;
							return tokens::pure_string;
						}

						ch = data_.str.c_str()[++data_.pos];
					}

					return tokens::eof;
				}


			private:
				std::stack<font_t> stack_;

				struct data_tag
				{
					nana::string tokenstr;
					nana::string str;
					nana::string::size_type pos;
					unsigned init_fmt;


					data_tag()
						: pos(0), init_fmt(0)
					{}

					void init(const nana::string& txt)
					{
						str = txt;
						pos = 0;
						tokenstr.clear();
						init_fmt = 0;
					}
				}data_;

			};

			class format_renderer
				:public renderer_interface
			{
				void render(widget_reference wd, graph_reference graph, const nana::string& str)
				{
					_m_erase_areas();
					content_.parse(str);
					trace_.cursor.bind(wd);

					color_sect_ = color_fg_ = wd.foreground();
					nana::paint::font font;
					nana::point pos;
					unsigned bounds = graph.height();
					const content::line_container * line = 0;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						unsigned px = _m_line_pixels(graph, line);
						for(content::line_container::const_iterator u = line->begin(); u != line->end(); ++u)
						{
							if(pos.y > static_cast<int>(bounds)) return;
							_m_change_font(graph, font, *u);
							pos = _m_draw_string(px, pos.x, pos.y, graph, (*u)->str, (*u));
						}
						pos.y += px;
						pos.x = 0;
					}
				}

				virtual unsigned extent_size(graph_reference graph, const nana::string& str)
				{
					content_.parse(str);
					nana::paint::font font;
					nana::point pos;

					const content::line_container * line = 0;
					unsigned extsize = 0;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						unsigned px = _m_line_pixels(graph, line);
						for(content::line_container::const_iterator u = line->begin(); u != line->end(); ++u)
						{
							_m_change_font(graph, font, *u);
							pos = _m_extent_size(px, pos.x, pos.y, graph, (*u)->str, (*u));
						}
						pos.y += px;
						pos.x = 0;
					}
					return static_cast<unsigned>(pos.y);				
				}

				virtual nana::size measure(graph_reference graph, const nana::string& str)
				{
					content_.parse(str);
					nana::paint::font font;
					nana::point pos;

					const content::line_container * line = 0;
					nana::size ts;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						unsigned px = _m_line_pixels(graph, line);
						unsigned width = 0;
						for(content::line_container::const_iterator u = line->begin(); u != line->end(); ++u)
						{
							_m_change_font(graph, font, *u);
							width += graph.text_extent_size((*u)->str).width;
						}
						if(ts.width < width)
							ts.width = width;
						ts.height += px;
					}
					return ts;
				}

				void over(int x, int y)
				{
					const content::line_container * line = 0;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						for(content::line_container::const_iterator u = line->begin(); u != line->end(); ++u)
						{
							std::vector<nana::rectangle>::iterator i = (*u)->areas.begin(), end = (*u)->areas.end();
							for(; i != end; ++i)
							{
								//Test if the specified point is in the area specified by rectangle.
								if((i->x <= x && x < i->x + static_cast<int>(i->width)) && (i->y <= y && y < i->y + static_cast<int>(i->height)))
								{
									trace_.cursor.load(cursor::predef::hand);
									trace_.url = (*u)->url;
									return;
								}
							}
						}
					}

					if(trace_.cursor.get() != cursor::predef::arrow)
					{
						trace_.cursor.load(cursor::predef::arrow);
						trace_.url.clear();
					}
				}

				void leave()
				{
					if(trace_.cursor.get() != cursor::predef::arrow)
					{
						trace_.cursor.load(cursor::predef::arrow);
						trace_.url.clear();
					}
				}

				void click()
				{
					if(trace_.url.size())
					{
						system::open_url(trace_.url);
					}
				}

				unsigned _m_line_pixels(graph_reference graph, const content::line_container * line) const
				{
					nana::paint::font font;
					unsigned pixels = 0;
					for(content::line_container::const_iterator u = line->begin(); u != line->end(); ++u)
					{
						_m_change_font(graph, font, *u);
						unsigned px = graph.text_extent_size((*u)->str).height;
						if(px > pixels)
							pixels = px;
					}
					return (pixels ? pixels : 10);
				}

				void _m_change_font(nana::paint::graphics& graph, nana::paint::font& font, const content::line_container::value_type s) const
				{
					if((s->font.size() && s->font != font.name()) || (s->size != 0xFFFFFF && s->size != font.size()) || (s->bold != font.bold()))
					{
						nana::string name = (s->font.size() ? s->font : font.name());
						unsigned size = (s->size != 0xFFFFFFFF ? s->size : font.size());
						nana::paint::font f(name.c_str(), size, s->bold);
						graph.typeface(f);
					}
				}

				nana::point _m_extent_size(unsigned line_pixels, int x, int y, graph_reference graph, const nana::string& str, const content::line_container::value_type s)
				{
					const unsigned text_area = graph.width();

					unsigned dw = text_area - x;
					int off = 0;
					int draw_y_pos;
					nana::rectangle r;
					while(true)
					{
						nana::size ts = graph.text_extent_size(str.c_str() + off, str.size() - off);
						if(ts.width <= dw)
							return nana::point(x + ts.width, y);

						unsigned pixels = 0;
						int len = 1;
						draw_y_pos = y;
						for(; off + len <= static_cast<int>(s->str.size()); ++len)
						{
							ts = graph.text_extent_size(str.c_str() + off, len);
							if(ts.width < dw)
							{
								pixels = ts.width;
							}
							else
							{
								if(ts.width > dw)
								{
									//a line must draw at least one character even through there is not a room for one character
									if(len > 1 || text_area > ts.width)
										--len;

									y += line_pixels;
									dw = text_area;
								}
								else
								{
									pixels = ts.width;
									dw -= ts.width;
								}
								break;
							}
						}

						//The text is splitted for a new line
						if(y != draw_y_pos)
						{
							x = 0;
							draw_y_pos = y;
						}
						else
							x += pixels;

						off += len;
						if(off >= static_cast<int>(str.size())) break;
					}
					return nana::point(x, y);			
				}

				nana::point _m_draw_string(unsigned line_pixels, int x, int y, graph_reference graph, const nana::string& str,  const content::line_container::value_type s)
				{
					const unsigned text_area = graph.width();
					nana::color_t clr = s->color == 0xFF000000 ? color_fg_ : s->color;

					unsigned dw = text_area - x;
					int off = 0;
					int draw_y_pos;
					nana::rectangle r;
					while(true)
					{
						nana::size ts = graph.text_extent_size(str.c_str() + off, str.size() - off);
						if(ts.width <= dw)
						{
							graph.string(x, y + (line_pixels - ts.height) / 2, clr, str.c_str() + off, str.size() - off);
							if(s->url.size())
							{
								r.x = x;
								r.y = y;
								r.width = ts.width;
								r.height = ts.height;
								s->areas.push_back(r);
							}
							return nana::point(x + ts.width, y);
						}

						unsigned pixels = 0;
						int len = 1;
						draw_y_pos = y;
						for(; off + len <= static_cast<int>(s->str.size()); ++len)
						{
							ts = graph.text_extent_size(str.c_str() + off, len);
							if(ts.width < dw)
							{
								pixels = ts.width;
							}
							else
							{
								if(ts.width > dw)
								{
									//a line must draw at least one character even through there is not a room for one character
									if(len > 1 || text_area > ts.width)
										--len;

									y += line_pixels;
									dw = graph.width();
								}
								else
								{
									pixels = ts.width;
									dw -= ts.width;
								}
								break;
							}
						}
						if(len)
						{
							if(s->url.size())
							{
								r.x = x;
								r.y = draw_y_pos;
								r.width = ts.width;
								r.height = ts.height;
								s->areas.push_back(r);
							}
							graph.string(x, draw_y_pos + (line_pixels - ts.height) / 2, clr, str.c_str() + off, len);
						}

						//The text is splitted for a new line
						if(y != draw_y_pos)
						{
							x = 0;
							draw_y_pos = y;
						}
						else
							x += pixels;

						off += len;
						if(off >= static_cast<int>(str.size())) break;
					}
					return nana::point(x, y);
				}

				void _m_erase_areas()
				{
					const content::line_container * line = 0;
					for(unsigned i = 0; (line = content_.getline(i)); ++i)
					{
						for(content::line_container::const_iterator u = line->begin(); u != line->end(); ++u)
						{
							(*u)->areas.clear();
						}
					}
				}
			private:
				struct trace_tag
				{
					nana::gui::cursor cursor;
					nana::string url;
				}trace_;

				content content_;
				nana::color_t color_sect_;
				nana::color_t color_fg_;
			};
			
			//class trigger
			//@brief: Draw the button
				struct trigger::impl_t
				{
					nana::gui::widget * wd;
					nana::paint::graphics * graph;
					renderer_interface * renderer;

					impl_t()
						: wd(0), graph(0), format_state_(false)
					{
						renderer = new simple_renderer;
					}

					~impl_t()
					{
						delete renderer;
					}

					void format(bool enabled)
					{
						if((enabled != format_state_) && wd)
						{
							nana::gui::internal_scope_guard isg;
							renderer_interface * rnd_if = renderer;
							if(enabled)
								renderer = new format_renderer;
							else
								renderer = new simple_renderer;
							format_state_ = enabled;
							API::refresh_window(wd->handle());
							delete rnd_if;
						}
					}
				private:
					bool format_state_;
				};

				trigger::trigger()
					:impl_(new impl_t)
				{
				}

				trigger::~trigger()
				{
					delete impl_;
				}

				void trigger::bind_window(nana::gui::widget& w)
				{
					impl_->wd = &w;
				}

				trigger::impl_t * trigger::impl() const
				{
					return impl_;
				}

				void trigger::attached(trigger::graph_reference graph)
				{
					impl_->graph = &graph;
					window wd = impl_->wd->handle();
					API::dev::make_drawer_event<events::mouse_move>(wd);
					API::dev::make_drawer_event<events::mouse_leave>(wd);
					API::dev::make_drawer_event<events::click>(wd);
				}

				void trigger::detached()
				{
					API::dev::umake_drawer_event(impl_->wd->handle());
				}

				void trigger::mouse_move(graph_reference, const eventinfo& ei)
				{
					impl_->renderer->over(ei.mouse.x, ei.mouse.y);
				}

				void trigger::mouse_leave(graph_reference, const eventinfo& ei)
				{
					impl_->renderer->leave();
				}

				void trigger::click(graph_reference, const eventinfo&)
				{
					impl_->renderer->click();
				}

				void trigger::notify_background_change(nana::paint::graphics& graph)
				{
					refresh(graph);
				}

				void trigger::refresh(paint::graphics& graph)
				{
					if(0 == impl_->wd) return;

					nana::gui::window wd = impl_->wd->handle();
					if(API::glass_window(wd))
						API::make_glass_background(wd);
					else
						graph.rectangle(0, 0, graph.width(), graph.height(), API::background(wd), true);

					impl_->renderer->render(*impl_->wd, graph, API::window_caption(wd));
				}

			//end class label_drawer
		}//end namespace label
	}//end namespace drawerbase


	//
	//class label
		label::label(){}

		label::label(window wd)
		{
			create(wd, 0, 0, 0, 0);
		}

		label::label(window wd, const nana::rectangle& r)
		{
			create(wd, r.x, r.y, r.width, r.height);
		}

		label::label(window wd, int x, int y, unsigned w, unsigned h)
		{
			create(wd, x, y, w, h);
		}

		void label::transparent(bool value)
		{
			nana::gui::internal_scope_guard isg;
			if(API::glass_window(this->handle(), value))
				API::refresh_window(this->handle());
		}

		bool label::transparent() const
		{
			return API::glass_window(this->handle());
		}

		void label::format(bool f)
		{
			this->get_drawer_trigger().impl()->format(f);
		}

		nana::size label::measure() const
		{
			if(this->empty())
				return nana::size();

			return this->get_drawer_trigger().impl()->renderer->measure(*(this->get_drawer_trigger().impl()->graph), API::window_caption(handle()));
		}

		unsigned label::extent_size() const
		{
			if(this->empty())
				return 0;

			return this->get_drawer_trigger().impl()->renderer->extent_size(*(this->get_drawer_trigger().impl()->graph), API::window_caption(handle()));
		}
	//end class label
}//end namespace gui
}//end namespace nana
