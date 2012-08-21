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
	#include <sys/time.h>
	#include <errno.h>
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
				pthread_mutex_t lock_obj;
#endif
				impl_t()
				{
#if defined(NANA_WINDOWS)
					::InitializeCriticalSection(&lock_obj);
#elif defined(NANA_LINUX)
					pthread_mutex_t dup = PTHREAD_MUTEX_INITIALIZER;
					lock_obj = dup;
					pthread_mutexattr_t attr;
					pthread_mutexattr_init(&attr);
					pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
					pthread_mutex_init(&lock_obj, &attr);
					pthread_mutexattr_destroy(&attr);
#endif
				}

				~impl_t()
				{
#if defined(NANA_WINDOWS)
					::DeleteCriticalSection(&lock_obj);
#elif defined(NANA_LINUX)
					pthread_mutex_destroy(&lock_obj);
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
				pthread_mutex_lock(&(impl_->lock_obj));
#endif
				return true;
			}

			bool token::try_lock() const volatile
			{
#if defined(NANA_WINDOWS)
				return (0 != ::TryEnterCriticalSection(&(impl_->lock_obj)));
#elif defined(NANA_LINUX)
				return (0 == pthread_mutex_trylock(&(impl_->lock_obj)));
#endif
			}

			void token::unlock() const volatile
			{
#if defined(NANA_WINDOWS)
				::LeaveCriticalSection(&(impl_->lock_obj));
#elif defined(NANA_LINUX)
				pthread_mutex_unlock(&(impl_->lock_obj));
#endif
			}

			void * token::native_handle() const volatile
			{
				return &impl_->lock_obj;
			}
		//end class token

		//class mutex
			struct mutex::impl_t
			{
#if defined(NANA_WINDOWS)
				HANDLE lock_handle;
#elif defined(NANA_LINUX)
				pthread_mutex_t lock_obj;
#endif
				unsigned long time;
			};

			mutex::mutex()
				: impl_(new impl_t)
			{
				impl_->time = tick_infinite;
#if defined(NANA_WINDOWS)
				impl_->lock_handle = ::CreateMutex(0, 0, 0);
#elif defined(NANA_LINUX)
				pthread_mutex_t dup = PTHREAD_MUTEX_INITIALIZER;
				impl_->lock_obj = dup;
				pthread_mutex_init(&(impl_->lock_obj), 0);
#endif
			}

			mutex::~mutex()
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
						::CloseHandle(impl_->lock_handle);
#elif defined(NANA_LINUX)
					pthread_mutex_destroy(&(impl_->lock_obj));
#endif
					delete impl_;
				}
			}
			
			bool mutex::lock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
						return (WAIT_TIMEOUT != ::WaitForSingleObject(impl_->lock_handle, impl_->time));
#elif defined(NANA_LINUX)
					if(impl_->time != tick_infinite)
					{
						timespec abs;
						clock_gettime(CLOCK_REALTIME, &abs);
						abs.tv_nsec += impl_->time;
						return (0 == pthread_mutex_timedlock(&(impl_->lock_obj), &abs));
					}
					pthread_mutex_lock(&(impl_->lock_obj));
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
					if(impl_->lock_handle)
						return (WAIT_TIMEOUT != ::WaitForSingleObject(impl_->lock_handle, 1));
#elif defined(NANA_LINUX)
					return (0 == pthread_mutex_trylock(&(impl_->lock_obj)));
#endif
				}
				return false;
			}

			void mutex::unlock() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
						::ReleaseMutex(impl_->lock_handle);
#elif defined(NANA_LINUX)
					pthread_mutex_unlock(&(impl_->lock_obj));
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

			void * mutex::native_handle() const volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					return impl_->lock_handle;
#elif defined(NANA_LINUX)
					return &impl_->lock_obj;
#endif
				}
				return 0;
			}
		//end class mutex

		//class condition
			struct condition::impl_t
			{
#if defined(NANA_WINDOWS)
				HANDLE lock_handle;
#elif defined(NANA_LINUX)
				pthread_cond_t lock_obj;
#endif
				unsigned long time;
			};

			condition::condition()
				: impl_(new impl_t)
			{
				impl_->time = tick_infinite;
#if defined(NANA_WINDOWS)
				impl_->lock_handle = ::CreateEvent(0, false, false, 0);
#elif defined(NANA_LINUX)
				pthread_cond_t dup = PTHREAD_COND_INITIALIZER;
				impl_->lock_obj = dup;			
#endif
			}

			condition::~condition()
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
						::CloseHandle(impl_->lock_handle);
#elif defined(NANA_LINUX)
					pthread_cond_destroy(&(impl_->lock_obj));
#endif
					delete impl_;
				}
			}

			void condition::wait(volatile lock_type& lck) volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
					{
						lck.unlock();
						::WaitForSingleObject(impl_->lock_handle, INFINITE);
						lck.lock();
					}
#elif defined(NANA_LINUX)
					::pthread_cond_wait(&(impl_->lock_obj), reinterpret_cast<pthread_mutex_t*>(lck.native_handle()));
#endif
				}
			}

			bool condition::wait_for(volatile lock_type & lck, std::size_t milliseconds) volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
					{
						lck.unlock();
						bool is_timeout = (WAIT_TIMEOUT == ::WaitForSingleObject(impl_->lock_handle, static_cast<unsigned long>(milliseconds)));
						lck.lock();
						return is_timeout;
					}
#elif defined(NANA_LINUX)
					struct timeval now;
					struct timespec tm;
					::gettimeofday(&now, 0);
					tm.tv_nsec = now.tv_usec * 1000 + (milliseconds % 1000) * 1000000;
					tm.tv_sec = now.tv_sec + milliseconds / 1000 + tm.tv_nsec / 1000000000;
					tm.tv_nsec %= 1000000000;
					return (ETIMEDOUT == ::pthread_cond_timedwait(&impl_->lock_obj, reinterpret_cast<pthread_mutex_t*>(lck.native_handle()), &tm));
#endif
				}
				return false;
			}

			void condition::signal() volatile
			{
				if(impl_)
				{
#if defined(NANA_WINDOWS)
					if(impl_->lock_handle)
						::SetEvent(impl_->lock_handle);
#elif defined(NANA_LINUX)
					::pthread_cond_signal(&impl_->lock_obj);
#endif
				}
			}
		//end class condition
	}	//end namespace threads
}	//end namespace nana

