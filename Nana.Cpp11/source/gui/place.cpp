/*
 *	An Implementation of Place for Layout
 *	Nana C++ Library(http://www.nanapro.org)
 *	Copyright(C) 2003-2014 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Boost Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://www.boost.org/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/place.cpp
 */
#include <sstream>
#include <cfloat>
#include <cmath>
#include <stdexcept>
#include <cstring>
#include <nana/gui/place.hpp>
#include <nana/gui/programming_interface.hpp>
#include <nana/gui/widgets/widget.hpp>
#include <nana/gui/dragger.hpp>


namespace nana
{
	namespace place_parts
	{
		class splitter_interface
		{
		public:
			virtual ~splitter_interface(){}
		};

		class splitter_dtrigger
			: public drawer_trigger
		{
		};

		template<bool IsLite>
		class splitter
			: public widget_object <typename std::conditional<IsLite, category::lite_widget_tag, category::widget_tag>::type, splitter_dtrigger>,
			public splitter_interface
		{
		};


		//number_t is used for storing a number type variable
		//such as integer, real and percent. Essentially, percent is a typo of real.
		class number_t
		{
		public:
			enum class kind{ none, integer, real, percent };

			number_t()
				: kind_(kind::none)
			{
				value_.integer = 0;
			}

			bool is_none() const
			{
				return (kind::none == kind_);
			}

			kind kind_of() const
			{
				return kind_;
			}

			int integer() const
			{
				if (kind::integer == kind_)
					return value_.integer;
				return static_cast<int>(value_.real);
			}

			double real() const
			{
				if (kind::integer == kind_)
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
				value_.real = d / 100;
			}
		private:
			kind kind_;
			union valueset
			{
				int integer;
				double real;
			}value_;
		};//end class number_t

		class margin
		{
		public:
			margin& operator=(margin&& rhs)
			{
				if (this != &rhs)
				{
					all_edges_ = rhs.all_edges_;
					margins_ = std::move(rhs.margins_);
				}
				return *this;
			}

			void clear()
			{
				all_edges_ = true;
				margins_.clear();
			}

			void push(const number_t& v)
			{
				margins_.emplace_back(v);
			}

			void set_value(const number_t& v)
			{
				clear();
				margins_.emplace_back(v);
			}

			void set_array(const std::vector<number_t>& v)
			{
				all_edges_ = false;
				margins_ = v;
			}

			nana::rectangle area(const ::nana::rectangle& field_area) const
			{
				if (margins_.empty())
					return field_area;

				auto r = field_area;
				if (all_edges_)
				{
					int px = 0;
					if (margins_.back().kind_of() == number_t::kind::percent)
						px = static_cast<int>(margins_.back().real() * r.width);
					else
						px = margins_.back().integer();
					const auto dbl_px = static_cast<unsigned>(px << 1);
					r.x += px;
					r.width = (r.width < dbl_px ? 0 : r.width - dbl_px);

					r.y += px;
					r.height = (r.height < dbl_px ? 0 : r.height - dbl_px);
				}
				else
				{
					int il{ -1 }, ir{ -1 }, it{ -1 }, ib{ -1 };	//index of four corners in margin
					switch (margins_.size())
					{
					case 0:	break;
					case 1:	//top
						it = 0;
						break;
					case 2://top,bottom and left,right
						it = ib = 0;
						il = ir = 1;
						break;
					default:
						il = 3;	//left
					case 3:	//top, right, bottom
						it = 0;
						ir = 1;
						ib = 2;
					}

					typedef decltype(r.height) px_type;
					auto calc = [](px_type a, px_type b)
					{
						return (a > b ? a - b : 0);
					};

					int px;
					if (0 == it)	//top
					{
						if (margins_.front().kind_of() == number_t::kind::percent)
							px = static_cast<int>(field_area.height * margins_.front().real());
						else
							px = margins_.front().integer();

						r.y += px;
						r.height = calc(r.height, static_cast<px_type>(px));
					}

					if (-1 != ib)	//bottom
					{
						if (margins_[ib].kind_of() == number_t::kind::percent)
							px = static_cast<int>(field_area.height * margins_[ib].real());
						else
							px = margins_[ib].integer();

						r.height = calc(r.height, static_cast<px_type>(px));
					}

					if (-1 != il)	//left
					{
						if (margins_[il].kind_of() == number_t::kind::percent)
							px = static_cast<int>(field_area.width * margins_[il].real());
						else
							px = margins_[il].integer();

						r.x += px;
						r.width = calc(r.width, static_cast<px_type>(px));
					}

					if (-1 != ir)	//right
					{
						if (margins_[ir].kind_of() == number_t::kind::percent)
							px = static_cast<int>(field_area.width * margins_[ir].real());
						else
							px = margins_[ir].integer();
						r.width = calc(r.width, static_cast<px_type>(px));
					}
				}
				return r;
			}
		private:
			bool all_edges_ = true;
			std::vector<number_t> margins_;
		};//end class margin

		class repeated_array
		{
		public:
			repeated_array()
			{}

			repeated_array(repeated_array&& rhs)
				: repeated_(rhs.repeated_),
				values_(std::move(rhs.values_))
			{
			}

