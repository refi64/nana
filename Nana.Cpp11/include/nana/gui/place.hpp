
#include <map>
#include <vector>
#include <utility>

#include <nana/gui/widgets/widget.hpp>

namespace nana
{
namespace gui
{
	class place
	{
		typedef std::pair<window, unsigned>	fixed_t;
		typedef std::pair<window, int>	percent_t;
		typedef std::pair<window, std::pair<unsigned, unsigned> > room_t;

		class number_t
		{
		public:
			enum class kind{integer, real, percent};

			number_t()
				: kind_(kind::integer)
			{
				value_.integer = 0;
			}

			kind kind_of() const
			{
				return kind_;
			}

			int integer() const
			{
				if(kind::integer == kind_)
					return value_.integer;
				return static_cast<int>(value_.real);
			}

			double real() const
			{
				if(kind::integer == kind_)
					return value_.integer;
				return value_.real;
			}

			void assign(int i)
			{
				kind_ = kind::integer;
				value_.integer = i;
			}

			void assign(double d)
			{
				kind_ = kind::real;
				value_.real = d;
			}

			void assign_percent(double d)
			{
				kind_ = kind::percent;
				value_.real = d;
			}
		private:
			kind kind_;
			union valueset
			{
				int integer;
				double real;
			}value_;
		};

		class field_t
		{
		public:
			struct element_t
			{
				enum class kind
				{
					window, gap, fixed, percent, room
				};

				kind kind_of_element;
				union
				{
					window handle;
					unsigned gap_value;
					fixed_t *	fixed_ptr;
					percent_t * percent_ptr;
					room_t *	room_ptr;
				}u;

				element_t(window wd)
					: kind_of_element(kind::window)
				{
					u.handle = wd;
				}

				element_t(unsigned gap)
					: kind_of_element(kind::gap)
				{
					u.gap_value = gap;
				}

				element_t(const fixed_t& fixed)
					: kind_of_element(kind::fixed)
				{
					u.fixed_ptr = new fixed_t(fixed);
				}

				element_t(const percent_t& per)
					: kind_of_element(kind::percent)
				{
					u.percent_ptr = new percent_t(per);
				}

				element_t(const room_t& rm)
					: kind_of_element(kind::room)
				{
					u.room_ptr = new room_t(rm);
				}

				element_t(const element_t& rhs)
					: kind_of_element(rhs.kind_of_element)
				{
					switch(kind_of_element)
					{
					case kind::fixed:
						u.fixed_ptr = new fixed_t(*rhs.u.fixed_ptr);
						break;
					case kind::percent:
						u.percent_ptr = new percent_t(*rhs.u.percent_ptr);
						break;
					case kind::room:
						u.room_ptr = new room_t(*rhs.u.room_ptr);
						break;
					default:
						u = rhs.u;
						break;
					}
				}

				element_t(element_t && rv)
					: kind_of_element(rv.kind_of_element), u(rv.u)
				{
					switch(kind_of_element)
					{
					case kind::fixed:
						rv.u.fixed_ptr = nullptr;
						break;
					case kind::percent:
						rv.u.percent_ptr = nullptr;
						break;
					case kind::room:
						rv.u.room_ptr = nullptr;
					default:	break;
					}
				}
				
				~element_t()
				{
					switch(kind_of_element)
					{
					case kind::fixed:
						delete u.fixed_ptr;
						break;
					case kind::percent:
						delete u.percent_ptr;
						break;
					case kind::room:
						delete u.room_ptr;
						break;
					default:	break;
					}
				}
			};
		public:
			typedef std::vector<element_t>::const_iterator const_iterator;

			field_t()
				: attached(false)
			{}

			field_t& operator<<(window wd)
			{
				seq_.emplace_back(wd);
				return *this;
			}

			field_t& operator<<(unsigned gap)
			{
				seq_.emplace_back(gap);
				return *this;
			}

			field_t& operator<<(const fixed_t& f)
			{
				seq_.emplace_back(f);
				return *this;
			}

			field_t& operator<<(const percent_t& p)
			{
				seq_.emplace_back(p);
				return *this;
			}

			field_t& operator<<(const room_t& r)
			{
				room_t x = r;
				if(x.second.first == 0)
					x.second.first = 1;
				if(x.second.second == 0)
					x.second.second = 1;
				seq_.emplace_back(x);
				return *this;
			}

