
/*
 * CmdLine.cpp
 *
 * Created on: May 20, 2013
 * Author: philip
 */

#include "glog/logging.h"

#include <string.h>

#include "kul/cli.hpp"
#include "kul/string.hpp"

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
	return args;
}