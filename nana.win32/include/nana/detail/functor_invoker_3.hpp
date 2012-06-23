/*
 *	A Functor implementation
 *	Copyright(C) 2003-2012 Jinhao(cnjinhao@hotmail.com)
 *
 *	Distributed under the Nana Software License, Version 1.0. 
 *	(See accompanying file LICENSE_1_0.txt or copy at 
 *	http://stdex.sourceforge.net/LICENSE_1_0.txt)
 *
 *	@file: nana/detail/functor_invoker_3.hpp
 *	@description:
 *		This is a functor invoker definition. It specifies an invoker with 3 parameters.
 */
 
#ifndef NANA_DETAIL_FUNCTOR_INVOKER_3_HPP
#define NANA_DETAIL_FUNCTOR_INVOKER_3_HPP
#include "functor_invoker.hpp"

namespace nana
{
	namespace detail
	{
		/*
		 * 1-Parameter Implementation
		 */
		template<typename R, typename P0, typename P1, typename P2>
		class abs_invoker<R(P0, P1, P2)>
		{
		public:
			virtual ~abs_invoker(){}
			virtual R apply(P0, P1, P2) const = 0;
			virtual abs_invoker* clone() const = 0;
		};
		
		//class invoker
		//@brief: invoker is used for saving a function address and invoking it.
		template<typename R, typename P0, typename P1, typename P2>
		class invoker<R(P0, P1, P2)>: public abs_invoker<R(P0, P1, P2)>
		{
			typedef abs_invoker<R(P0, P1, P2)> base_type;
			typedef R(*address_t)(P0, P1, P2);
		public:
			invoker(address_t address)
				: address_(address)
			{}
			
			R apply(P0 p0, P1 p1, P2 p2) const{ return (address_ ? address_(p0, p1, p2): R());}
			base_type* clone() const { return new invoker(address_); }
		private:
			address_t address_;
		};

		//Invokers for member function
		template<typename R, typename P0, typename P1, typename P2, typename Concept>
		class invoker<R(Concept::*)(P0, P1, P2)>: public abs_invoker<R(P0, P1, P2)>
		{
			typedef R(Concept::*mfptr)(P0, P1, P2);
		public:
			invoker(Concept& obj, mfptr mf)
				: obj_(obj), mf_(mf)
			{}
			
			R apply(P0 p0, P1 p1, P2 p2) const{ return (mf_ ? (obj_.*mf_)(p0, p1, p2): R());}
			abs_invoker<R(P0, P1, P2)>* clone() const { return new invoker(*this); }
		private:
			Concept & obj_;
			mfptr mf_;
		};

		template<typename R, typename P0, typename P1, typename P2, typename Concept>
		class invoker<R(Concept::*)(P0, P1, P2) const>: public abs_invoker<R(P0, P1, P2)>
		{
			typedef R(Concept::*mfptr)(P0, P1, P2) const;
		public:
			invoker(const Concept& obj, mfptr mf)
				: obj_(obj), mf_(mf)
			{}
			
			R apply(P0 p0, P1 p1, P2 p2) const{ return (mf_ ? (obj_.*mf_)(p0, p1, p2): R());}
			abs_invoker<R(P0, P1, P2)>* clone() const { return new invoker(*this); }
		private:
			const Concept & obj_;
			mfptr mf_;
		};

		template<typename R, typename P0, typename P1, typename P2, typename Concept>
		class invoker<R(Concept::*)(P0, P1, P2) volatile>: public abs_invoker<R(P0, P1, P2)>
		{
			typedef R(Concept::*mfptr)(P0, P1, P2) volatile;
		public:
			invoker(volatile Concept& obj, mfptr mf)
				: obj_(obj), mf_(mf)
			{}
			
			R apply(P0 p0, P1 p1, P2 p2) const{ return (mf_ ? (obj_.*mf_)(p0, p1, p2): R());}
			abs_invoker<R(P0, P1, P2)>* clone() const { return new invoker(*this); }
		private:
			volatile Concept & obj_;
			mfptr mf_;
		};

		template<typename R, typename P0, typename P1, typename P2, typename Concept>
		class invoker<R(Concept::*)(P0, P1, P2) const volatile>: public abs_invoker<R(P0, P1, P2)>
		{
			typedef R(Concept::*mfptr)(P0, P1, P2) const volatile;
		public:
			invoker(const volatile Concept& obj, mfptr mf)
				: obj_(obj), mf_(mf)
			{}
			
			R apply(P0 p0, P1 p1, P2 p2) const{ return (mf_ ? (obj_.*mf_)(p0, p1, p2): R());}
			abs_invoker<R(P0, P1, P2)>* clone() const { return new invoker(*this); }
		private:
			const volatile Concept & obj_;
			mfptr mf_;
		};

		template<typename FO, typename R, typename P0, typename P1, typename P2>
		class fo_invoker<FO, R(P0, P1, P2)>: public abs_invoker<R(P0, P1, P2)>
		{
		public:
			fo_invoker(FO& fobj)
				: fobj_(fobj)
			{}

			R apply(P0 p0, P1 p1, P2 p2) const
			{
				return fobj_(p0, p1, p2);
			}

			abs_invoker<R(P0, P1, P2)> * clone() const
			{
				return (new fo_invoker(*this));
			}
		private:
			mutable FO fobj_;
		};

		template<typename R, typename P0, typename P1, typename P2>
		class interface_holder<R(P0, P1, P2)>
		{
		public:
			typedef R function_type(P0, P1, P2);
			typedef abs_invoker<function_type> invoker_type;

			interface_holder()
				:invoker_(0)
			{}

			interface_holder(const interface_holder& rhs)
				:invoker_(rhs.invoker_?rhs.invoker_->clone():0)
			{
			}

			virtual ~interface_holder()
			{
				delete invoker_;
			}

			void close()
			{
				delete invoker_;
				invoker_ = 0;
			}

			bool empty() const
			{
				return (0 == invoker_);	
			}

			interface_holder& operator=(const interface_holder& rhs)
			{
				if(this != &rhs)
				{
					delete invoker_;
					invoker_ = (rhs.invoker_ ? rhs.invoker_->clone() : 0);
				}
				return *this;
			}

			R operator()(P0 p0, P1 p1, P2 p2) const
			{
				return (invoker_? invoker_->apply(p0, p1, p2) : R());
			}
		protected:
			void assign_invoker(invoker_type* ivk)
			{
				if(ivk)
				{
					delete invoker_;
					invoker_ = ivk;	
				}	
			}
		private:
			invoker_type* invoker_;
		};
	}//end namespace detail
}//end namespace nana
#endif