			//returns the number of fixed pixels and the number of adjustable items
			std::pair<unsigned, std::size_t> fixed_and_adjustable() const 
			{
				std::pair<unsigned, std::size_t> vpair;
				for(auto & e : seq_)
				{
					switch(e.kind_of_element)
					{
					case element_t::kind::fixed:
						vpair.first += e.u.fixed_ptr->second;
						break;
					case element_t::kind::gap:
						vpair.first += e.u.gap_value;
						break;
					case element_t::kind::percent:	//the percent is not fixed and not adjustable.
						break;
					default:
						++vpair.second;
					}
				}
				return vpair;
			}

			unsigned percent_pixels(unsigned pixels) const
			{
				unsigned perpx = 0;
				for(auto & e : seq_)
				{
					if(element_t::kind::percent == e.kind_of_element)
						perpx += pixels * e.u.percent_ptr->second / 100;
				}
				return perpx;
			}

			const std::vector<element_t>& container() const
			{
				return seq_;
			}
		public:
			bool attached;
		private:
			std::vector<element_t> seq_;
		};


		class division
		{
		public:
			enum class kind{arrange, vertical_arrange, grid};

			division(kind k, std::string&& n)
				: kind_of_division(k), name(std::move(n)), field(nullptr)
			{}

			virtual ~division()
			{
				for(auto p : children)
				{
					delete p;
				}
			}

			bool is_fixed() const
			{
				return ((weight.kind_of() == number_t::kind::integer) && (weight.integer() != 0));
			}

			bool is_percent() const
			{
				return ((weight.kind_of() == number_t::kind::percent) && (weight.real() != 0));
			}

			//return the fixed pixels and adjustable items.
			std::pair<unsigned, std::size_t> fixed_pixels(kind match_kind) const
			{
				//this is a weight fixed division
				if(is_fixed())
					return std::pair<unsigned, std::size_t>(weight.integer(), 1);

				std::pair<unsigned, std::size_t> pair;
				if(field && (kind_of_division == match_kind))
					pair = field->fixed_and_adjustable();
				
				for(auto child : children)
				{
					if(false == child->is_fixed()) //it is adjustable
					{
						if(false == child->is_percent())
							++pair.second;
					}
					else
						pair.first += static_cast<unsigned>(child->weight.integer());
				}
				return pair;
			}

			virtual void collocate() = 0;

		public:
			kind kind_of_division;
			const std::string name;
			std::vector<division*> children;
			nana::rectangle area;
			number_t weight;
			field_t * field;
		};

		class div_arrange
			: public division
		{
		public:
			div_arrange(std::string&& name)
				: division(kind::arrange, std::move(name))
			{}

			virtual void collocate()
			{
				auto pair = fixed_pixels(kind::arrange);
				if(field)
					pair.first += field->percent_pixels(area.width);
				
				unsigned adjustable_pixels = (pair.second && pair.first < area.width ? ((area.width - pair.first) / pair.second) : 0);
				
				int left = area.x;
				for(auto child : children)
				{
					child->area.x = left;
					child->area.y = area.y;
					child->area.height = area.height;

					unsigned adj_px = static_cast<unsigned>(child->weight.kind_of() == number_t::kind::integer ? child->weight.integer() : 0);
					if(0 == adj_px) //the child is adjustable
					{
						if(false == child->is_percent())
						{
							adj_px = child->fixed_pixels(kind::arrange).first;
							if(adj_px <= adjustable_pixels)
								adj_px = adjustable_pixels;
						}
						else
							adj_px = static_cast<unsigned>(area.width * child->weight.real() / 100);
					}
					left += static_cast<int>(adj_px);
					child->area.width = adj_px;
					child->collocate();
				}

				if(field)
				{
					nana::rectangle r = area;
					for(auto & el : field->container())
					{
						r.x = left;
						switch(el.kind_of_element)
						{
						case field_t::element_t::kind::fixed:
							r.width = el.u.fixed_ptr->second;
							API::move_window(el.u.fixed_ptr->first, left, r.y, r.width, r.height);
							left += static_cast<int>(r.width);
							break;
						case field_t::element_t::kind::gap:
							left += static_cast<int>(el.u.gap_value);
							break;
						case field_t::element_t::kind::percent:
							r.width = area.width * el.u.percent_ptr->second / 100;
							API::move_window(el.u.percent_ptr->first, left, r.y, r.width, r.height);
							left += static_cast<int>(r.width);
							break;
						case field_t::element_t::kind::window:
							API::move_window(el.u.handle, left, r.y, adjustable_pixels, r.height);
							left += static_cast<int>(adjustable_pixels);
							break;
						case field_t::element_t::kind::room:
							API::move_window(el.u.room_ptr->first, left, r.y, adjustable_pixels, r.height);
							left += static_cast<int>(adjustable_pixels);
							break;
						}
					}
				}
			}
		};

