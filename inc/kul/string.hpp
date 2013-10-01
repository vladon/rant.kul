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

namespace kul { namespace st_d{

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
				LOG(INFO) << l;
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

};};
#endif /* _KUL_ST_D_STRING_HPP_ */
