
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

		class field_t
		{
		public:
			struct element_t
			{
				enum class kind
				{
					window, gap, fixed, percent
				};

				kind kind_of_element;
				union
				{
					window handle;
					unsigned gap_value;
					fixed_t *	fixed_ptr;
					percent_t * percent_ptr;
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

				element_t(const element_t& rhs)
					: kind_of_element(rhs.kind_of_element), u(rhs.u)
				{
					switch(kind_of_element)
					{
					case kind::fixed:
						u.fixed_ptr = new fixed_t(*rhs.u.fixed_ptr);
						break;
					case kind::percent:
						u.percent_ptr = new percent_t(*rhs.u.percent_ptr);
						break;
					default:	break;
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
					default:	break;
					}
				}
			};
		public:
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
				: kind_of_division(k), name(std::move(n)), fixed(0), percent(0), field(nullptr)
			{}

			virtual ~division()
			{
				for(auto p : children)
				{
					delete p;
				}
			}

			std::pair<unsigned, std::size_t> fixed_pixels(kind match_kind) const
			{
				if(fixed)
					return std::pair<unsigned, std::size_t>(fixed, 1);

				std::pair<unsigned, std::size_t> pair;
				if(field && (kind_of_division == match_kind))
					pair = field->fixed_and_adjustable();
				
				for(auto child : children)
				{
					if(0 == child->fixed) //it is adjustable
					{
						if(0 == child->percent)
							++pair.second;
					}
					else
						pair.first += child->fixed;
				}
				return pair;
			}

			virtual void collocate() = 0;

		public:
			kind kind_of_division;
			const std::string name;
			std::vector<division*> children;
			nana::rectangle area;
			unsigned fixed;
			unsigned percent;
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

					unsigned adj_px = child->fixed;
					if(0 == adj_px) //the child is adjustable
					{
						if(false == child->percent)
						{
							adj_px = child->fixed_pixels(kind::arrange).first;
							if(adj_px <= adjustable_pixels)
								adj_px = adjustable_pixels;
						}
						else
							adj_px = area.width * child->percent / 100;
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

					unsigned adj_px = child->fixed;
					if(0 == adj_px) //the child is adjustable
					{
						if(false == child->percent)
						{
							adj_px = child->fixed_pixels(kind::vertical_arrange).first;
							if(adj_px <= adjustable_pixels)
								adj_px = adjustable_pixels;
						}
						else
							adj_px = area.height * child->percent / 100;
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
			{}

			virtual void collocate()
			{}
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
			}
		};

		class tokenizer
		{
		public:
			enum class token
			{
				div_start, div_end,
				identifier, vertical, grid, number,
				eof, error
			};

			tokenizer(const char* p)
				: divstr_(p), sp_(p)
			{}

			const std::string& idstr() const
			{
				return idstr_;
			}

			token read()
			{
				_m_eat_whitespace();

				std::size_t readbytes = 0;
				switch(*sp_)
				{
				case '\0':
					return token::eof;
				case '<':
					++sp_;
					return token::div_start;
				case '>':
					++sp_;
					return token::div_end;
				case '.': case '-':
					readbytes = _m_number(sp_, *sp_ == '-');
					if(readbytes)
					{
						sp_ += readbytes;
						return token::number;
					}
					else
						_m_throw_error(*sp_);
					break;
				}

				readbytes = _m_number(sp_, false);
				if(readbytes)
				{
					sp_ += readbytes;
					return token::number;
				}

				char ch = *sp_;
				if('_' == ch || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z'))
				{
					const char * idstart = sp_++;
					ch = *sp_;
					while('_' == ch || ('a' <= ch && ch <= 'z') || ('A' <= ch && ch <= 'Z') || ('0' <= ch && ch <= '9'))
						ch = *++sp_;

					idstr_.assign(idstart, sp_);
					if(idstr_ == "vertical")
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

			void _m_eat_whitespace()
			{
				while(*sp_)
				{
					switch(*sp_)
					{
					case ' ': case '\t': case '\r': case '\n':
						++sp_;	break;
					default:
						return;
					}
				}			
			}

			std::size_t _m_number(const char* sp, bool negative)
			{
				const char* allstart = sp;
				_m_eat_whitespace();

				integer_ = 0;
				real_ = 0;
				//read the integral part.
				const char* istart = sp;
				while('0' <= *sp && *sp <= '9')
				{
					integer_ = integer_ * 10 + (*sp - '0');
					++sp;
				}
				const char* iend = sp;

				if('.' == *sp++)
				{
					double div = 1;
					const char* rstart = sp;
					while('0' <= *sp && *sp <= '9')
					{
						real_ += (*sp - '0') / (div *= 10);
						++sp;
					}

					if(rstart != sp)
					{
						is_integer_ = false;
						real_ += integer_;
						if(negative)
							real_ = - real_;
						return sp - allstart;

					}
				}
				if(istart != iend)
				{
					is_integer_ = true;
					if(negative)
						integer_ = - integer_;
					return sp - allstart;
				}
				return 0;
			}
		private:
			const char* divstr_;
			const char* sp_;
			std::string idstr_;
			bool is_integer_;
			int	integer_;
			double real_;
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
				div = new div_grid(std::move(name));
				break;
			}

			div->field = field;
			div->children.swap(children);
			return div;
		}
	private:
		implement * impl_;
	};
}//end namespace gui
}//end namespace nana