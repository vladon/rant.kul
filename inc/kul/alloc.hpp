/*
 * alloc.hpp
 *
 *  Created on: 24 Jan 2013
 *      Author: philix
 */

#ifndef _ALLOC_HPP_
#define _ALLOC_HPP_

#include <cstdlib>

namespace kul { namespace alloc{

template <class T>
class HeapObject{
	private:
		void *	operator 	new(size_t size);
		void 	operator 	delete(void *p);
		void *	operator 	new[](size_t size);
		void  	operator 	delete[](void *p);
	public:
		static T* create(){ return new T();}
		template <class X, class Y> static T* create(X x, Y y) { return new T(x, y);}
		template <class X, class Y, class Z> static T* create(X x, Y y, Z z) { return new T(x, y, z);}
};



};};
#endif /* _ALLOC_HPP_ */
