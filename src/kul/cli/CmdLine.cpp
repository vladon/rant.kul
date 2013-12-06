/**

./src/kul/cli/CmdLine.cpp

Created on: 20 May 2013

Copyright (c) 2013, Philip Deegan
All rights reserved.

Redistribution and use in binary OR source form, without modification, are permitted provided that the following conditions are met: 
1. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
2. Neither the name of the organization nor the
   names of its contributors may be used to endorse or promote products
   derived from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY COPYRIGHT HOLDER ''AS IS'' AND ANY  EXPRESS OR IMPLIED WARRANTIES, 
INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A 
PARTICULAR PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL  BE LIABLE FOR ANY DIRECT, INDIRECT, 
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, 
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS 
INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT 
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF 
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

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