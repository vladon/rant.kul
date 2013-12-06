/**

./src/kul/cli/Args.cpp

Created on: 14 Feb 2013

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

#include "kul/cli.hpp"
#include "kul/string.hpp"


void kul::cli::Args::process(int argc, char* argv[]){
	std::string cliArg = "";
	for(int i = 1; i < argc; i++)
		cliArg += std::string(argv[i]) + " ";

	for(const Arg& a : arguments())
		if(strlen(a.dash()) > 1)
			throw Exception(__FILE__, __LINE__, "Short param too long, only one character allowed");

	for(const Arg& a1 : arguments())
		for(const Arg& a2 : arguments()){
			if(&a1 == &a2) continue;
			if(strcmp(a1.dash(), a2.dash()) == 0
					|| strcmp(a1.dashdash(), a2.dashdash()) == 0)
				throw Exception(__FILE__, __LINE__, "Duplicate argument detected");
		}
	for(const Cmd& c1 : commands())
		for(const Cmd& c2 : commands()){
			if(&c1 == &c2) continue;
			if(strcmp(c1.command(), c2.command()) == 0)
				throw Exception(__FILE__, __LINE__, "Duplicate argument detected");
		}

	Arg* arg = 0;
	bool valExpected = false;
	std::string valExpectedFor, c, t;

	for(int i = 1; i < argc; i++){
		c = argv[i];
		t = c;

		kul::String::replaceAll(t, "----", "---");
		if(t.find("---") != std::string::npos) throw Exception(__FILE__, __LINE__, "Illegal argument " + c);
		if(c.compare("---") == 0)	throw Exception(__FILE__, __LINE__, "Illegal argument ---");
		if(c.compare("--") == 0)	throw Exception(__FILE__, __LINE__, "Illegal argument --");
		if(c.compare("-") == 0) throw Exception(__FILE__, __LINE__, "Illegal argument -");
		if((c.find("--") == 0 || c.find("-") == 0) && valExpected)
			throw Exception(__FILE__, __LINE__, "Value expected for argument: \""+ valExpectedFor + "\"");

		if(valExpected){
			valExpected = !valExpected;
			vals[arg->dashdash()] = c;
			arg = 0;
			continue;
		}

		if(c.find("--") == 0){
			c = c.substr(c.find("--") + 2);
			valExpectedFor = c;
			if(c.find("=") == std::string::npos){
				arg = const_cast<Arg*>(&doubleDashes(c.c_str()));
				valExpected = arg->valueExpected();
				continue;
			}
			valExpectedFor = c.substr(0, c.find("="));
			arg = const_cast<Arg*>(&doubleDashes(valExpectedFor.c_str()));
			valExpected = arg->valueExpected();
			if(!valExpected) throw Exception(__FILE__, __LINE__, "Found = when no value is expected for arg " + valExpectedFor);

			c = c.substr(c.find("=") + 1);			
			vals[arg->dashdash()] = c;			
		}
		else if(c.find("-") == 0){
			valExpectedFor = c;
			c = c.substr(c.find("-") + 1);
			if(c.find("=") != std::string::npos){
				if(c.substr(0, c.find("=")).size() > 1)
					throw Exception(__FILE__, __LINE__, "Cannot mix flag and non-flag arguments");
				arg = const_cast<Arg*>(&dashes(c.substr(0, c.find("=")).c_str()));
				vals[arg->dashdash()] = c.substr(c.find("=") + 1);
				valExpected = 0;
			}else if(c.length() > 1){
				for(unsigned int i = 0; i < c.length(); i++){
					char ch = c[i];
					arg = const_cast<Arg*>(&dashes(&ch));
					if(arg->valueExpected())
						throw Exception(__FILE__, __LINE__, "Cannot mix flag and non-flag arguments");
					vals[arg->dashdash()] = "";
				}
			}else{
				arg = const_cast<Arg*>(&dashes(c.c_str()));

				valExpected = arg->valueExpected();
				if(!valExpected)
					vals[arg->dashdash()] = "";
			}
			continue;
		}
		if(valExpected){
			valExpected = !valExpected;
			continue;
		}
		commands(c.c_str());
		vals[c] = "";
	}
	if(valExpected)
		throw Exception(__FILE__, __LINE__, "Value expected for argument: \""+ valExpectedFor + "\"");
}
