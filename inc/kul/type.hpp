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

namespace kul { 

template <class T>
class Ref{
	private:
		T& t;
	public:
		Ref(T& t) : t(t){}
		T& get() const { return t;}
};
	
class TypeException : public kul::Exception{
	public:
		TypeException(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class Bool{
	public:
		static bool FROM(std::string s){
			kul::String::trim(s);
			const std::vector<std::string>& pos {"yes", "y", "true", "1"}; 
			const std::vector<std::string>& neg {"no", "n", "false", "0"}; 
			std::transform(s.begin(), s.end(), s.begin(), ::tolower);
			if (std::find(pos.begin(), pos.end(), s) != pos.end()) return true;
			if (std::find(neg.begin(), neg.end(), s) != neg.end()) return false;
			KEXCEPT(TypeException, "input not bool-able");
		}

};

template <class T>
class Type{
	private:
		static int GET(const std::string& s){
			try{ 
				return std::stoi(s); }
			catch(const std::invalid_argument& e){
				KEXCEPT(TypeException, "stoi failed");
			}
		}
	public:
		static T FROM(const std::string& s){
			return GET(s);
		}
};

}
#endif /* _KUL_TYPE_HPP_ */