			void assign(std::vector<number_t>&& c)
			{
				values_ = std::move(c);
			}

			bool empty() const
			{
				return values_.empty();
			}

			void reset()
			{
				repeated_ = false;
				values_.clear();
			}

			void repeated()
			{
				repeated_ = true;
			}

			void push(const number_t& n)
			{
				values_.emplace_back(n);
			}

			number_t at(std::size_t pos) const
			{
				if (values_.empty())
					return{};

				if (repeated_)
					pos %= values_.size();
				else if (pos >= values_.size())
					return{};

				return values_[pos];
			}
		private:
			bool repeated_ = false;
			std::vector<number_t> values_;
		};
	}//end namespace place_parts

	typedef place_parts::number_t number_t;
	typedef place_parts::repeated_array repeated_array;

	class tokenizer
	{
	public:
		enum class token
		{
			div_start, div_end, splitter,
			identifier, vert, grid, number, array, reparray,
			weight, gap, margin, track, arrange, repeated,
			collapse, parameters,
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

		repeated_array& reparray()
		{
			return reparray_;
		}

		std::vector<number_t>& parameters()
		{
			return parameters_;
		}

		token read()
		{
			sp_ = _m_eat_whitespace(sp_);

			std::size_t readbytes = 0;
			switch (*sp_)
			{
			case '\0':
				return token::eof;
			case '|':
				++sp_;
				return token::splitter;
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
				if (*sp_ == ']')
				{
					++sp_;
					return token::array;
				}

				while (true)
				{
					sp_ = _m_eat_whitespace(sp_);

					auto tk = read();
					if (token::number == tk)
					{
						array_.push_back(number_);
					}
					else if (token::repeated == tk)
					{
						reparray_.assign(std::move(array_));
						reparray_.repeated();
					}
					else
						_m_throw_error("invalid array element");

					sp_ = _m_eat_whitespace(sp_);
					char ch = *sp_++;

					if (ch == ']')
						return (token::number == tk ? token::array : token::reparray);

					if (ch != ',')
						_m_throw_error("invalid array");
				}
				break;
			case '(':
				parameters_.clear();
				sp_ = _m_eat_whitespace(sp_ + 1);
				if (*sp_ == ')')
				{
					++sp_;
					return token::parameters;
				}

				while (true)
				{
					if (token::number == read())
						parameters_.push_back(number_);
					else
						_m_throw_error("invalid parameter.");

					sp_ = _m_eat_whitespace(sp_);
					char ch = *sp_++;

					if (ch == ')')
						return token::parameters;

					if (ch != ',')
						_m_throw_error("invalid parameter.");
				}
				break;
			case '.': case '-':
				if (*sp_ == '-')
				{
					readbytes = _m_number(sp_ + 1, true);
					if (readbytes)
						++readbytes;
				}
				else
					readbytes = _m_number(sp_, false);

				if (readbytes)
				{
					sp_ += readbytes;
					return token::number;
				}
				else
					_m_throw_error(*sp_);
				break;
			default:
				if ('0' <= *sp_ && *sp_ <= '9')
				{
					readbytes = _m_number(sp_, false);
					if (readbytes)
					{
						sp_ += readbytes;
						return token::number;
					}
				}
				break;
			}

			if ('_' == *sp_ || isalpha(*sp_))
			{
				const char * idstart = sp_++;

				while ('_' == *sp_ || isalpha(*sp_) || isalnum(*sp_))
					++sp_;

				idstr_.assign(idstart, sp_);

				if ("weight" == idstr_)
				{
					_m_attr_number_value();
					return token::weight;
				}
				else if ("vertical" == idstr_ || "vert" == idstr_ || "repeated" == idstr_)
					return ('v' == idstr_[0] ? token::vert : token::repeated);
				else if ("arrange" == idstr_ || "gap" == idstr_)
				{
					_m_attr_reparray();
					return ('a' == idstr_[0] ? token::arrange : token::gap);
				}
				else if ("margin" == idstr_ || "track" == idstr_ || "grid" == idstr_)
				{
					if (token::equal != read())
						_m_throw_error("an equal sign is required after '" + idstr_ + "'");

					switch (idstr_[0])
					{
					case 'm': return token::margin;
					case 't': return token::track;
					case 'g': return token::grid;
					}
				}
				else if ("collapse" == idstr_)
				{
					if (token::parameters != read())
						_m_throw_error("a parameter list is required after 'collapse'");
					return token::collapse;
				}
				return token::identifier;
			}

			return token::error;
		}
	private:
		void _m_throw_error(char err_char)
		{
			std::stringstream ss;
			ss << "place: invalid character '" << err_char << "' at " << static_cast<unsigned>(sp_ - divstr_);
			throw std::runtime_error(ss.str());
		}

		void _m_attr_number_value()
		{
			if (token::equal != read())
				_m_throw_error("an equal sign is required after '" + idstr_ + "'");

			const char* p = sp_;
			for (; *p == ' '; ++p);

			std::size_t len = 0;
			if (*p == '-')
			{
				len = _m_number(p + 1, true);
				if (len)	++len;
			}
			else
				len = _m_number(p, false);

			if (0 == len)
				_m_throw_error("the '" + idstr_ + "' requires a number(integer or real or percent)");

			sp_ += len + (p - sp_);
		}

