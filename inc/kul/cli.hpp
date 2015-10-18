/**

./inc/kul/cli.hpp

Created on: 13 Feb 2013

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
#ifndef _KUL_CLI_HPP_
#define _KUL_CLI_HPP_

#include <string>
#include <vector>
#include <iostream>

#include "kul/os.hpp"
#include "kul/hash.hpp"
#include "kul/except.hpp"
#include "kul/string.hpp"

namespace kul{  namespace cli {

inline const std::string receive(const std::string& t = ""){
	if(!t.empty()) std::cout << t << std::endl;
	std::string s;
	std::getline(std::cin, s);
	return s;
}

const std::string hidden(const std::string& t = "");
void show();

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, const std::string& s) : kul::Exception(f, l, s){}
};

class ArgNotFoundException : public Exception{
	public:
		ArgNotFoundException(const char*f, const int l, const std::string& s) : Exception(f, l, s){}
};


class Cmd{
	private:
		const char* c;
	public:
		Cmd(const char* c) : c(c){}
		const char* command() 	const { return c;}
};

enum EnvVarMode{ APPE = 0, PREP, REPL};

class EnvVar{
	private:
		const std::string n;
		const std::string v;
		const EnvVarMode m;
	public:
		EnvVar(const std::string n, const std::string v, const EnvVarMode m) : n(n), v(v), m(m){}
		const char* 		name() 		const { return n.c_str(); }
		const char* 		value() 	const { return v.c_str(); }
		EnvVarMode		 	mode() 		const { return m; }
		const std::string 	toString() 	const {
			std::string var(value());
			kul::String::replaceAll(var, kul::os::EOL(), "");
			kul::String::trim(var);
			const char* c = env::GET(name());
			std::string ev(c ? c : "");
			if(!ev.empty()){
				if 		(mode() == EnvVarMode::PREP)
					var = var + kul::env::SEP() + ev;
				else if (mode() == EnvVarMode::APPE)
					var = ev + kul::env::SEP() + var;
			}
			return var;
		}
};

enum ArgType{ FLAG = 0, STRING, MAYBE};

class Arg : public Cmd{
	private:
		bool man;
		const char d;
		ArgType t;
	public:
		Arg(const char d, const char* dd, ArgType t, bool m = false) : Cmd(dd), man(m), d(d), t(t){}
		Arg(const char d, const char* dd, bool m = false) : Cmd(dd), man(m), d(d), t(ArgType::FLAG){}
		bool mandatory() 			const { return man;}
		const char  dash() 			const { return d;}
		const char* dashdash() 		const { return command();}
		const ArgType& type() 		const { return t; }
};

class Args{
		std::vector<Cmd> cmds;
		std::vector<Arg> args;
		hash::map::S2S vals;
	public:
		Args(){}
		Args(const std::vector<Cmd>& cmds, const std::vector<Arg>& args) : cmds(cmds), args(args){}
		void arg(const Arg& a){ args.push_back(a); }
		void cmd(const Cmd& c){ cmds.push_back(c); }
		const Cmd& commands(const char* c) const {
			for(const Cmd& cmd : cmds) if(strcmp(cmd.command(), c) == 0) return cmd;
			KEXCEPT(ArgNotFoundException, "No command " + std::string(c) + " found");
		}
		const Arg& dashes(const char c) const {
			for(const Arg& a : arguments()) if(a.dash() == c) return a;
			KEXCEPT(ArgNotFoundException, "No argument " + std::string(1, c) + " found");
		}
		const Arg& doubleDashes(const char* c) const {
			for(const Arg& a : arguments()) if(strcmp(a.command(), c) == 0) return a;
			KEXCEPT(ArgNotFoundException, "No argument " + std::string(c) + " found");
		}
		const std::vector<Cmd>& commands()	const  { return cmds;}
		const std::vector<Arg>& arguments() const  { return args;}
		const std::string& get(const std::string& s) const {
			if(has(s)) return (*vals.find(s)).second;
			KEXCEPT(ArgNotFoundException, "No value " + s + " found");
		}
		bool empty() const {
			return vals.size() == 0;
		}
		bool has(const std::string& s) const {
			return vals.count(s);
		}
		void process(int argc, char* argv[], int first = 1) throw(ArgNotFoundException){
			for(const Arg& a1 : arguments())
				for(const Arg& a2 : arguments()){
					if(&a1 == &a2) continue;
					if((a1.dash() != ' ') && (a1.dash() == a2.dash()
							|| strcmp(a1.dashdash(), a2.dashdash()) == 0))
						KEXCEPT(Exception, "Duplicate argument detected");
				}
			for(const Cmd& c1 : commands())
				for(const Cmd& c2 : commands()){
					if(&c1 == &c2) continue;
					if(strcmp(c1.command(), c2.command()) == 0)
						KEXCEPT(Exception, "Duplicate argument detected");
				}

			Arg* arg = 0;
			int valExpected = 0;
			std::string valExpectedFor, c, t;

			for(int i = first; i < argc; i++){
				c = argv[i];
				t = c;

				if(c.compare("---") == 0)	KEXCEPT(Exception, "Illegal argument ---");
				if(c.compare("--") == 0)	KEXCEPT(Exception, "Illegal argument --");
				if(c.compare("-") == 0) 	KEXCEPT(Exception, "Illegal argument -");
				if(valExpected == 1 || (valExpected == 2 && c.find("-") != 0)){
					valExpected = 0;
					vals[arg->dashdash()] = c;
					arg = 0;
					continue;
				}else if(valExpected == 2) valExpected = 0;

				if(c.find("--") == 0){
					c = c.substr(c.find("--") + 2);
					valExpectedFor = c;
					if(c.find("=") == std::string::npos){
						arg = const_cast<Arg*>(&doubleDashes(c.c_str()));
						valExpected = arg->type();
						if(!valExpected) vals[arg->dashdash()] = "";
						continue;
					}
					valExpectedFor = c.substr(0, c.find("="));
					arg = const_cast<Arg*>(&doubleDashes(valExpectedFor.c_str()));
					valExpected = arg->type();
					if(valExpected == 0)
						KEXCEPT(Exception, "Found = when no value is expected for arg " + valExpectedFor);
					c = c.substr(c.find("=") + 1);
					vals[arg->dashdash()] = c;
				}
				else if(c.find("-") == 0){
					valExpectedFor = c;
					c = c.substr(c.find("-") + 1);
					if(c.find("=") != std::string::npos){
						if(c.substr(0, c.find("=")).size() > 1)
							KEXCEPT(Exception, "Cannot mix flag and non-flag arguments");
						arg = const_cast<Arg*>(&dashes(c.at(0)));
						vals[arg->dashdash()] = c.substr(c.find("=") + 1);
						valExpected = 0;
					}else if(c.length() > 1){
						std::string a = c;
						for(unsigned int i = 0; i < c.length(); i++){
							arg = const_cast<Arg*>(&dashes(a.at(0)));
							if(i + 1 == c.length())
								valExpected = arg->type();
							else if(arg->type() == ArgType::STRING)
								KEXCEPT(Exception, "Cannot mix flag and non-flag arguments");
							vals[arg->dashdash()] = "";
							if(a.length() > 1) a = a.substr(1);
						}
					}else{
						arg = const_cast<Arg*>(&dashes(c.at(0)));
						valExpected = arg->type();
						vals[arg->dashdash()] = "";
					}
					continue;
				}
				if(valExpected == 1){
					valExpected = 0;
					continue;
				}
				commands(c.c_str());
				vals[c] = "";
			}
			if(valExpected == 1) KEXCEPT(Exception, "Value expected for argument: \""+ valExpectedFor + "\"");
			for(const Arg& a : args) if(a.mandatory()) get(a.dashdash());
		}
};

inline std::vector<std::string> asArgs(const std::string& cmd){
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

} // END NAMESPACE cli
} // END NAMESPACE kul
#endif /* _KUL_CLI_HPP_ */
