/*
 * ScopedVector.hpp
 *
 *  Created on: 13 Jan 2013
 *      Author: philix
 */

#ifndef _Vector_HPP_
#define _Vector_HPP_

#include <vector>

namespace kul { namespace smart{

class test{ public: void Test();};

template <class T> class Iterator{
	private:
		const T* t;
		int pos;
	public:
		Iterator(const T* t, int pos) : t(t), pos(pos){}
		const Iterator& 	operator++() 									 				{ ++pos; return *this;}
		const bool 			operator!=		(const Iterator& i)	const 	{ return pos != i.pos;}
		const T					operator*() 										const 	{ return t[pos];}
};

template <class T> class Array{
	private:
		const T* ts;
	public:
		Array(T* ts) : ts(ts){}
		Array(std::vector<T*>& vts) 	{ std::copy(vts.begin()	, vts.end()	, ts);}
		Array(std::vector<T*>* vts) 	{ std::copy(vts->begin(), vts->end()	, ts);}
		~Array(){ delete[] ts; }
		const T* 	get() 							const { return ts;}
		const int 	size() 						const { return (sizeof(ts)/sizeof(T)); }
		const Iterator<T> begin()		const { return Iterator<T>(ts, 0); }
		const Iterator<T> end()			const { return Iterator<T>(ts, this->size() + 1); }
};

template <class T> class Vector{
	private:
		const std::vector<T*>* ts;
	public:
		explicit Vector<T>(const std::vector<T*> * ts = 0 ) : ts(ts){}
		Vector<T>(const Vector<T>& s) : ts(s.get()){}
		~Vector() {
			if(ts){
				for(typename std::vector<T*, std::allocator<T*> >::const_iterator it = ts->begin();  it != ts->end(); it++){
					if(*it) delete *it;
				}
				delete ts;
			}
		}
		Vector<T>& 						operator=(const Vector& s) 	const { return *this;}
		const std::vector<T*>* 	get() 													const { return ts; }
		const std::vector<T*>* 	operator->() 									const { return ts; }
};



};};
#endif /* _Vector_HPP_ */
