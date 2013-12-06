/**

./inc/kul/smart.hpp

Created on: 13 Jan 2013

Copyright (c) 2013, Philip Deegan
All rights reserved.

Redistribution and use in binary OR source form, without modification, are permitted provided that the following conditions are met: 
1. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
2. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ''AS IS'' AND ANY  EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/
#ifndef _Vector_HPP_
#define _Vector_HPP_

#include <vector>

#include "glog/logging.h"

namespace kul { namespace smart{

template <class T> class Iterator{
	private:
		T* t;
		int pos;
	public:
		Iterator(T* t, int pos) : t(t), pos(pos){}
		Iterator& 		operator++() 				 				{ ++pos; return *this;}
		bool 			operator!=		(const Iterator& i)	const 	{ return pos != i.pos;}
		T&				operator*() 						const 	{ return t[pos];}
};

template <class T> class ArrayHelper;

template <class T> class Array{
	private:
		T* ts;
		int cap, siz;
		Array& swap(const Array& a)		{ T* ots = ts; T* nts = new T[a.siz]; 		std::copy(&a.ts[0]	, &a.ts[a.siz]	, nts); 			ts = nts; delete[] ots; siz=a.siz; cap=a.cap; return *this;}
		void add(T* p){ if(siz == cap)	{ T* ots = ts; T* nts = new T[cap + 10]; 	std::copy(&ts[0]	, &ts[siz]		, nts); cap += 10; 	ts = nts; delete[] ots;} T& r = *p;  ts[siz++] = r; delete p;}
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
