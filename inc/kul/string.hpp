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
		static std::vector<std::string> split(const std::string& s, const char& delim='\n'){
			std::vector<std::string> ss;
			std::string l;
			std::stringstream stream(s);
			while(std::getline(stream, l, delim)) {
				if(l.compare("") != 0)
					ss.push_back(l);
			}
			return ss;
		}
		static std::vector<std::string> split(const std::string& s, const std::string& delim="\n"){
			std::string l = s;
			std::vector<std::string> ss;
			unsigned int pos = 0;
			while((pos = s.find(delim, pos + 1)) < l.size()){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}
		static bool cicmp(const std::string& a, const std::string& b){
			    std::string aCpy(a);
			    std::string bCpy(b);
			    std::transform(aCpy.begin(), aCpy.end(), aCpy.begin(), ::tolower);
			    std::transform(bCpy.begin(), bCpy.end(), bCpy.begin(), ::tolower);
			    return (aCpy == bCpy);
		}
		static std::vector<std::string> lines(const std::string& s){
			std::vector<std::string> ls;
			if(s.find("\n") != std::string::npos){
				std::string l;
				std::stringstream ss(s);
				while(std::getline(ss, l)) if(!l.empty()) ls.push_back(l);
			}else ls.push_back(s);
			return ls;
		}
};

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
		static void leftTrim(std::wstring& s, const wchar_t& delim=' '){
			while(s.find(delim) == 0)
				s = s.substr(1);
		}
		static void rightTrim(std::wstring& s, const wchar_t& delim=' '){
			while(s.rfind(delim) == s.size())
				s = s.substr(0, s.size() - 2);
		}
		static void trim(std::wstring& s){
			while(s.find(' ') == 0 || s.find('	') == 0)
				s = s.substr(1);
			while(s.rfind(' ') == 0 || s.rfind('	') == 0)
				s = s.substr(0, s.size() - 2);
		}
		static std::vector<std::wstring> split(const std::wstring& s, const wchar_t& delim='\n'){
			std::vector<std::wstring> ss;
			std::wstring l = s;
			unsigned int pos = 0;
			while((pos = s.find(delim, pos + 1)) < l.size()){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}
		static std::vector<std::wstring> split(const std::wstring& s, const std::wstring& delim){
			std::vector<std::wstring> ss;
			std::wstring l = s;			
			unsigned int pos = 0;
			while((pos = s.find(delim, pos + 1)) < l.size()){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}		
};

}
#endif /* _KUL_STRING_HPP_ */
