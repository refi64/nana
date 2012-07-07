/*
 *	A textbase class implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/skeletons/textbase.hpp
 *	@description: This class manages the multi-line text and provides some operation on text
 */
 
#ifndef NANA_GUI_WIDGET_DETAIL_TEXTBASE_HPP
#define NANA_GUI_WIDGET_DETAIL_TEXTBASE_HPP
#include <string>
#include <deque>
#include <iostream>

namespace nana
{
namespace gui
{
namespace widgets
{
namespace skeletons
{
	template<typename CharT>
	class textbase
	{
	public:
		typedef CharT						char_type;
		typedef std::basic_string<CharT>	string_type;
		typedef typename string_type::size_type	size_type;

		textbase()
		{
			text_cont_.push_back(string_type());
		}

		bool empty() const
		{
			return (text_cont_.size() == 0 ||
					((text_cont_.size() == 1) && (text_cont_.begin()->size() == 0)));
		}

		size_type lines() const
		{
			return text_cont_.size();
		}

		size_type line_size(size_type line) const
		{
			if(line < text_cont_.size())
				return text_cont_[line].size();
			return 0;
		}
		
		string_type getline(size_type line) const
		{
			if(line < text_cont_.size())
				return text_cont_[line];
				
			return string_type();
		}

		std::pair<size_t, size_t> max_line() const
		{
			return std::make_pair(attr_max_.line, attr_max_.size);
		}
		
		friend std::ostream& operator<<(std::ostream& os, const textbase& tb)
		{
			const size_type lines = tb.lines();
			if(lines)
			{
				os<<tb.getline(0);
				for(size_type i = 1; i < lines; ++i)
				{
					os<<'\n'<<tb.getline(i);
				}
			}	
			return os;
		}
	public:
		void cover(size_type line, const char_type* text)
		{
			if(line < text_cont_.size())
				text_cont_[line] = text;
			else
			{
				text_cont_.push_back(text);
				line = text_cont_.size() - 1;
			}

			this->_m_make_max(line);
		}

		void insert(size_type line, size_type pos, const char_type* str)
		{
			if(line < text_cont_.size())
			{
				string_type& lnstr = text_cont_[line];
				
				if(pos < lnstr.size())
					lnstr.insert(pos, str);
				else
					lnstr += str;
			}
			else
			{
				text_cont_.push_back(string_type(str));
				line = text_cont_.size() - 1;
			}

			this->_m_make_max(line);
		}

		void insert(size_type line, size_type pos, char_type ch)
		{
			if(line < text_cont_.size())
			{
				string_type& lnstr = text_cont_[line];
				
				if(pos < lnstr.size())
					lnstr.insert(pos, 1, ch);
				else
					lnstr += ch;
			}
			else
			{
				text_cont_.push_back(string_type(1, ch));
				line = text_cont_.size() - 1;
			}

			this->_m_make_max(line);
		}

		void insertln(size_type line, const string_type& str)
		{
			if(line < text_cont_.size())
				text_cont_.insert(text_cont_.begin() + line, str);
			else
				text_cont_.push_back(str);

			_m_make_max(line);
		}

		void erase(size_type line, size_type pos, size_type count)
		{
			if(line < text_cont_.size())
			{
				string_type& lnstr = text_cont_[line];
				if((pos == 0) && (count >= lnstr.size()))
					lnstr.clear();
				else
					lnstr.erase(pos, count);

				if(attr_max_.line == line)
					this->_m_scan_for_max();
			}	
		}
		
		void erase(size_type line)
		{
			if(line < text_cont_.size())
				text_cont_.erase(text_cont_.begin() + line);

			if(line == attr_max_.line)
				_m_scan_for_max();
			else if(line < attr_max_.line)
				attr_max_.line--;
		}
		
		void erase_all()
		{
			std::deque<string_type>().swap(text_cont_);
			attr_max_.reset();
		}
		
		void merge(size_type line)
		{
			if(text_cont_.size() && (0 <= line) && line < text_cont_.size() - 1)
			{
				text_cont_[line] += text_cont_[line + 1];
				text_cont_.erase(text_cont_.begin() + line + 1);
				_m_make_max(line);
				if(line < attr_max_.line)
					attr_max_.line--;
			}
		}
	private:
		void _m_make_max(size_t line)
		{
			const string_type& str = text_cont_[line];
			if(str.size() > attr_max_.size)
			{
				attr_max_.size = str.size();
				attr_max_.line = line;
			}
		}

		void _m_scan_for_max()
		{
			attr_max_.size = 0;
			typename std::deque<string_type>::iterator it = text_cont_.begin(), end = text_cont_.end();
			size_t n = 0;
			for(; it != end; ++it, ++n)
			{
				if(it->size() > attr_max_.size)
				{
					attr_max_.size = it->size();
					attr_max_.line = n;
				}
			}
		}
		
	private:
		std::deque<string_type>	text_cont_;
		struct attr_max
		{
			attr_max()
				:line(0), size(0)
			{}

			size_t line;
			size_t size;

			void reset()
			{
				line = 0;
				size = 0;
			}
		}attr_max_;
	};

}//end namespace detail
}//end namespace widgets
}//end namespace gui
}//end namespace nana


#endif
