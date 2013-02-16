/*
 *	A textbase class implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://nanapro.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/gui/widgets/skeletons/textbase.hpp
 *	@description: This class manages the multi-line text and provides some operation on text
 */
 
#ifndef NANA_GUI_WIDGET_DETAIL_TEXTBASE_HPP
#define NANA_GUI_WIDGET_DETAIL_TEXTBASE_HPP
#include <string>
#include <deque>
#include <memory>
#include <fstream>
#include <nana/charset.hpp>

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

		void load(const char* tfs)
		{
			text_cont_.clear();
			attr_max_.reset();

			std::ifstream ifs(tfs);
			std::string str;
			std::size_t lines = 0;
			while(ifs.good())
			{
				std::getline(ifs, str);
				text_cont_.push_back(nana::charset(str));
				if(text_cont_.back().size() > attr_max_.size)
				{
					attr_max_.size = text_cont_.back().size();
					attr_max_.line = lines;
				}
				++lines;
			}
		}

		void store(const char* tfs) const
		{
			std::ofstream ofs(tfs, std::ios::binary);
			if(ofs && text_cont_.size())
			{
				if(text_cont_.size() > 1)
				{
					for(auto i = text_cont_.begin(), end = text_cont_.end() - 1; i != end; ++i)
					{
						std::string mbs = nana::charset(*i);
						ofs.write(mbs.c_str(), mbs.size());
						ofs.write("\r\n", 2);
					}
				}
				std::string mbs = nana::charset(text_cont_.back());
				ofs.write(mbs.c_str(), mbs.size());
			}
		}

		void store(const char* tfs, nana::unicode encoding) const
		{
			std::ofstream ofs(tfs, std::ios::binary);
			if(ofs && text_cont_.size())
			{
				const char * le_boms[] = {"\xEF\xBB\xBF", "\xFF\xFE", "\xFF\xFE\x0\x0"};	//BOM for little-endian
				int bytes = 0;
				switch(encoding)
				{
				case nana::unicode::utf8:
					bytes = 3;	break;
				case nana::unicode::utf16:
					bytes = 2;	break;
				case nana::unicode::utf32:
					bytes = 4;	break;
				}

				if(bytes)
					ofs.write(le_boms[static_cast<int>(encoding)], bytes);

				if(text_cont_.size() > 1)
				{
					std::string mbs;
					for(auto i = text_cont_.begin(), end = text_cont_.end() - 1; i != end; ++i)
					{
						mbs = nana::charset(*i).to_bytes(encoding);
						mbs += "\r\n";
						ofs.write(mbs.c_str(), static_cast<std::streamsize>(mbs.size()));
					}
				}
				std::string mbs = nana::charset(text_cont_.back()).to_bytes(encoding);
				ofs.write(mbs.c_str(), static_cast<std::streamsize>(mbs.size()));
			}		
		}

		size_type lines() const
		{
			return text_cont_.size();
		}
		
		const string_type& getline(size_type pos) const
		{
			if(pos < text_cont_.size())
				return text_cont_[pos];

			if(nullptr == nullstr_)
				nullstr_ = std::shared_ptr<string_type>(new string_type);
			return *nullstr_;
		}

		std::pair<size_t, size_t> max_line() const
		{
			return std::make_pair(attr_max_.line, attr_max_.size);
		}
	public:
		void cover(size_type pos, const char_type* text)
		{
			if(text_cont_.size() <= pos)
			{
				text_cont_.push_back(text);
				pos = text_cont_.size() - 1;
			}
			else
				text_cont_[pos] = text;

			_m_make_max(pos);
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
					_m_scan_for_max();
			}	
		}
		
		void erase(size_type pos)
		{
			if(pos < text_cont_.size())
				text_cont_.erase(text_cont_.begin() + pos);

			if(pos == attr_max_.line)
				_m_scan_for_max();
			else if(pos < attr_max_.line)
				attr_max_.line--;
		}
		
		void erase_all()
		{
			std::deque<string_type>().swap(text_cont_);
			attr_max_.reset();
		}
		
		void merge(size_type pos)
		{
			if(text_cont_.size() && (pos < text_cont_.size() - 1))
			{
				text_cont_[pos] += text_cont_[pos + 1];
				text_cont_.erase(text_cont_.begin() + pos + 1);
				_m_make_max(pos);
				if(pos < attr_max_.line)
					--attr_max_.line;
			}
		}
	private:
		void _m_make_max(std::size_t pos)
		{
			const string_type& str = text_cont_[pos];
			if(str.size() > attr_max_.size)
			{
				attr_max_.size = str.size();
				attr_max_.line = pos;
			}
		}

		void _m_scan_for_max()
		{
			attr_max_.size = 0;
			std::size_t n = 0;
			for(auto & s : text_cont_)
			{
				if(s.size() > attr_max_.size)
				{
					attr_max_.size = s.size();
					attr_max_.line = n;
				}
				++n;
			}
		}
		
	private:
		std::deque<string_type>	text_cont_;
		mutable std::shared_ptr<string_type> nullstr_;
		struct attr_max
		{
			attr_max()
				:line(0), size(0)
			{}

			std::size_t line;
			std::size_t size;

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
