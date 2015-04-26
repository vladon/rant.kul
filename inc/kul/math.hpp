/**

./inc/kul/byte.hpp

Created on: 3 Sept 2014

Copyright (c) 2014, Philip Deegan

This file is part of kul (The Kommon Usage Library).

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _KUL_MATH_HPP_
#define _KUL_MATH_HPP_

#include <math.h>

#include "kul/log.hpp"
#include "kul/except.hpp"

namespace kul{ namespace math{

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

template <class T> T abs(const T& f){
	return f < 0 ? f * -1 : f;
}

template <class T = float> T pow(const float& f, const int& e = 2){
	T r = f < 0 ? -1 : 1;
	for(int i = 0; i < abs(e); i++) r *= f;
	return e < 0 ? 1/r : r;
}

template <class T = float> T root(const float& f, const int& r = 2, const int& it = 6, T g = 0){
	if(r < 1) KEXCEPT(Exception, "Invalid root quotient, must be greater than abs(1)");
	if(g == 0) g = r >= f || r+r >= f ? 1 : (int) f/(r + r);
	for(int i = 0; i < it; i++)
		g = ((float) 1 / r) * ( ((r - 1) * g) + (f / pow<T>(g, r - 1)) );
 	return g;
}

} // END NAMESPACE math
} // END NAMESPACE kul

#endif /* _KUL_MATH_HPP_ */
