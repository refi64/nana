/*
 *	Refer Smart Pointer Implementation
 *	Copyright(C) 2003-2013 Jinhao(cnjinhao@hotmail.com)
 *
 *	@file: nana/refer.h
 */

#ifndef NANA_REFER_H
#define NANA_REFER_H
#include "traits.hpp"
#include "exceptions.hpp"

namespace nana
{
	template<typename ValueType, typename DeleterType = nana::null_type>
	class refer
	{
		typedef refer this_type;
	public:
		typedef ValueType value_type;
		typedef DeleterType deleter_type;
	public:
		typedef typename metacomp::rm_ref<value_type>::value_type& reference;
		typedef const typename metacomp::rm_ref<value_type>::value_type& const_reference;

		refer():ref_(0){}

		refer(const_reference t)
			: ref_(new ref_object_helper<value_type, deleter_type>(t))
		{}

		refer(const this_type& rhs)
			: ref_(rhs.ref_)
		{
			if(ref_)
				ref_->count++; 
		}
        
		this_type& operator=(const this_type& rhs)
		{
			if(this == &rhs)	return *this;

			if(&rhs.ref_ != &ref_)
			{
				release();
				if(rhs.ref_)
				{
					ref_ = rhs.ref_;
					ref_->count++;
				}
			}
			return *this;
		}
        
		~refer()
		{
			release();
		}
	public:
		void release()
		{
			if(ref_ && (ref_->count -- <= 1))
				delete ref_;
			ref_ = 0;
		}

		this_type& operator=(const_reference vt)
		{
			if(ref_ && (1 == ref_->count --))
			{
				ref_->~ref_object_helper<value_type, deleter_type>();
				new (ref_) ref_object_helper<value_type, deleter_type>(vt);
			}
			else 
				ref_ = new ref_object_helper<value_type, deleter_type>(vt);
			
			return *this;
        }

		operator const_reference() const
		{
			if(ref_ == 0)
				throw bad_handle("nana::refer.operator value_type&() const");
			return ref_->value;
        }

		operator reference()
        {
			if(ref_ == 0)
				throw bad_handle("nana::refer.operator value_type&()");
			return ref_->value;
        }

		bool empty()const
		{
			return (0 == ref_);
		}

		operator bool() const
		{
			return (0 != ref_);
		}

		operator const void*() const
		{
			return ref_;
		}

		reference handle()
		{
			if(ref_ == 0)	throw bad_handle("nana::refer.handle()");
			return ref_->value;
		}

		const_reference handle() const
		{
			if(ref_ == 0)	throw nana::bad_handle("nana::refer::handle, it does not refer to any object");
			return ref_ ->value;
		}

		unsigned count() const
		{
			return (ref_ ? ref_->count : 0);
		}
	private:
		template<typename T, typename Deleter>
		struct ref_object_helper
		{
			ref_object_helper(const T& t)
				:value(t), count(1)
			{}
           
			~ref_object_helper()
			{
				Deleter()(value);
			}

			T value;
			unsigned int count;
		};

		template<typename T>
		struct ref_object_helper<T, nana::null_type>
		{
			//Introducing a second template parameter into def_deleter is avoiding the template-specialization of a member template class. 
			template<bool IsPtr, int> struct def_deleter
			{
				void operator()(T &){}
			};

			template<int HelperForSpec> struct def_deleter<true, HelperForSpec>
			{
				void operator()(T &p)
				{
					delete p;
				}
			};

			ref_object_helper(const T& t)
				:value(t), count(1)
			{}

			~ref_object_helper()
			{
				//This is dangerous, the class refer does not know whether the value refers to an array buffer or a heap object.
				def_deleter<nana::traits::is_pointer<T>::value, 0>()(value);
			}

			T value;
			unsigned int count;
		};
             
	private:
		ref_object_helper<value_type, deleter_type>* ref_;
	};
}// end namespace nana

#endif
