/*
 *	Locks Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: source/threads/locks.cpp
 */
#include <nana/config.hpp>
#include <nana/threads/locks.hpp>
#if defined(NANA_WINDOWS)
	#define _WIN32_WINNT 0x0400
	#include <windows.h>
#elif defined(NANA_LINUX)
	#include <pthread.h>
	#include <time.h>
#endif

namespace nana
{
	namespace threads
	{
		namespace detail
		{
			superlock::~superlock()
			{}
		}

		//class token
			struct token::impl_t
			{
#if defined(NANA_WINDOWS)
				::CRITICAL_SECTION lock_obj;

#elif defined(NANA_LINUX)
				pthread_mutex_t mtx;
#endif
				impl_t()
				{
#if defined(NANA_WINDOWS)
					::InitializeCriticalSection(&lock_obj);
#elif defined(NANA_LINUX)
					pthread_mutex_t dup = PTHREAD_MUTEX_INITIALIZER;
					mtx = dup;
					pthread_mutexattr_t attr;
					pthread_mutexattr_init(&attr);
					pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
					pthread_mutex_init(&mtx, &attr);
					pthread_mutexattr_destroy(&attr);
#endif
				}

				~impl_t()
				{
#if defined(NANA_WINDOWS)
					::DeleteCriticalSection(&lock_obj);
#elif defined(NANA_LINUX)
					pthread_mutex_destroy(&mtx);
#endif
				}
			};

			token::token()
				: impl_(new impl_t)
			{
			}

			token::~token()
			{
				delete impl_;
			}

			bool token::lock() const volatile
			{
#if defined(NANA_WINDOWS)
				::EnterCriticalSection(&(impl_->lock_obj));
#elif defined(NANA_LINUX)
				pthread_mutex_lock(&(impl_->mtx));
#endif
				return true;
			}

			bool token::try_lock() const volatile
			{
#if defined(NANA_WINDOWS)
				return (0 != ::TryEnterCriticalSection(&(impl_->lock_obj)));
#elif defined(NANA_LINUX)
				return (0 == pthread_mutex_trylock(&(impl_->mtx)));
#endif
			}

			void token::unlock() const volatile
			{
#if defined(NANA_WINDOWS)
				::LeaveCriticalSection(&(impl_->lock_obj));
#elif defined(NANA_LINUX)
				pthread_mutex_unlock(&(impl_->mtx));
#endif
			}
		//end class token

		//class mutex
			struct mutex::impl_t
			{
#if defined(NANA_WINDOWS)
				HANDLE object;
#elif defined(NANA_LINUX)
				pthread_mutex_t mtx;
#endif
				unsigned long time;
			};

			mutex::mutex()
				: impl_(new impl_t)
			{
				impl_->time = tick_infinite;
#if defined(NANA_WINDOWS)
				impl_->object = ::CreateMutex(0, 0, 0);
#elif defined(NANA_LINUX)
				pthread_mutex_t dup = PTHREAD_MUTEX_INITIALIZER;
				impl_->mtx = dup;
				pthread_mutex_init(&(impl_->mtx), 0);
#endif
			}

			mutex::~mutex()
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						::CloseHandle(impl_->object);
#elif defined(NANA_LINUX)
					pthread_mutex_destroy(&(impl_->mtx));
#endif
					delete impl_;
				}
			}
			
			bool mutex::lock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						return (WAIT_TIMEOUT != ::WaitForSingleObject(impl_->object, impl_->time));
#elif defined(NANA_LINUX)
					if(impl_->time != tick_infinite)
					{
						timespec abs;
						clock_gettime(CLOCK_REALTIME, &abs);
						abs.tv_nsec += impl_->time;
						return (0 == pthread_mutex_timedlock(&(impl_->mtx), &abs));
					}
					pthread_mutex_lock(&(impl_->mtx));
					return true;
#endif
				}
				return false;
			}

			bool mutex::try_lock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						return (WAIT_TIMEOUT != ::WaitForSingleObject(impl_->object, 1));
#elif defined(NANA_LINUX)
					return (0 == pthread_mutex_trylock(&(impl_->mtx)));
#endif
				}
				return false;
			}

			void mutex::unlock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						::ReleaseMutex(impl_->object);
#elif defined(NANA_LINUX)
					pthread_mutex_unlock(&(impl_->mtx));
#endif
				}
			}

			void mutex::time(unsigned milliseconds) volatile
			{
				if(impl_)
					impl_->time = milliseconds;
			}

			unsigned mutex::time() const volatile
			{
				return (impl_ ? impl_->time : 0);
			}
		//end class mutex

		//class condition
			struct condition::impl_t
			{
#if defined(NANA_WINDOWS)
				HANDLE object;
#elif defined(NANA_LINUX)
				pthread_mutex_t obj;
#endif
				unsigned long time;
			};

			condition::condition()
				: impl_(new impl_t)
			{
				impl_->time = tick_infinite;
#if defined(NANA_WINDOWS)
				impl_->object = ::CreateEvent(0, false, true, 0);
#elif defined(NANA_LINUX)
				pthread_mutex_t dup = PTHREAD_MUTEX_INITIALIZER;
				impl_->obj = dup;
				pthread_mutex_init(&(impl_->obj), 0);				
#endif
			}

			condition::~condition()
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						::CloseHandle(impl_->object);
#elif defined(NANA_LINUX)
					pthread_mutex_destroy(&(impl_->obj));
#endif
					delete impl_;
				}
			}

			bool condition::lock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						return (WAIT_TIMEOUT != ::WaitForSingleObject(impl_->object, impl_->time));
#elif defined(NANA_LINUX)
					if(impl_->time != tick_infinite)
					{
						timespec abs;
						clock_gettime(CLOCK_REALTIME, &abs);
						abs.tv_nsec += impl_->time;
						return (0 == pthread_mutex_timedlock(&(impl_->obj), &abs));
					}
					pthread_mutex_lock(&(impl_->obj));
					return true;
#endif
				}
				return false;
			}

			bool condition::try_lock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						return (WAIT_TIMEOUT != ::WaitForSingleObject(impl_->object, 1));
#elif defined(NANA_LINUX)
					return (0 == pthread_mutex_trylock(&(impl_->obj)));
#endif
				}
				return false;
			}

			bool condition::lock(unsigned milliseconds) const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						return (::WaitForSingleObject(impl_->object, milliseconds) != WAIT_TIMEOUT);
#elif defined(NANA_LINUX)
					if(milliseconds != tick_infinite)
					{
						timespec abs;
						clock_gettime(CLOCK_REALTIME, &abs);
						abs.tv_nsec += milliseconds;
						return (0 == pthread_mutex_timedlock(&(impl_->obj), &abs));
					}
					pthread_mutex_lock(&(impl_->obj));
					return true;
#endif
				}
				return false;
			}

			void condition::unlock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->object)
						::SetEvent(impl_->object);
#elif defined(NANA_LINUX)
					pthread_mutex_unlock(&(impl_->obj));
#endif
				}
			}

			void condition::time(unsigned milliseconds) volatile
			{
				if(impl_)
					impl_->time = milliseconds;
			}

			unsigned condition::time() const volatile
			{
				return (impl_ ? impl_->time : 0);
			}
		//end class condition
	}	//end namespace threads
}	//end namespace nana