		void _m_attr_reparray()
		{
			auto idstr = idstr_;
			if (token::equal != read())
				_m_throw_error("an equal sign is required after '" + idstr + "'");

			const char* p = sp_;
			for (; *p == ' '; ++p);

			reparray_.reset();
			auto tk = read();
			switch (tk)
			{
			case token::number:
				reparray_.push(number());
				reparray_.repeated();
				break;
			case token::array:
				reparray_.assign(std::move(array_));
				break;
			case token::reparray:
				break;
			default:
				_m_throw_error("a (repeated) array is required after '" + idstr + "'");
			}
		}

		void _m_throw_error(const std::string& err)
		{
			std::stringstream ss;
			ss << "place: " << err << " at " << static_cast<unsigned>(sp_ - divstr_);
			throw std::runtime_error(ss.str());
		}

		const char* _m_eat_whitespace(const char* sp)
		{
			while (*sp && !isgraph(*sp))
				++sp;
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
			while ('0' <= *sp && *sp <= '9')
			{
				integer = integer * 10 + (*sp - '0');
				++sp;
			}
			const char* iend = sp;

			if ('.' == *sp)
			{
				double div = 1;
				const char* rstart = ++sp;
				while ('0' <= *sp && *sp <= '9')
				{
					real += (*sp - '0') / (div *= 10);
					++sp;
				}

				if (rstart != sp)
				{
					real += integer;
					number_.assign(negative ? -real : real);
					gotcha = true;
				}
			}
			else if (istart != iend)
			{
				number_.assign(negative ? -integer : integer);
				gotcha = true;
			}

			if (gotcha)
			{
				for (; *sp == ' '; ++sp);
				if ('%' == *sp)
				{
					if (number_t::kind::integer == number_.kind_of())
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
		repeated_array		reparray_;
		std::vector<number_t> parameters_;
	};	//end class tokenizer


	//struct implement
	struct place::implement
	{
		class field_impl;
		class division;
		class div_arrange;
		class div_grid;
		class div_splitter;

		window window_handle;
		event_handle event_size_handle;
		division * root_division;
		std::map<std::string, field_impl*> fields;

		implement()
			: window_handle(nullptr), event_size_handle(nullptr), root_division(nullptr)
		{}

		//The following functions are defined behind the definition of class division.
		//because the class division here is an incomplete type.
		~implement();
		static division * search_div_name(division* start, const std::string&);
		division * scan_div(tokenizer&);
	};	//end struct implement


	place::field_t::~field_t(){}

	class place::implement::field_impl
		: public place::field_t
	{
	public:
		struct element_t
		{
			window handle;
			event_handle evt_destroy;

			element_t(window h, event_handle event_destroy)
				:handle(h), evt_destroy(event_destroy)
			{}
		};

		field_impl(place * p)
			: attached(false),
			place_ptr_(p)
		{}
	private:
		//Listen to destroy of a window
		//It will delete the element and recollocate when the window destroyed.
		event_handle _m_make_destroy(window wd)
		{
			return API::events(wd).destroy.connect([this](const arg_destroy& arg)
			{
				for (auto i = elements.begin(), end = elements.end(); i != end; ++i)
				{
					if (arg.window_handle == i->handle)
					{
						elements.erase(i);
						break;
					}
				}
				place_ptr_->collocate();
			});
		}

		field_t& operator<<(window wd) override
		{
			if (API::empty_window(wd))
				throw std::invalid_argument("Place: An invalid window handle.");

			if (API::get_parent_window(wd) != place_ptr_->window_handle())
				throw std::invalid_argument("Place: the window is not a child of place bind window");

			auto evt = _m_make_destroy(wd);
			elements.emplace_back(wd, evt);
			return *this;
		}

		field_t& fasten(window wd) override
		{
			fastened.push_back(wd);

			//Listen to destroy of a window. The deleting a fastened window
			//does not change the layout.
			API::events(wd).destroy.connect([this](const arg_destroy& arg)
			{
				auto destroyed_wd = arg.window_handle;
				auto i = std::find_if(fastened.begin(), fastened.end(), [destroyed_wd](window wd){
					return (wd == destroyed_wd);
				});

				if (i != fastened.end())
					fastened.erase(i);
			});
			return *this;
		}
	public:
		bool attached;
		std::vector<element_t>	elements;
		std::vector<window>	fastened;
	private:
		place * place_ptr_;
	};//end class field_impl

	class place::implement::division
	{
	public:
		enum class kind{ arrange, vertical_arrange, grid, splitter };

		division(kind k, std::string&& n)
			: kind_of_division(k),
			name(std::move(n)),
			field(nullptr),
			div_next(nullptr),
			div_owner(nullptr)
		{}

		virtual ~division()
		{
			//detach the field
			if (field)
				field->attached = false;

			for (auto p : children)
			{
				delete p;
			}
		}

		bool is_fixed() const
		{
			return (weight.kind_of() == number_t::kind::integer);
		}

		bool is_percent() const
		{
			return (weight.kind_of() == number_t::kind::percent);
		}

		nana::rectangle margin_area() const
		{
			return margin.area(field_area);
		}

		division * find_last_adjustable()
		{
			for (auto i = children.rbegin(); i != children.rend(); ++i)
			{
				if (false == (*i)->is_fixed())
					return (*i);
			}
			return nullptr;
		}
	public:
		//Collocate the division and its children divisions,
		//The window parameter is specified for the window which the place object binded.
		virtual void collocate(window) = 0;

	public:
		kind kind_of_division;
		const std::string name;
		std::vector<division*> children;
		nana::rectangle field_area;
		number_t weight;

		place_parts::margin	margin;
		place_parts::repeated_array gap;
		field_impl * field;
		division * div_next, *div_owner;
	};

	class place::implement::div_arrange
		: public division
	{
	public:
		div_arrange(bool vert, std::string&& name, place_parts::repeated_array&& arr)
			: division((vert ? kind::vertical_arrange : kind::arrange), std::move(name)),
			arrange_(std::move(arr))
		{}

		void collocate(window wd) override
		{
			const auto area = margin_area();
			const auto area_px = _m_area_pixels(area);

			auto fa = _m_fixed_and_adjustable(kind_of_division, area_px);

			double adjustable_pixels = (fa.second && fa.first < area_px ? (double(area_px - fa.first) / fa.second) : 0.0);

			//double left = area.x;	//deprecated
			double position = (kind_of_division == kind::arrange ? area.x : area.y);
			double precise_px = 0;
			for (auto child : children)					/// First collocate child div's !!!
			{
				_m_assign_area_tri(child->field_area, static_cast<int>(position), area);

				double child_px;							/// and calculate this div.
				if (!child->is_fixed())					/// with is fixed for fixed div
				{
					if (child->is_percent())			/// and calculated for others: if the child div is percent - simple take it full
					{
						auto px = area_px * child->weight.real() + precise_px;
						auto npx = static_cast<unsigned>(px);
						precise_px = px - npx;
						child_px = npx;
					}
					else
						child_px = adjustable_pixels;
				}
				else
					child_px = static_cast<unsigned>(child->weight.integer());

				//Use 'endpos' to calc width is to avoid deviation
				int endpos = static_cast<int>(position + child_px);
				const int area_edp = _m_area_endpos(area);

				if ((!child->is_fixed()) && child == children.back() && (endpos != area_edp))
					endpos = area_edp;

				auto& child_area_px = _m_area_pixels_ref(child->field_area);
				child_area_px = static_cast<unsigned>(endpos - _m_area_pos(child->field_area));

				position += child_px;
				child->collocate(wd);	/// The child div have full position. Now we can collocate  inside it the child fields and child-div. 
			}

			if (field)
			{
				auto element_r = area;
				std::size_t index = 0;
				double precise_px = 0;
				for (auto & el : field->elements)
				{
					_m_area_pos_ref(element_r) = static_cast<int>(position);

					auto px = _m_calc_number(arrange_.at(index), area_px, adjustable_pixels, precise_px);
					_m_area_pixels_ref(element_r) = px;
					API::move_window(el.handle, element_r);

					if (index + 1 < field->elements.size())
					{
						position += px;
						position += _m_calc_number(gap.at(index), area_px, 0, precise_px);
					}
					++index;
				}

				for (auto & fsn : field->fastened)
					API::move_window(fsn, area);
			}
		}
	private:
		unsigned _m_calc_number(const place_parts::number_t& number, unsigned area_px, double adjustable_px, double& precise_px)
		{
			switch (number.kind_of())
			{
			case number_t::kind::integer:
				return static_cast<unsigned>(number.integer());
			case number_t::kind::real:
				return static_cast<unsigned>(number.real());
			case number_t::kind::percent:
			{
				auto fpx = area_px * number.real() + precise_px;
				auto px = static_cast<unsigned>(fpx);
				precise_px = fpx - px;
				return px;
			}
				break;
			case number_t::kind::none:
			{
				auto fpx = adjustable_px + precise_px;
				auto px = static_cast<unsigned>(fpx);
				precise_px = fpx - px;
				return px;
			}
				break;
			}
			return 0; //Useless
		}

		std::pair<unsigned, std::size_t> _m_calc_fa(const place_parts::number_t& number, unsigned area_px, double& precise_px) const
		{
			std::pair<unsigned, std::size_t> result;
			switch (number.kind_of())
			{
			case number_t::kind::integer:
				result.first = static_cast<unsigned>(number.integer());
				break;
			case number_t::kind::real:
				result.first = static_cast<unsigned>(number.real());
				break;
			case number_t::kind::percent:
			{
				double px = number.real() * area_px + precise_px;
				auto npx = static_cast<unsigned>(px);
				result.first = npx;
				precise_px = px - npx;
			}
				break;
			case number_t::kind::none:
				++result.second;
				break;
			}
			return result;
		}

		//Returns the fixed pixels and the number of adjustable items.
		std::pair<unsigned, std::size_t> _m_fixed_and_adjustable(kind match_kind, unsigned area_px) const
		{
			std::pair<unsigned, std::size_t> result;
			if (field && (kind_of_division == match_kind))
			{
				//Calculate fixed and adjustable of elements
				double precise_px = 0;
				auto count = field->elements.size();
				for (decltype(count) i = 0; i < count; ++i)
				{
					auto fa = _m_calc_fa(arrange_.at(i), area_px, precise_px);
					result.first += fa.first;
					result.second += fa.second;

					if (i + 1 < count)
					{
						fa = _m_calc_fa(gap.at(i), area_px, precise_px);
						result.first += fa.first;
						//fa.second is ignored for gap, because the it has not the adjustable gap.
					}
				}
			}

			double precise_px = 0;
			for (auto child : children)
			{
				if (child->is_percent())
				{
					auto px = child->weight.real() * area_px + precise_px;
					auto npx = static_cast<unsigned>(px);
					result.first += npx;
					precise_px = px - npx;
				}
				else if (child->is_fixed())
					result.first += static_cast<unsigned>(child->weight.integer());
				else
					++result.second;
			}
			return result;
		}

		unsigned _m_area_pixels(const ::nana::rectangle& area) const
		{
			return (kind_of_division == kind::arrange ? area.width : area.height);
		}

		unsigned& _m_area_pixels_ref(::nana::rectangle& area) const
		{
			return (kind_of_division == kind::arrange ? area.width : area.height);
		}

		int _m_area_pos(const ::nana::rectangle& area) const
		{
			return (kind_of_division == kind::arrange ? area.x : area.y);
		}

		int& _m_area_pos_ref(::nana::rectangle& area) const
		{
			return (kind_of_division == kind::arrange ? area.x : area.y);
		}

		int _m_area_endpos(const ::nana::rectangle& area) const
		{
			return (kind_of_division == kind::arrange ? area.right() : area.bottom());
		}

		void _m_assign_area_tri(::nana::rectangle & dst, int position, const ::nana::rectangle& area) const
		{
			if (kind_of_division == kind::arrange)
			{
				dst.x = position;
				dst.y = area.y;
				dst.height = area.height;
			}
			else
			{
				dst.x = area.x;
				dst.y = position;
				dst.width = area.width;
			}
		}
	private:
		place_parts::repeated_array arrange_;
	};

	class place::implement::div_grid
		: public division
	{
	public:
		div_grid(std::string&& name, place_parts::repeated_array&& arrange, std::vector<::nana::rectangle>&& collapses)
			: division(kind::grid, std::move(name)),
			arrange_(std::move(arrange)),
			collapses_(std::move(collapses))
		{
			dimension.first = dimension.second = 0;
		}

		void collocate(window wd) override
		{
			if (nullptr == field)
				return;

			auto area = field_area;

			auto gap_number = gap.at(0);
			unsigned gap_size = (gap_number.kind_of() == number_t::kind::percent ?
				static_cast<unsigned>(area.width * gap_number.real()) : static_cast<unsigned>(gap_number.integer()));

			if (dimension.first <= 1 && dimension.second <= 1)
			{
				auto n_of_wd = field->elements.size();
				std::size_t edge;
				switch (n_of_wd)
				{
				case 0:
				case 1:
					edge = 1;	break;
				case 2: case 3: case 4:
					edge = 2;	break;
				default:
					edge = static_cast<std::size_t>(std::sqrt(n_of_wd));
					if ((edge * edge) < n_of_wd) ++edge;
				}

				bool exit_for = false;
				double y = area.y;
				double block_w = area.width / double(edge);
				double block_h = area.height / double((n_of_wd / edge) + (n_of_wd % edge ? 1 : 0));
				unsigned uns_block_w = static_cast<unsigned>(block_w);
				unsigned uns_block_h = static_cast<unsigned>(block_h);
				unsigned height = (uns_block_h > gap_size ? uns_block_h - gap_size : uns_block_h);

				auto i = field->elements.cbegin(), end = field->elements.cend();
				std::size_t arr_pos = 0;
				for (std::size_t u = 0; u < edge; ++u)
				{
					double x = area.x;
					for (std::size_t v = 0; v < edge; ++v)
					{
						if (i == end)
						{
							exit_for = true;
							break;
						}

						unsigned value = 0;
						auto arr = arrange_.at(arr_pos++);
						switch (arr.kind_of())
						{
						case number_t::kind::integer:
							value = static_cast<decltype(value)>(arr.integer());
							break;
						case number_t::kind::real:
							value = static_cast<decltype(value)>(arr.real());
							break;
						case number_t::kind::percent:
							value = static_cast<decltype(value)>(arr.real() * area.width);
							break;
						default:
							value = static_cast<decltype(value)>(block_w);
						}

						unsigned width = (value > uns_block_w ? uns_block_w : value);
						if (width > gap_size)	width -= gap_size;
						API::move_window(i->handle, static_cast<int>(x), static_cast<int>(y), width, height);
						x += block_w;
						++i;
					}
					if (exit_for) break;
					y += block_h;
				}
			}
			else
			{
				double block_w = int(area.width - gap_size * (dimension.first - 1)) / double(dimension.first);
				double block_h = int(area.height - gap_size * (dimension.second - 1)) / double(dimension.second);

				std::unique_ptr<char[]> table_ptr(new char[dimension.first * dimension.second]);

				char *table = table_ptr.get();
				std::memset(table, 0, dimension.first * dimension.second);

				std::size_t lbp = 0;
				bool exit_for = false;

				auto i = field->elements.cbegin(), end = field->elements.cend();

				for (std::size_t c = 0; c < dimension.second; ++c)
				{
					double precise_w = 0, precise_h = 0;
					for (std::size_t l = 0; l < dimension.first; ++l)
					{
						if (table[l + lbp])
							continue;

						if (i == end)
						{
							exit_for = true;
							break;
						}

						std::pair<unsigned, unsigned> room{ 1, 1 };

						_m_find_collapse(l, c, room);

						int pos_x = area.x + static_cast<int>(l * (block_w + gap_size));
						int pos_y = area.y + static_cast<int>(c * (block_h + gap_size));
						if (room.first <= 1 && room.second <= 1)
						{
							precise_w += block_w;
							precise_h += block_h;
							table[l + lbp] = 1;
						}
						else
						{
							precise_w += block_w * room.first + (room.first - 1) * gap_size;
							precise_h += block_h * room.second + (room.second - 1) * gap_size;
							for (std::size_t y = 0; y < room.second; ++y)
								for (std::size_t x = 0; x < room.first; ++x)
									table[l + x + lbp + y * dimension.first] = 1;
						}
						//precise_w/h may be negative values, in such case, the size of all gaps are larger than area's size.
						unsigned result_w, result_h;
						if (precise_w >= 0.0001)
						{
							result_w = static_cast<unsigned>(precise_w);
							precise_w -= result_w;
						}
						else
						{
							result_w = 0;
							precise_w = 0;
						}

						if (precise_h >= 0.0001)
						{
							result_h = static_cast<unsigned>(precise_h);
							precise_h -= result_h;
						}
						else
						{
							result_h = 0;
							precise_h = 0;
						}

						API::move_window(i->handle, pos_x, pos_y, result_w, result_h);
						++i;
					}

					if (exit_for)
						break;

					lbp += dimension.first;
				}
			}

			for (auto & fsn : field->fastened)
				API::move_window(fsn, area);
		}
	public:
		std::pair<unsigned, unsigned> dimension;
	private:
		void _m_find_collapse(int x, int y, std::pair<unsigned, unsigned>& collapse) const
		{
			for (auto & col : collapses_)
			{
				if (col.x == x && col.y == y)
				{
					collapse.first = col.width;
					collapse.second = col.height;
					return;
				}
			}
		}
	private:
		place_parts::repeated_array arrange_;
		std::vector<::nana::rectangle> collapses_;
	};//end class div_grid

	class place::implement::div_splitter
		: public division
	{
		struct div_block
		{
			division * div;
			int	pixels;
			double		scale;

			div_block(division* d, int px)
				: div(d), pixels(px)
			{}
		};
	public:
		div_splitter()
			: division(kind::splitter, std::string()),
			splitter_cursor_(cursor::arrow),
			leaf_left_(nullptr), leaf_right_(nullptr),
			pause_move_collocate_(false)
		{
			this->weight.assign(4);
		}

		void leaf_left(division * d)
		{
			leaf_left_ = d;
		}

		void leaf_right(division * d)
		{
			leaf_right_ = d;
		}

		void direction(bool horizontal)
		{
			splitter_cursor_ = (horizontal ? cursor::size_we : cursor::size_ns);
		}
	private:
		void collocate(window wd) override
		{
			if (splitter_.empty())
			{
				splitter_.create(wd);
				splitter_.cursor(splitter_cursor_);

				dragger_.trigger(splitter_);
				splitter_.events().mouse_down.connect([this](const arg_mouse& arg)
				{
					if (false == arg.left_button)
						return;

					begin_point_ = splitter_.pos();

					auto px_ptr = &nana::rectangle::width;

					auto area_left = leaf_left_->margin_area();
					auto area_right = leaf_right_->margin_area();

					if (nana::cursor::size_we != splitter_cursor_)
					{
						left_pos_ = area_left.y;
						right_pos_ = area_right.bottom();
						px_ptr = &nana::rectangle::height;
					}
					else
					{
						left_pos_ = area_left.x;
						right_pos_ = area_right.right();
					}

					left_pixels_ = area_left.*px_ptr;
					right_pixels_ = area_right.*px_ptr;
				});

				splitter_.events().mouse_move.connect([this](const arg_mouse& arg)
				{
					if (false == arg.left_button)
						return;

					int delta = splitter_.pos().x - begin_point_.x;
					auto px_ptr = &nana::rectangle::width;
					if (nana::cursor::size_we != splitter_cursor_)
					{
						delta = splitter_.pos().y - begin_point_.y;
						px_ptr = &nana::rectangle::height;
					}

					int total_pixels = static_cast<int>(left_pixels_ + right_pixels_);

					int w = static_cast<int>(left_pixels_)+delta;
					if (w > total_pixels)
						w = total_pixels;
					else if (w < 0)
						w = 0;

					auto owner_area = div_owner->margin_area();
					leaf_left_->weight.assign_percent(100.0 * w / owner_area.*px_ptr);

					w = static_cast<int>(right_pixels_)-delta;
					if (w > total_pixels)
						w = total_pixels;
					else if (w < 0)
						w = 0;

					leaf_right_->weight.assign_percent(100.0 * w / owner_area.*px_ptr);

					pause_move_collocate_ = true;
					div_owner->collocate(splitter_.parent());
					pause_move_collocate_ = false;
				});
			}

			dragger_.target(splitter_, div_owner->margin_area(), (cursor::size_ns == splitter_cursor_ ? nana::arrange::vertical : nana::arrange::horizontal));

			if (false == pause_move_collocate_)
				splitter_.move(this->field_area);
		}
	private:
		nana::cursor	splitter_cursor_;
		place_parts::splitter<true>	splitter_;
		nana::point	begin_point_;
		division * leaf_left_;
		division * leaf_right_;
		int			left_pos_, right_pos_;
		unsigned	left_pixels_, right_pixels_;
		dragger	dragger_;
		bool	pause_move_collocate_;	//A flag represents whether do move when collocating.
	};

	place::implement::~implement()
	{
		API::umake_event(event_size_handle);
		delete root_division;
		for (auto & pair : fields)
		{
			delete pair.second;
		}
	}

	//search_div_name
	//search a division with the specified name.
	place::implement::division * place::implement::search_div_name(division* start, const std::string& name)
	{
		if (nullptr == start) return nullptr;

		if (start->name == name) return start;

		for (auto child : start->children)
		{
			division * div = search_div_name(child, name);
			if (div)
				return div;
		}
		return nullptr;
	}

	place::implement::division* place::implement::scan_div(tokenizer& tknizer)
	{
		typedef tokenizer::token token;

		division * div = nullptr;
		token div_type = token::eof;
		std::string name;

		number_t weight;
		place_parts::repeated_array arrange, gap;

		place_parts::margin margin;
		std::vector<number_t> array;

		std::vector<::nana::rectangle> collapses;

		std::vector<division*> children;
		for (token tk = tknizer.read(); tk != token::eof; tk = tknizer.read())
		{
			bool exit_for = false;
			switch (tk)
			{
			case token::splitter:
				if (!children.empty() && (division::kind::splitter != children.back()->kind_of_division))	//Ignore the splitter when there is not a division.
				{
					auto splitter = new div_splitter;

					splitter->leaf_left(children.back());
					children.back()->div_next = splitter;
					children.push_back(splitter);
				}
				break;
			case token::div_start:
			{
				auto div = scan_div(tknizer);

				if (children.size())
				{
					children.back()->div_next = div;
					if (division::kind::splitter == children.back()->kind_of_division)
						dynamic_cast<div_splitter&>(*children.back()).leaf_right(div);
				}
				children.push_back(div);
			}
				break;
			case token::vert:
				div_type = tk;
				break;
			case token::grid:
				div_type = tk;
				switch (tknizer.read())
				{
				case token::number:
					array.push_back(tknizer.number());
					array.push_back(tknizer.number());
					break;
				case token::array:
					tknizer.array().swap(array);
					break;
				case token::reparray:
					array.push_back(tknizer.reparray().at(0));
					array.push_back(tknizer.reparray().at(1));
					break;
				}
				break;
			case token::collapse:
				if (tknizer.parameters().size() == 4)
				{
					::nana::rectangle col;
					auto arg = tknizer.parameters().at(0);
					if (arg.kind_of() == number_t::kind::integer)
						col.x = arg.integer();
					else if (arg.kind_of() == number_t::kind::real)
						col.x = static_cast<int>(arg.real());
					else
						throw std::runtime_error("place: the type of the 1st parameter for collapse should be integer.");

					arg = tknizer.parameters().at(1);
					if (arg.kind_of() == number_t::kind::integer)
						col.y = arg.integer();
					else if (arg.kind_of() == number_t::kind::real)
						col.y = static_cast<int>(arg.real());
					else
						throw std::runtime_error("place: the type of the 2nd parameter for collapse should be integer.");

					arg = tknizer.parameters().at(2);
					if (arg.kind_of() == number_t::kind::integer)
						col.width = static_cast<decltype(col.width)>(arg.integer());
					else if (arg.kind_of() == number_t::kind::real)
						col.width = static_cast<decltype(col.width)>(arg.real());
					else
						throw std::runtime_error("place: the type of the 3rd parameter for collapse should be integer.");

					arg = tknizer.parameters().at(3);
					if (arg.kind_of() == number_t::kind::integer)
						col.height = static_cast<decltype(col.height)>(arg.integer());
					else if (arg.kind_of() == number_t::kind::real)
						col.height = static_cast<decltype(col.height)>(arg.real());
					else
						throw std::runtime_error("place: the type of the 4th parameter for collapse should be integer.");

					if (!col.empty_size() && (col.width > 1 || col.height > 1))
					{
						//Overwrite if a exist_col in collapses has same position as the col.
						for (auto & exist_col : collapses)
						{
							if (exist_col.x == col.x && exist_col.y == col.y)
							{
								exist_col = col;
								break;
							}
						}
						collapses.emplace_back(col);
					}
				}
				else
					throw std::runtime_error("place: collapse requires 4 parameters.");
				break;
			case token::weight:
				weight = tknizer.number();
				//If the weight is type of real, convert it to integer.
				//the integer and percent are allowed for weight.
				if (weight.kind_of() == number_t::kind::real)
					weight.assign(static_cast<int>(weight.real()));
				break;
			case token::arrange:
				arrange = tknizer.reparray();
				break;
			case token::gap:
				gap = tknizer.reparray();
				break;
			case token::margin:
				margin.clear();
				switch (tknizer.read())
				{
				case token::number:
					margin.set_value(tknizer.number());
					break;
				case token::array:
					margin.set_array(tknizer.array());
					break;
				case token::reparray:
					for (std::size_t i = 0; i < 4; ++i)
						margin.push(tknizer.reparray().at(i));
					break;
				default:
					break;
				}
				break;
			case token::div_end:
				exit_for = true;
				break;
			case token::identifier:
				name = tknizer.idstr();
				break;
			default:	break;
			}
			if (exit_for)
				break;
		}

		field_impl * field = nullptr;
		if (name.size())
		{
			//find the field with specified name.
			//the field may not be created.
			auto i = fields.find(name);
			if (fields.end() != i)
			{
				field = i->second;

				//the field is attached to a division, it means there is another division with same name.
				if (field->attached)
					throw std::runtime_error("place, the name '" + name + "' is redefined.");

				//this field will be attached to the division that will be created later.
				field->attached = true;
			}
		}

		switch (div_type)
		{
		case token::eof:
		case token::vert:
			div = new div_arrange(token::vert == div_type, std::move(name), std::move(arrange));
			break;
		case token::grid:
		{
			div_grid * p = new div_grid(std::move(name), std::move(arrange), std::move(collapses));

			if (array.size())
			{
				if (array[0].kind_of() != number_t::kind::percent)
					p->dimension.first = array[0].integer();
			}

			if (array.size() > 1)
			{
				if (array[1].kind_of() != number_t::kind::percent)
					p->dimension.second = array[1].integer();
			}

			if (0 == p->dimension.first)
				p->dimension.first = 1;

			if (0 == p->dimension.second)
				p->dimension.second = 1;

			div = p;
		}
			break;
		default:
			throw std::runtime_error("nana.place: invalid division type.");
		}

		div->weight = weight;
		div->gap = gap;
		div->field = field;		//attach the field to the division

		if (children.size() && (division::kind::splitter == children.back()->kind_of_division))
		{
			//Erase the splitter if the last one is a splitter.
			delete children.back();
			children.pop_back();

			if (children.size())
				children.back()->div_next = nullptr;
		}

		for (auto child : children)
		{
			child->div_owner = div;
			if (division::kind::splitter == child->kind_of_division)
			{
				dynamic_cast<div_splitter&>(*child).direction(div_type != token::vert);
			}
		}
		div->children.swap(children);
		div->margin = std::move(margin);
		return div;
	}

	//class place
	place::place()
		: impl_(new implement)
	{}

	place::place(window wd)
		: impl_(new implement)
	{
		bind(wd);
	}

	place::~place()
	{
		delete impl_;
	}

	void place::bind(window wd)
	{
		if (impl_->window_handle)
			throw std::runtime_error("place.bind: it has already binded to a window.");

		impl_->window_handle = wd;
		impl_->event_size_handle = API::events(wd).resized.connect([this](const arg_resized& arg)
		{
			if (impl_->root_division)
			{
				impl_->root_division->field_area = API::window_size(arg.window_handle);
				impl_->root_division->collocate(arg.window_handle);
			}
		});
	}

	window place::window_handle() const
	{
		return impl_->window_handle;
	}

	void place::div(const char* s)
	{
		delete impl_->root_division;
		impl_->root_division = nullptr;

		tokenizer tknizer(s);
		impl_->root_division = impl_->scan_div(tknizer);
	}

	place::field_reference place::field(const char* name)
	{
		name = name ? name : "";

		//check the name
		if (*name && (*name != '_' && !(('a' <= *name && *name <= 'z') || ('A' <= *name && *name <= 'Z'))))
			throw std::invalid_argument("place.field: bad field name");

		//get the field with specified name, if no such field with specified name
		//then create one.
		auto & p = impl_->fields[name];
		if (nullptr == p)
			p = new implement::field_impl(this);

		if ((false == p->attached) && impl_->root_division)
		{
			//search the division with the specified name,
			//and attached the division to the field
			implement::division * div = implement::search_div_name(impl_->root_division, name);
			if (div)
			{
				if (div->field && (div->field != p))
					throw std::runtime_error("nana.place: unexpected error, the division attachs a unexpected field.");

				div->field = p;
				p->attached = true;
			}
		}
		return *p;
	}

	void place::collocate()
	{
		if (impl_->root_division && impl_->window_handle)
		{
			impl_->root_division->field_area = API::window_size(impl_->window_handle);
			impl_->root_division->collocate(impl_->window_handle);

			for (auto & field : impl_->fields)
			{
				bool is_show = field.second->attached;
				if (is_show)
					is_show = (nullptr != implement::search_div_name(impl_->root_division, field.first));

				for (auto & el : field.second->elements)
					API::show_window(el.handle, is_show);
			}
		}
	}
	//end class place
}//end namespace nana
