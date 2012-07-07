/*
 *	A Date Time Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/datetime.hpp
 */

#ifndef NANA_DATETIME_HPP
#define NANA_DATETIME_HPP

namespace nana
{
	class date
	{
	public:
		struct value
		{
			unsigned year;	//1601 - 30827
			unsigned month;	//1-12
			unsigned day;	//1-31
		};

		date();
		date(int year, int month, int day);

		date operator - (int off) const;
		date operator + (int off) const;
		bool operator==(const date&) const;
		bool operator!=(const date&) const;
		bool operator<(const date&) const;
		bool operator>(const date&) const;
		bool operator<=(const date&) const;
		bool operator>=(const date&) const;

		int day_of_week() const;
		const value & read() const;

		static int day_of_week(int year, int month, int day);
		static unsigned year_days(unsigned year);
		static unsigned month_days(unsigned year, unsigned month);
		static unsigned day_in_year(unsigned y, unsigned m, unsigned d);
	private:
		date _m_add(unsigned x) const;
		date _m_sub(unsigned x) const;
	private:
		value	value_;
	}; //end namespace date
}//end namespace nana

#endif
