
/*
 * CmdLine.cpp
 *
 * Created on: May 20, 2013
 * Author: philip
 */

#include "glog/logging.h"

#include <string.h>
#include <iostream>

#include "kul/cli.hpp"
#include "kul/string.hpp"
#include "kul/smart.hpp"

std::vector<std::string> kul::cli::CmdLine::asArgs(const std::string& cmd) throw (ArgParsingException){
	std::vector<std::string> args;
	
	std::string arg;
	bool openQuotesS = false;
	bool openQuotesD = false;
	bool backSlashed = false;
	for(const char c : cmd){
		if(backSlashed){
			backSlashed = false;
			arg += c;
			continue;
		}
		switch(c){
			case ' ': 
				if(!openQuotesD && !openQuotesS){               //     ||||| ||||| ||||| ||||| |||||
					if(arg.size() > 0) args.push_back(arg);	    //	   ||    || || || || ||    ||
					arg.clear();                                //	   ||||| ||||| ||||| ||    |||||
					continue;	                                //		  || ||    || || ||    ||
				}		                                        //	   ||||| ||    || || ||||| ||||| 				
				break;                                          // 										The final frontier																																																																				... or is it?
			case '"': 
				if(openQuotesD && !openQuotesS){
					openQuotesD = false;
					arg += c;					
					args.push_back(arg);
					arg.clear();
					continue;
				}
				else openQuotesD = true;
				break;
			case '\'':
				if(openQuotesS && !openQuotesD){
					openQuotesS = false;
					arg += c;					
					args.push_back(arg);
					arg.clear();
					continue;
				}
				else openQuotesS = true;
				break;
			case '\\':
				if(!openQuotesS && !openQuotesD){
					backSlashed = true; 
					continue;
				}
				break;
		}
		arg += c;
	}
	if(arg.size() > 0) args.push_back(arg);
	return args;
}

void kul::cli::CmdLine::print(const std::vector<std::string>& ss, bool tab){
	if(tab){
		unsigned int cols = 0;
		for(const std::string& s : ss){
			unsigned int c = kul::String::split(s, '\t').size();
			if(c > cols) cols = c;
		}		
		kul::smart::Array<unsigned int> p;
		for(unsigned int i = 0; i < cols; i++)
			p.add()(new unsigned int(0));

		for(const std::string& s : ss){
			unsigned int c = 0;
			for(const std::string b : kul::String::split(s, '\t')){
				if(b.size() > p[c]) p[c] = b.size();
				c++;
			}			
		}
		for(const std::string& s : ss){
			unsigned int c = 0;
			std::string line;
			for(const std::string b : kul::String::split(s, '\t')){
				if(line.size())
					for(int i = 0; i < std::ceil((p[c]) / b.size()); i++) line = line + "\t";

				line = line + 	" " + b;
				c++;				
			}
			std::cout << line << std::endl;
		}
	}else 
		for(const std::string& s : ss)
			std::cout << s << std::endl;
}