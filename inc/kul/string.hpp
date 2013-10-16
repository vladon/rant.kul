/*
 * string.hpp
 *
 *  Created on: Feb 14, 2013
 *      Author: philip
 */

#ifndef _KUL_ST_D_STRING_HPP_
#define _KUL_ST_D_STRING_HPP_

#include "glog/logging.h"

#include "kul/ext/google.hpp"

#include <string>
#include <vector>
#include <string.h>
#include <sstream>

namespace kul { 

class String{
	public:
		static std::string toString(const int& i){
			std::stringstream out;
			out << i;
			return out.str();
		}
		static void replace(std::string& s, const std::string& f, const std::string& r){
			s = s.substr(0, s.find(f)) + r + s.substr(s.find(f) + 1);
		}
		static void replaceAll(std::string& s, const std::string& f, const std::string& r){
			while(s.find(f) != std::string::npos)
				replace(s, f, r);
		}
		static void leftTrim(std::string& s, const char& delim=' '){
			while(s.find(delim) == 0)
				s = s.substr(1);
		}
		static void rightTrim(std::string& s, const char& delim=' '){
			while(s.rfind(delim) == s.size() - 1)
				s = s.substr(0, s.size() - 2);			
		}		
		static void trim(std::string& s){
			while(s.find(' ') == 0 || s.find('	') == 0)
				s = s.substr(1);
			while(s.rfind(' ') == s.size() - 1 || s.rfind('	') == s.size() - 1)
				s = s.substr(0, s.size() - 2);
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
			while((pos = s.find(delim, pos + 1)) != std::string::npos){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}		
		static bool compareIgnoreCase(const std::string& a, const std::string& b){
			    std::string aCpy(a);
			    std::string bCpy(b);
			    std::transform(aCpy.begin(), aCpy.end(), aCpy.begin(), ::tolower);
			    std::transform(bCpy.begin(), bCpy.end(), bCpy.begin(), ::tolower);
			    return (aCpy == bCpy);
		}
};

class WString{
	public:
		static std::string toString(const std::wstring& ws){
			return std::string(ws.begin(), ws.end());
		}
		static void replace(std::wstring& s, const std::wstring& f, const std::wstring& r){
			s = s.substr(0, s.find(f)) + r + s.substr(s.find(f) + 1);
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
			while((pos = s.find(delim, pos + 1)) != std::wstring::npos){
				ss.push_back(l.substr(0, pos));
				l = l.substr(pos + 1);
			}
			ss.push_back(l);
			return ss;
		}		
};

};
#endif /* _KUL_ST_D_STRING_HPP_ */
