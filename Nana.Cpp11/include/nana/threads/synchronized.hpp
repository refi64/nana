/*
 *	Synchronized Object
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://nanapro.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/threads/synchronized.h
 */

#ifndef NANA_THREADS_SYNCHRONIZED_HPP
#define NANA_THREADS_SYNCHRONIZED_HPP
#include <map>
#include "locks.hpp"

namespace nana
{
	namespace threads
	{
		template<typename Lock_Type>
		class basic_synchronized
		{
		public:
			typedef auto_locker auto_locker_type;
			typedef basic_synchronized this_type;
			typedef Lock_Type lock_type;
			typedef std::map<const void*, std::pair<lock_type*, unsigned> > container_type;

			template<typename Wait_Object_Type>
			basic_synchronized(const Wait_Object_Type& wait_object)
				:identifier_(&wait_object)
			{
				lock_type * lock = 0;
				
				{
					auto_locker_type al(total_lock_);
					container_type::iterator iter = locks_.find(identifier_);
					if(iter == locks_.end())
					{
						lock = new lock_type;	//throw std::bad_alloc???
						locks_[identifier_] = std::make_pair<lock_type*, unsigned>(lock, 1);
					}
					else
					{
						lock = iter->second.first;
						iter->second.second ++;
					}
				}
				lock->lock();
			}

			~basic_synchronized()
			{
				auto_locker_type al(total_lock_);
				container_type::iterator iter = locks_.find(identifier_);

				if(iter != locks_.end()) //??? is this if test useless?
				{
					lock_type* lock = iter->second.first;
					lock->unlock();
					if(--(iter->second.second) == 0)
					{
						locks_.erase(iter);
						delete lock;
					}
				}
			}
		private:
			const void* identifier_;
			static container_type locks_;
			static lock_type total_lock_;	//Total Lock synchronize the locks
		};

		template<typename Lock_Type>
		typename basic_synchronized<Lock_Type>::container_type basic_synchronized<Lock_Type>::locks_;
		
		template<typename Lock_Type>
		typename basic_synchronized<Lock_Type>::lock_type basic_synchronized<Lock_Type>::total_lock_;

		typedef basic_synchronized<token> synchronized;
	}	//end namespace threads
}	//end namespace nana
#endif

