/**

./inc/kul/type.hpp

Created on: 21 Aug 2015

Copyright (c) 2015, Philip Deegan

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
#ifndef _KUL_TYPE_HPP_
#define _KUL_TYPE_HPP_

#include "kul/except.hpp"
#include "kul/string.hpp"

namespace kul { namespace type{
class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};
}

class Bool{
	public:
		static bool FROM(std::string s){
			kul::String::trim(s);
			const std::vector<std::string>& pos {"yes", "y", "true", "1"}; 
			const std::vector<std::string>& neg {"no", "n", "false", "0"}; 
			std::transform(s.begin(), s.end(), s.begin(), ::tolower);
			if (std::find(pos.begin(), pos.end(), s) != pos.end()) return true;
			if (std::find(neg.begin(), neg.end(), s) != neg.end()) return false;
			KEXCEPTION("input not bool-able");
		}

};

class Type{
	public:
		template <class T>
		static T FROM(std::string s){
			T t = 0;
			std::stringstream ss(s);
			ss >> t;
			return t;
		}
};
}
#endif /* _KUL_TYPE_HPP_ */