		class div_vertical_arrange
			: public division
		{
		public:
			div_vertical_arrange(std::string&& name)
				: division(kind::vertical_arrange, std::move(name))
			{}

			virtual void collocate()
			{
				auto pair = fixed_pixels(kind::vertical_arrange);
				if(field)
					pair.first += field->percent_pixels(area.height);
				
				unsigned adjustable_pixels = (pair.second && pair.first < area.height ? ((area.height - pair.first) / pair.second) : 0);
				
				int top = area.y;
				for(auto child : children)
				{
					child->area.x = area.x;
					child->area.y = top;
					child->area.width = area.width;

					unsigned adj_px = static_cast<unsigned>(child->weight.kind_of() == number_t::kind::integer ? child->weight.integer() : 0);
					if(0 == adj_px) //the child is adjustable
					{
						if(false == child->is_percent())
						{
							adj_px = child->fixed_pixels(kind::vertical_arrange).first;
							if(adj_px <= adjustable_pixels)
								adj_px = adjustable_pixels;
						}
						else
							adj_px = static_cast<unsigned>(area.height * child->weight.real() / 100);
					}
					top += static_cast<int>(adj_px);
					child->area.height = adj_px;
					child->collocate();
				}

				if(field)
				{
					nana::rectangle r = area;
					for(auto & el : field->container())
					{
						r.y = top;
						switch(el.kind_of_element)
						{
						case field_t::element_t::kind::fixed:
							r.height = el.u.fixed_ptr->second;
							API::move_window(el.u.fixed_ptr->first, r.x, top, r.width, r.height);
							top += static_cast<int>(r.height);
							break;
						case field_t::element_t::kind::gap:
							top += static_cast<int>(el.u.gap_value);
							break;
						case field_t::element_t::kind::percent:
							r.height = area.height * el.u.percent_ptr->second / 100;
							API::move_window(el.u.percent_ptr->first, r.x, top, r.width, r.height);
							top += static_cast<int>(r.height);
							break;
						case field_t::element_t::kind::window:
							API::move_window(el.u.handle, r.x, top, r.width, adjustable_pixels);
							top += static_cast<int>(adjustable_pixels);
							break;
						case field_t::element_t::kind::room:
							API::move_window(el.u.room_ptr->first, r.x, top, r.width, adjustable_pixels);
							top += static_cast<int>(adjustable_pixels);
							break;
						}
					}
				}
			}
		};

