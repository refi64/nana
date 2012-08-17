/*
 *	Locks Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/threads/locks.h
 */

#ifndef NANA_THREADS_LOCKS_HPP
#define NANA_THREADS_LOCKS_HPP

#include "../traits.hpp"
#include <cstddef>

namespace nana
{
	namespace threads
	{
		namespace detail
		{
			class superlock
				:private noncopyable
			{
			public:
				virtual ~superlock() = 0;
				virtual bool lock() const volatile = 0;
				virtual bool try_lock() const volatile = 0;
				virtual void unlock() const volatile = 0;
				virtual void * native_handle() const volatile = 0;
			};
		}//end namespace detail

		//class scope_guard
		//@brief:	It locks a specified lock while it is initializing and
		//	unlocks the lock while it is desconstructing
		class scope_guard
			: private noncopyable
		{
		public:
			//Construct an scope_guard with a lock
			template<typename Lock_Type>
			explicit scope_guard(Lock_Type& lock)
				:lock_(lock)
			{
				lock.lock();
			}

			//Destroy the scope_guard instance
			~scope_guard()
			{
				lock_.unlock();
			}
		private: //Private data member
			const volatile detail::superlock& lock_;	//Handle of a lock
		};

		enum
		{
			tick_infinite = 0xFFFFFFFF
		};

		class token
			: public detail::superlock
		{
		public:
			token();
			~token();

			bool lock() const volatile;
			bool try_lock() const volatile;
			void unlock() const volatile;
			void * native_handle() const volatile;
		private:
			struct impl_t;
			impl_t * impl_;
		};

		class mutex
			: public detail::superlock
		{
		public:
			mutex();
			~mutex();
			bool lock() const volatile;
			bool try_lock() const volatile;
			void unlock() const volatile;

			void time(unsigned milliseconds) volatile;
			unsigned time() const volatile;

			void * native_handle() const volatile;
		private:
			struct impl_t;
			impl_t * impl_;
		};

		class condition
		{
		public:
			typedef detail::superlock lock_type;
			condition();
			~condition();

			void wait(volatile lock_type& ) volatile;
			bool wait_for(volatile lock_type&, std::size_t milliseconds) volatile;
			void signal() volatile;
		private:
			struct impl_t;
			impl_t * impl_;
		};
	}	//end namespace threads
}	//end namespace nana
#endif

