/*
 *	A Thread Pool Implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0.
 *	(See accompanying file LICENSE_1_0.txt or copy at
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *
 *	@file: nana/threads/pool.hpp
 */

#ifndef NANA_THREADS_POOL_HPP
#define NANA_THREADS_POOL_HPP

#include <vector>
#include "locks.hpp"
#include <nana/functor.hpp>


namespace nana
{
	namespace threads
	{

		namespace detail
		{
			class task_object
			{
			public:
				enum kind{general, signal};

				task_object(kind);
				virtual ~task_object();
				virtual void exec() = 0;
			public:
				const kind tsk_kind;
			};	//end class task_object

			template<typename Functor>
			class task
				:public task_object
			{
				task(const task&);
				task& operator=(const task&);

			public:
				typedef Functor	functor_type;

				task(const functor_type& functor)
					:	task_object(task::general), functor_(functor)
				{}

				virtual void exec()
				{
					functor_();
				}
			private:
				Functor functor_;
			};//end class task

			struct pool_thread_object;

		}//end namespace detail

		class pool
		{
			typedef pool self_t;
		public:
			pool();
			pool(unsigned tsize);
			~pool();

			void close() volatile;

			template<typename Function>
			void push(const Function& f) volatile
			{
				detail::task_object* tkob = 0;

				try
				{
					tkob = new detail::task<typename nana::metacomp::static_if<typename nana::traits::is_function_type<Function>::value_type, Function*, Function>::value_type>(f);
					_m_push(tkob);
				}
				catch(std::bad_alloc&)
				{
					delete tkob;
				}
			}

			void signal() volatile;
			void wait_for_signal() volatile;
		private:
			void _m_push(detail::task_object*) volatile;
			void _m_start_threads(unsigned size) volatile;
			void _m_wakeup() volatile;
		private:
			//These methods only can be called by job routine thread.
			void _m_job_suspend(detail::pool_thread_object*) volatile;
			detail::task_object* _m_job_read_task(detail::pool_thread_object*) volatile;
			static unsigned _m_job_routine(detail::pool_thread_object*);
		private:
			volatile bool	tpool_running_;

			std::vector<detail::pool_thread_object*>	threads_;
			std::vector<detail::task_object*>	task_container_;

			struct locks_tag
			{
				threads::token thread;
				threads::token task;
				threads::condition signal;
			}locks_;
		};

		template<typename Function>
		class pool_pusher
		{
		public:
			typedef typename nana::metacomp::static_if<typename nana::traits::is_function_type<Function>::value_type, Function*, Function>::value_type value_type;

			pool_pusher(pool& pobj, value_type fn)
				:pobj_(pobj), value_(fn)
			{}

			void operator()() const
			{
				pobj_.push(value_);
			}
		private:
			pool & pobj_;
			value_type value_;
		};

		template<typename Function>
		pool_pusher<Function> pool_push(pool& pobj, const Function& fn)
		{
			return pool_pusher<Function>(pobj, fn);
		}

		template<typename Class, typename Concept>
		pool_pusher<functor<void()> > pool_push(pool& pobj, Class& obj, void(Concept::*mf)())
		{
			return pool_pusher<functor<void()> >(pobj, functor<void()>(obj, mf));
		}
	}//end namespace threads
}//end namespace nana
#endif