		class div_grid
			: public division
		{
		public:
			div_grid(std::string&& name)
				: division(kind::grid, std::move(name))
			{
				dimension.first = dimension.second = 0;
			}

			virtual void collocate()
			{
				if(dimension.first <= 1 && dimension.second <= 1)
				{
					std::size_t n_of_wd = _m_number_of_window();

					std::size_t edge;
					switch(n_of_wd)
					{
					case 0:
					case 1:
						edge = 1;	break;
					case 2: case 3: case 4:
						edge = 2;	break;
					default:
						edge = static_cast<std::size_t>(std::sqrt(n_of_wd));
						if((edge * edge) < n_of_wd) ++edge;
					}

					auto i = field->container().begin(), end = field->container().end();

					bool exit_for = false;
					int y = area.y;
					unsigned block_w = area.width / edge;
					unsigned block_h = area.height / ((n_of_wd / edge) + (n_of_wd % edge ? 1 : 0));

					for(std::size_t u = 0; u < edge; ++u)
					{
						int x = area.x;
						for(std::size_t v = 0; v < edge; ++v)
						{
							i = _m_search(i, end);
							if(i == end)
							{
								exit_for = true;
								break;
							}

							window wd = nullptr;
							unsigned value = 0;

							switch(i->kind_of_element)
							{
							case field_t::element_t::kind::fixed:
								wd = i->u.fixed_ptr->first;
								value = i->u.fixed_ptr->second;
								break;
							case field_t::element_t::kind::percent:
								wd = i->u.percent_ptr->first;
								value = i->u.percent_ptr->second * area.width / 100;
								break;
							case field_t::element_t::kind::window:
								wd = i->u.handle;
								value = block_w;
								break;
							}
							++i;
							
							API::move_window(wd, x, y, (value > block_w ? block_w : value), block_h);
							x += static_cast<int>(block_w);
						}
						if(exit_for) break;
						y += static_cast<int>(block_h);
					}
				}
				else
				{
					unsigned block_w = area.width / dimension.first;
					unsigned block_h = area.height / dimension.second;

					char *table = new char[dimension.first * dimension.second];
					memset(table, 0, dimension.first * dimension.second);

					auto i = field->container().begin(), end = field->container().end();
					std::size_t lbp = 0;
					bool exit_for = false;

					for(std::size_t c = 0; c < dimension.second; ++c)
					{
						for(std::size_t l = 0; l < dimension.first; ++l)
						{
							if(table[l + lbp])
								continue;
							
							i = _m_search(i, end);
							if(i == end)
							{
								exit_for = true;
								break;
							}

							std::pair<unsigned, unsigned> room(1, 1);
							if(i->kind_of_element == field_t::element_t::kind::room)
							{
								room = i->u.room_ptr->second;
								if(room.first > dimension.first - l)
									room.first = dimension.first - l;
								if(room.second > dimension.second - c)
									room.second = dimension.second - c;
							}

								window wd = nullptr;
								unsigned value = 0;

								switch(i->kind_of_element)
								{
								case field_t::element_t::kind::fixed:
									wd = i->u.fixed_ptr->first;
									value = i->u.fixed_ptr->second;
									break;
								case field_t::element_t::kind::percent:
									wd = i->u.percent_ptr->first;
									value = i->u.percent_ptr->second * area.width / 100;
									break;
								case field_t::element_t::kind::window:
									wd = i->u.handle;
									value = block_w;
									break;
								}

								if(room.first <= 1 && room.second <= 1)
								{
									API::move_window(wd, area.x + l * block_w, area.y + c * block_h, block_w, block_h);
									table[l + lbp] = 1;
								}
								else
								{
									API::move_window(i->u.room_ptr->first, area.x + l * block_w, area.y + c * block_h, block_w * room.first, block_h * room.second);

									for(std::size_t y = 0; y < room.second; ++y)
										for(std::size_t x = 0; x < room.first; ++x)
										{
											table[l + x + lbp + y * dimension.first] = 1;
										}
								}
								++i;
						}

						if(exit_for)
							break;

						lbp += dimension.first;
					}

					delete [] table;
				}
			}

		private:
			static field_t::const_iterator _m_search(field_t::const_iterator i, field_t::const_iterator end)
			{
				if(i == end) return end;
				
				while(i->kind_of_element == field_t::element_t::kind::gap)
				{
					if(++i == end) return end;
				}
				return i;
			}

			std::size_t _m_number_of_window() const
			{
				if(nullptr == field) return 0;

				std::size_t n = 0;
				for(auto & el : field->container())
				{
					if(field_t::element_t::kind::gap != el.kind_of_element)
						++n;
				}
				return n;
			}
		public:
			std::pair<unsigned, unsigned> dimension;
		};

		struct implement
		{
			window window_handle;
			event_handle event_size_handle;
			division * root_division;
			std::map<std::string, field_t*> fields;

			implement()
				: window_handle(nullptr), event_size_handle(nullptr), root_division(nullptr)
			{}

			~implement()
			{
				API::umake_event(event_size_handle);
				delete root_division;
				for(auto & pair : fields)
				{
					delete pair.second;
				}
			}
		};

		class tokenizer
		{
		public:
			enum class token
			{
				div_start, div_end,
				identifier, vertical, grid, number, array,
				weight,
				equal,
				eof, error
			};

			tokenizer(const char* p)
				: divstr_(p), sp_(p)
			{}

			const std::string& idstr() const
			{
				return idstr_;
			}

			number_t number() const
			{
				return number_;
			}

			std::vector<number_t>& array()
			{
				return array_;
			}

