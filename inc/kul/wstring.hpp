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
#ifndef _KUL_WSTRING_HPP_
#define _KUL_WSTRING_HPP_

#include <string>
#include <vector>
#include <sstream>
#include <algorithm>

namespace kul { 


class WString{
	public:
		static std::string toString(const std::wstring& ws){
			return std::string(ws.begin(), ws.end());
		}
		static void replace(std::wstring& s, const std::wstring& f, const std::wstring& r){
			s = s.substr(0, s.find(f)) + r + s.substr(s.find(f) + f.size());
		}
		static void replaceAll(std::wstring& s, const std::wstring& f, const std::wstring& r){
			while(s.find(f) < s.size())
				replace(s, f, r);
		}
		static void leftTrim(std::wstring& s, const wchar_t& d=' '){
			while(s.find(d) == 0)
				s = s.substr(1);
		}
		static void rightTrim(std::wstring& s, const wchar_t& d=' '){
			while(s.rfind(d) == s.size())
				s = s.substr(0, s.size() - 2);
		}
		static void trim(std::wstring& s){
			while(s.find(' ') == 0 || s.find('	') == 0)
				s = s.substr(1);
			while(s.rfind(' ') == 0 || s.rfind('	') == 0)
				s = s.substr(0, s.size() - 2);
		}
		static std::vector<std::wstring> split(const std::wstring& s, const wchar_t& d){
			std::vector<std::wstring> ss;
			std::wstring l = s;
			unsigned int pos = 0;
			while((pos = s.find(d, pos + 1)) < l.size()){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}
		static std::vector<std::wstring> split(const std::wstring& s, const std::wstring& d){
			std::vector<std::wstring> ss;
			std::wstring l = s;			
			unsigned int pos = 0;
			while((pos = s.find(d, pos + 1)) < l.size()){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}		
};

}
#endif /* _KUL_WSTRING_HPP_ */
