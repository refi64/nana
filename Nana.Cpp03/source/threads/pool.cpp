/*
 *	A Thread Pool Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *
 *	@file: nana/threads/pool.cpp
 */

#include <nana/threads/pool.hpp>
#include <time.h>
#include <nana/system/platform.hpp>
#if defined(NANA_WINDOWS)
	#include <windows.h>
	#include <process.h>
#elif defined(NANA_LINUX)
	#include <pthread.h>
	#include <nana/threads/locks.hpp>
#endif

namespace nana
{
namespace threads
{
	namespace detail
	{
#if defined(NANA_WINDOWS)
		typedef HANDLE thread_t;
#elif defined(NANA_LINUX)
		typedef pthread_t thread_t;
#endif
		//begin class task_object
		task_object::task_object(task_object::kind tsk)
			: tsk_kind(tsk)
		{}

		task_object::~task_object(){}
		//end class task_object

		class task_signal
			: public task_object
		{
		public:
			task_signal(): task_object(task_object::signal)
			{}

			void exec(){}
		};

		struct pool_thread_object
		{
			enum state_t
			{
				stop, run
			};

			volatile threads::pool * owner;
			unsigned (*routine)(pool_thread_object*);
			thread_t handle;
			unsigned tid;
			state_t state;
			time_t task_timestamp;
#ifdef NANA_LINUX
			nana::threads::mutex suspend_mutex;
#endif
		};

#if defined(NANA_WINDOWS)
		typedef unsigned (__stdcall* thread_starter_t)(void*);
		unsigned __stdcall thread_starter(pool_thread_object* pto)
		{
			unsigned r = pto->routine(pto);
			::_endthreadex(r);
			return r;
		}
#elif defined(NANA_LINUX)
		typedef void* (* thread_starter_t)(void*);
		void* thread_starter(pool_thread_object* pto)
		{
			return reinterpret_cast<void*>(pto->routine(pto));
		}
#endif
	}//end namespace detail

	//begin class pool
		pool::pool()
			:tpool_running_(true)
		{
			_m_start_threads(0);
		}

		pool::pool(unsigned tsize)
			:tpool_running_(true)
		{
			_m_start_threads(tsize);
		}

		pool::~pool()
		{
			close();
			nana::threads::scope_guard sg(locks_.task);
			for(std::vector<detail::task_object*>::iterator i = task_container_.begin(); i != task_container_.end(); ++i)
				delete *i;
		}

		void pool::close() volatile
		{
			pool * self = const_cast<pool*>(this);
			std::vector<detail::pool_thread_object*> dup;

			{
				threads::scope_guard sg(locks_.thread);
				tpool_running_ = false;

				std::vector<detail::pool_thread_object*>::iterator i = self->threads_.begin(), end = self->threads_.end();
				for(; i != end; ++i)
				{
					if((*i)->state == detail::pool_thread_object::stop)
					{
#if defined(NANA_WINDOWS)
						while(true)
						{
							unsigned long susp_n = ::ResumeThread((*i)->handle);
							if(susp_n == 1 || susp_n == static_cast<unsigned long>(-1))
								break;
						}
#elif defined(NANA_LINUX)
						(*i)->suspend_mutex.unlock();
#endif
					}
				}
				dup.swap(self->threads_);
			}

			for(std::vector<detail::pool_thread_object*>::iterator i = dup.begin(); i != dup.end(); ++i)
			{
#if defined(NANA_WINDOWS)
				::WaitForSingleObject((*i)->handle, INFINITE);
				::CloseHandle((*i)->handle);
#elif defined(NANA_LINUX)
				::pthread_join((*i)->handle, 0);
				::pthread_detach((*i)->handle);
#endif
				delete (*i);
			}
		}

		void pool::signal() volatile
		{
			detail::task_object* tkob = 0;
			try
			{
				tkob = new detail::task_signal;
				_m_push(tkob);
			}
			catch(std::bad_alloc&)
			{
				delete tkob;
			}
		}

		void pool::wait_for_signal() volatile
		{
			nana::threads::scope_guard lock(locks_.signal);
			locks_.signal_cond.wait(locks_.signal);
		}

		void pool::_m_push(detail::task_object* task) volatile
		{
			nana::threads::scope_guard sg(locks_.task);
			const_cast<pool*>(this)->task_container_.push_back(task);
			_m_wakeup();
		}