			token read()
			{
				sp_ = _m_eat_whitespace(sp_);

				std::size_t readbytes = 0;
				switch(*sp_)
				{
				case '\0':
					return token::eof;
				case '=':
					++sp_;
					return token::equal;
				case '<':
					++sp_;
					return token::div_start;
				case '>':
					++sp_;
					return token::div_end;
				case '[':
					array_.clear();
					sp_ = _m_eat_whitespace(sp_ + 1);
					if(*sp_ == ']')
					{
						++sp_;
						return token::array;
					}

					while(true)
					{
						sp_ = _m_eat_whitespace(sp_);
						if(token::number != read())
							_m_throw_error("invalid array element");

						array_.push_back(number_);

						sp_ = _m_eat_whitespace(sp_);
						char ch = *sp_++;
						if(ch != ',')
						{
							if(ch == ']')
								break;
							_m_throw_error("invalid array");
						}
					}
					return token::array;
				case '.': case '-':
					if(*sp_ == '-')
					{
						readbytes = _m_number(sp_ + 1, true);
						if(readbytes)
							++ readbytes;
					}
					else
						readbytes = _m_number(sp_, false);
						
					if(readbytes)
					{
						sp_ += readbytes;
						return token::number;
					}
					else
						_m_throw_error(*sp_);
					break;
				default:
					if('0' <= *sp_ && *sp_ <= '9')
					{
						readbytes = _m_number(sp_, false);
						if(readbytes)
						{
							sp_ += readbytes;
							return token::number;
						}
					}
					break;
				}

				char ch = *sp_;
				if('_' == ch || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
				{
					const char * idstart = sp_++;
					ch = *sp_;
					while('_' == ch || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9'))
						ch = *++sp_;

					idstr_.assign(idstart, sp_);

					if(idstr_ == "weight")
					{
						if(token::equal != read())
							_m_throw_error("an equal sign is required after \'weight\'");

						const char* p = sp_;
						for(; *p == ' '; ++p);

						std::size_t len = 0;
						if(*p == '-')
						{
							len = _m_number(p + 1, true);
							if(len)	++len;
						}
						else
							len = _m_number(p, false);

						if(0 == len)
							_m_throw_error("the \'weight\' requires a number(integer or real or percent)");
						
						sp_ += len + (p - sp_);
						return token::weight;
					}
					else if(idstr_ == "vertical")
						return token::vertical;
					else if(idstr_ == "grid")
						return token::grid;

					return token::identifier;
				}

				return token::error;
			}
		private:
			void _m_throw_error(char err_char)
			{
				std::stringstream ss;
				ss<<"place: invalid character '"<<err_char<<"' at "<<(sp_ - divstr_);
				throw std::runtime_error(ss.str());
			}

			void _m_throw_error(const std::string& err)
			{
				std::stringstream ss;
				ss<<"place: "<<err<<" at "<<(sp_ - divstr_);
				throw std::runtime_error(ss.str());
			}

			const char* _m_eat_whitespace(const char* sp)
			{
				while(*sp)
				{
					switch(*sp)
					{
					case ' ': case '\t': case '\r': case '\n':
						++sp;	break;
					default:
						return sp;
					}
				}
				return sp;
			}

			std::size_t _m_number(const char* sp, bool negative)
			{
				const char* allstart = sp;
				sp = _m_eat_whitespace(sp);

				number_.assign(0);

				bool gotcha = false;
				int integer = 0;
				double real = 0;
				//read the integral part.
				const char* istart = sp;
				while('0' <= *sp && *sp <= '9')
				{
					integer = integer * 10 + (*sp - '0');
					++sp;
				}
				const char* iend = sp;

				if('.' == *sp)
				{
					double div = 1;
					const char* rstart = ++sp;
					while('0' <= *sp && *sp <= '9')
					{
						real += (*sp - '0') / (div *= 10);
						++sp;
					}

					if(rstart != sp)
					{
						real += integer;
						number_.assign(negative ? -real : real);
						gotcha = true;
					}
				}
				else if(istart != iend)
				{
					number_.assign(negative ? - integer : integer);
					gotcha = true;
				}

				if(gotcha)
				{
					for(;*sp == ' '; ++sp);
					if('%' == *sp)
					{
						if(number_t::kind::integer == number_.kind_of())
							number_.assign_percent(number_.integer());
						return sp - allstart + 1;
					}
					return sp - allstart;
				}
				return 0;
			}
		private:
			const char* divstr_;
			const char* sp_;
			std::string idstr_;
			number_t number_;
			std::vector<number_t> array_;
		};
	public:
		place()
			: impl_(new implement)
		{}

		place(window wd)
			: impl_(new implement)
		{
			impl_->window_handle = wd;
			impl_->event_size_handle = API::make_event<events::size>(wd, nana::make_fun(*this, &place::_m_resize));
		}

		~place()
		{
			delete impl_;
		}

		void div(const char* s)
		{
			for(auto pair : impl_->fields)
			{
				pair.second->attached = false;
			}
			tokenizer tknizer(s);
			delete impl_->root_division;
			impl_->root_division = _m_scan_div(tknizer);
		}

		static fixed_t fixed(window wd, unsigned size)
		{
			return fixed_t(wd, size);
		}

		static percent_t percent(window wd, int per)
		{
			return percent_t(wd, per);
		}

		static room_t room(window wd, unsigned w, unsigned h)
		{
			return room_t(wd, std::pair<unsigned, unsigned>(w, h));
		}

		field_t& field(const char* name)
		{
			name = name ? name : "";

			auto * p = impl_->fields[name];
			if(nullptr == p)
				p = (impl_->fields[name] = new field_t);

			if((false == p->attached) && impl_->root_division)
			{
				division * div = _m_search_div_name(impl_->root_division, name);
				if(div)
				{
					div->field = p;
					p->attached = true;
				}
			}
			return *p;
		}

		void collocate()
		{
			if(impl_->root_division && impl_->window_handle)
			{
				impl_->root_division->area = API::window_size(impl_->window_handle);
				impl_->root_division->collocate();
			}
		}
	private:
		division* _m_search_div_name(division* start, const std::string& name)
		{
			if(nullptr == start) return nullptr;

			if(start->name == name) return start;

			for(auto child : start->children)
			{
				division * div = _m_search_div_name(child, name);
				if(div)
					return div;
			}
			return nullptr;
		}

		void _m_throw_error()
		{
			throw std::runtime_error("place. invalid div syntax");
		}

		void _m_resize(const eventinfo& ei)
		{
			if(impl_->root_division)
			{
				impl_->root_division->area = API::window_size(ei.window);
				impl_->root_division->collocate();
			}
		}

		division* _m_scan_div(tokenizer& tknizer)
		{
			typedef tokenizer::token token;

			division * div = nullptr;
			token div_type = token::eof;
			std::string name;

			number_t weight;
			std::vector<number_t> array;

			std::vector<division*> children;
			for(token tk = tknizer.read(); tk != token::eof; tk = tknizer.read())
			{
				bool exit_for = false;
				switch(tk)
				{
				case token::div_start:
					children.push_back(_m_scan_div(tknizer));
					break;
				case token::vertical:
				case token::grid:
					div_type = tk;
					break;
				case token::array:
					tknizer.array().swap(array);
					break;
				case token::weight:
					weight = tknizer.number();
					//If the weight is type of real, convert it to integer.
					//the integer and percent are allowed for weight.
					if(weight.kind_of() == number_t::kind::real)
						weight.assign(static_cast<int>(weight.real()));
					break;
				case token::div_end:
					exit_for = true;
					break;
				case token::identifier:
					name = tknizer.idstr();
					break;
				}
				if(exit_for)
					break;
			}

			field_t * field = nullptr;
			if(name.size())
			{
				auto i = impl_->fields.find(name);
				if(impl_->fields.end() != i)
				{
					field = i->second;
					if(field->attached)
						throw std::runtime_error("place, the name \'"+ name +"\' is redefined.");

					field->attached = true;
				}
			}

			switch(div_type)
			{
			case token::eof:
				div = new div_arrange(std::move(name));
				break;
			case token::vertical:
				div = new div_vertical_arrange(std::move(name));
				break;
			case token::grid:
				{
					div_grid * p = new div_grid(std::move(name));
					
					if(array.size())
					{
						if(array[0].kind_of() != number_t::kind::percent)
							p->dimension.first = array[0].integer();
					}
					
					if(array.size() > 1)
					{
						if(array[1].kind_of() != number_t::kind::percent)
							p->dimension.second = array[1].integer();
					}

					if(0 == p->dimension.first)
						p->dimension.first = 1;

					if(0 == p->dimension.second)
						p->dimension.second = 1;

					div = p;
				}
				break;
			}
			
			div->weight = weight;
			div->field = field;
			div->children.swap(children);
			return div;
		}
	private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana