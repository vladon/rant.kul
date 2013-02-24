/*
 * ScopedVector.hpp
 *
 *  Created on: 13 Jan 2013
 *      Author: philix
 */

#ifndef _Vector_HPP_
#define _Vector_HPP_

#include <vector>

#include "glog/logging.h"

#include "kul/alloc.hpp"

namespace kul { namespace smart{

template <class T> class Iterator{
	private:
		T* t;
		int pos;
	public:
		Iterator(T* t, int pos) : t(t), pos(pos){}
		Iterator& 	operator++() 									 					{ ++pos; return *this;}
		bool 			operator!=		(const Iterator& i)		const 	{ return pos != i.pos;}
		T&				operator*() 											const 	{ return t[pos];}
};

template <class T> class ArrayHelper;

template <class T> class Array{
	private:
		T* ts;
		int cap, siz;
		Array& swap(const Array& a)	{ T* ots = ts; T* nts = new T[a.siz]; 			std::copy(&a.ts[0]	, &a.ts[a.siz], nts); 						ts = nts; delete[] ots; 	siz=a.siz; cap=a.cap; return *this;}
		void add(T* p){ if(siz == cap)	{ T* ots = ts; T* nts = new T[cap + 10]; 	std::copy(&ts[0]		, &ts[siz]		, nts); cap += 10; 	ts = nts; delete[] ots;} T& r = *p;  ts[siz++] = r; delete p;}
	public:
		Array() : ts(new T[1]), cap(1), siz(0)					{}
		Array(const Array& a) : ts(0), cap(0), siz(0)			{ swap(a); }
		~Array()												{ if(ts) delete[] ts; }
		T&					operator[](const int& i)	const	{ return ts[i];}
		Array&				operator=(const Array& a)			{ return swap(a); }
		ArrayHelper<T>		add()								{ return ArrayHelper<T>(this); }
		const int& 			size() 						const	{ return this->siz; }
		Iterator<T> 		begin()						const	{ return Iterator<T>(ts, 0); }
		Iterator<T> 		end()						const	{ return Iterator<T>(ts, this->size()); }
		friend class ArrayHelper<T>;
};

template <class T> class ArrayHelper{
	private:
		Array<T>* ts;
	public:
		ArrayHelper(Array<T>* ts) : ts(ts){}
		ArrayHelper& operator()(T* t){ ts->add(t); return *this;}
};

template <class T> class Vector{
	private:
		const std::vector<T*>* ts;
	public:
		explicit Vector<T>(const std::vector<T*> * ts = 0 ) : ts(ts){}
		Vector<T>(const Vector<T>& s) : ts(s.get()){}
		~Vector() { if(ts){ for(T* t : *ts) if(t) delete t; delete ts; } }
		Vector<T>& 				operator=(const Vector& s)	const { return *this;}
		const std::vector<T*>* 	get() 						const { return ts; }
		const int& 				size() 						const { return this->ts.size(); }
		Iterator<T> 			begin()						const { return Iterator<T>(ts, 0); }
		Iterator<T> 			end()						const { return Iterator<T>(ts, this->size()); }
		const std::vector<T*>* 	operator->() 				const { return ts; }
};


};};
#endif /* _Vector_HPP_ */