		void pool::_m_start_threads(unsigned size) volatile
		{
			close();

			tpool_running_ = true;
			if(size == 0)	size = 4;

			for(unsigned i = 0; i < size; ++i)
			{
				detail::pool_thread_object * pto = new detail::pool_thread_object;
				pto->owner = this;
				pto->state = detail::pool_thread_object::stop;
				pto->routine = pool::_m_job_routine;
				const_cast<pool*>(this)->threads_.push_back(pto);
#if defined(NANA_WINDOWS)
				pto->handle = (HANDLE)::_beginthreadex(0, 0, reinterpret_cast<unsigned(__stdcall*)(void*)>(&self_t::_m_job_routine), pto, 0, &(pto->tid));
#elif defined(NANA_LINUX)
				::pthread_create(&(pto->handle), 0, reinterpret_cast<detail::thread_starter_t>(detail::thread_starter), pto);
#endif
			}
		}

		void pool::_m_wakeup() volatile
		{
			detail::pool_thread_object * pto = 0;
			{
				pool * self = const_cast<pool*>(this);
				nana::threads::scope_guard sg(locks_.thread);
				if(tpool_running_)
				{
					std::vector<detail::pool_thread_object*>::iterator i = self->threads_.begin(), end = self->threads_.end();
					for(; i != end; ++i)
					{
						if((*i)->state == detail::pool_thread_object::stop)	//sleep
						{
							pto = *i;
							pto->state = detail::pool_thread_object::run;
							break;
						}
					}
				}
			}

			if(pto && pto->handle)
			{
#if defined(NANA_WINDOWS)
				while(true)
				{
					unsigned long susp_n = ::ResumeThread(pto->handle);
					if(susp_n == 1)
						break;
					else if(susp_n == static_cast<unsigned long>(-1))
					{
						//For debug
						susp_n = ::GetLastError();
						susp_n = 0;
						break;
					}
				}
#elif defined(NANA_LINUX)
				pto->suspend_mutex.unlock();
#endif
			}
		}

		void pool::_m_job_suspend(detail::pool_thread_object * thr) volatile
		{
			{
				//lock threads
				nana::threads::scope_guard sg(locks_.thread);

				//when the pool is closing, pool should not suspend this job routine,
				//if the routine is continue, it will close automatically.
				if(tpool_running_)
					thr->state = detail::pool_thread_object::stop;
				else
					thr = 0;
			}

			if(thr && thr->handle)
			{
#if defined(NANA_WINDOWS)
				::SuspendThread(thr->handle);
#elif defined(NANA_LINUX)
				thr->suspend_mutex.lock();
#endif
			}
		}

		detail::task_object* pool::_m_job_read_task(detail::pool_thread_object * thr) volatile
		{
			pool * self = const_cast<pool*>(this);
			while(tpool_running_)
			{
				detail::task_object* task = 0;

				{
					nana::threads::scope_guard sg(locks_.task);

					if(self->task_container_.size())
					{
						task = *(self->task_container_.begin());
						self->task_container_.erase(self->task_container_.begin());
					}
				}

				if(task)
				{
					thr->task_timestamp = time(0);
					return task;
				}
				else
					_m_job_suspend(thr);
			}

			return 0;
		}

		unsigned pool::_m_job_routine(detail::pool_thread_object * thr)
		{
			nana::threads::pool * owner = const_cast<pool*>(thr->owner);
#if defined(NANA_WINDOWS)
			::SuspendThread(::GetCurrentThread());
#elif defined(NANA_LINUX)
			thr->suspend_mutex.lock();
#endif
			while(owner->tpool_running_)
			{
				detail::task_object* task = owner->_m_job_read_task(thr);
				if(task)
				{
					switch(task->tsk_kind)
					{
					case detail::task_object::general:
						try
						{
							task->exec();
						}catch(...){}
						break;
					case detail::task_object::signal:
						while(true)
						{
							bool await = false;
							owner->locks_.thread.lock();
							for(std::vector<detail::pool_thread_object*>::iterator i = owner->threads_.begin(); i != owner->threads_.end(); ++i)
							{
								detail::pool_thread_object* th = *i;
								if((th != thr) && (th->state == th->run) && (th->task_timestamp <= thr->task_timestamp))
								{
									await = true;
									break;
								}
							}
							owner->locks_.thread.unlock();
							if(await)
								nana::system::sleep(300);
							else
								break;
						}
						owner->locks_.signal_cond.signal();
						break;
					}

					delete task;
				}
				else
					break;

			}
			return 0;
		}
	//end class pool

}//end namespace threads
}//end namespace nana
