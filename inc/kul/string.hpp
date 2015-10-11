/**

./inc/kul/string.hpp

Created on: 14 Feb 2013

Copyright (c) 2013, Philip Deegan

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
#ifndef _KUL_STRING_HPP_
#define _KUL_STRING_HPP_

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace kul { 

class String{
	public:
		static void replace(std::string& s, const std::string& f, const std::string& r){
			size_t p = 0;
			if((p = s.find(f)) != std::string::npos)
				s.replace(p, f.size(), r);
		}
		static void replaceAll(std::string& s, const std::string& f, const std::string& r){
			while(s.find(f) != std::string::npos) replace(s, f, r);
		}
		static void leftTrim(std::string& s, const char& delim=' '){
			while(s.find(delim) == 0)
				s.erase(0, 1);
		}
		static void rightTrim(std::string& s, const char& delim=' '){
			while(s.rfind(delim) == s.size() - 1)
				s.pop_back();
		}
		static void trim(std::string& s){
			while(s.find(' ') == 0 || s.find('	') == 0)
				s.erase(0, 1);
			while(s.rfind(' ') == s.size() - 1 || s.rfind('\t') == s.size() - 1)
				s.pop_back();
		}
		static void pad(std::string& s, const unsigned int& p){
			while(s.size() < p) s += " ";
		}
		static std::vector<std::string> split(const std::string& s, const char& d){
			std::vector<std::string> v;
			split(s, d, v);
			return v;
		}
		static void split(const std::string& s, const char& d, std::vector<std::string>& v){
			if(s.find(d) != std::string::npos){
				std::string l;
				std::stringstream stream(s);
				while(std::getline(stream, l, d))
					if(l.compare("") != 0) v.push_back(l);
			}else v.push_back(s);
		}
		static std::vector<std::string> split(const std::string& s, const std::string& d){
			std::vector<std::string> v;
			split(s, d, v);
			return v;
		}
		static void split(const std::string& s, const std::string& d, std::vector<std::string>& v){
			std::string l = s;
			unsigned int pos = 0;
			while((pos = s.find(d, pos + 1)) < l.size()){
				v.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			v.push_back(l);
		}
		static bool cicmp(const std::string& a, const std::string& b){
			    std::string aCpy(a);
			    std::string bCpy(b);
			    std::transform(aCpy.begin(), aCpy.end(), aCpy.begin(), ::tolower);
			    std::transform(bCpy.begin(), bCpy.end(), bCpy.begin(), ::tolower);
			    return (aCpy == bCpy);
		}
		static std::vector<std::string> lines(const std::string& s){
			std::vector<std::string> v;
			if(s.find("\n") != std::string::npos){
				std::string l;
				std::stringstream ss(s);
				while(std::getline(ss, l)) if(!l.empty()) v.push_back(l);
			}else v.push_back(s);
			return v;
		}
};

}
#endif /* _KUL_STRING_HPP_ */
