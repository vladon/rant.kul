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

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ParseException : public kul::Exception{
	public:
		ParseException(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class ArgNotFoundException : public Exception{
	public:
		ArgNotFoundException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};

class ArgParsingException : public Exception{
	public:
		ArgParsingException(const char*f, const int l, std::string s) : Exception(f, l, s){}
};

class CmdLine{
	public:
		static std::vector<std::string> asArgs(const std::string& cmd) throw(ArgParsingException);
		static void print(const std::vector<std::string>& ss, bool tab = true);		
};

class CmdIn{
	public:
		static CmdIn& INSTANCE(){
			static CmdIn instance;
			return instance;
		}
		virtual bool receiveBool(const std::string txt) {
			std::string t = receive(txt);
			kul::String::trim(t);
			std::vector<std::string> pos; 
			pos.push_back("yes"); pos.push_back("y");  pos.push_back("true");
			std::vector<std::string> neg; 
			neg.push_back("no"); neg.push_back("n");  neg.push_back("false");

			for(const std::string& s : pos) if(kul::String::cicmp(t, s)) return true;
			for(const std::string& s : neg) if(kul::String::cicmp(t, s)) return false;

			if(t.compare("1") == 0) return true;
			if(t.compare("0") == 0) return false;

			KEXCEPT(ParseException, "input not bool-able");
		}
		virtual const std::string receive(const std::string t){
			std::string s;
			std::cout << t << std::endl;
			std::getline(std::cin, s);
			return s;
		}
};

class Cmd{
	private:
		const char* c;
		const char* h;
	public:
		Cmd(const char* c, const char* h = "") : c(c), h(h){}
		const char* command() 	const { return c;}
		const char* help() 		const { return h;}
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
			kul::String::replaceAll(var, kul::os::newLine(), "");
			kul::String::trim(var);
			std::string ev(Env::GET(name()));
			if(ev.compare("") != 0){
				if 		(mode() == EnvVarMode::PREP)
					var = var + std::string(kul::Env::SEP()) + ev;
				else if (mode() == EnvVarMode::APPE)
					var = ev + std::string(kul::Env::SEP()) + var;
			}
			return var;
		}
};

enum ArgType{ FLAG = 0, STRING, MAYBE};

class Arg : public Cmd{
	private:
		const char* d;
		ArgType t;
	public:
		Arg(const char* d, const char* dd, ArgType t, const char* h = "") : Cmd(dd, h), d(d), t(t){}
		Arg(const char* d, const char* dd, const char* h = "") : Cmd(dd, h), d(d), t(ArgType::FLAG){}
		const char* dash() 			const { return d;}
		const char* dashdash() 		const { return command();}
		const ArgType& type() 		const { return t; }
};

class Args{
	private:
		const std::vector<Cmd>& cmds;
		const std::vector<Arg>& args;
		hash::map::S2S vals;
		Args();
	public:
		Args(const std::vector<Cmd>& cmds, const std::vector<Arg>& args) : cmds(cmds), args(args){}
		void process(int argc, char* argv[]) throw(ArgNotFoundException);
		const Cmd& commands(const char* c){
			for(const Cmd& cmd : cmds) if(strcmp(cmd.command(), c) == 0) return cmd;
			KEXCEPT(ArgNotFoundException, "No command " + std::string(c) + " found");
		}
		const Arg& dashes(const char* c){
			for(const Arg& a : arguments()) if(strcmp(a.dash(), c) == 0) return a;
			KEXCEPT(ArgNotFoundException, "No argument " + std::string(c) + " found");
		}
		const Arg& doubleDashes(const char* c){
			for(const Arg& a : arguments()) if(strcmp(a.dashdash(), c) == 0) return a;
			KEXCEPT(ArgNotFoundException, "No argument " + std::string(c) + " found");
		}
		const std::vector<Cmd>& commands()	const { return cmds;}
		const std::vector<Arg>& arguments() const { return args;}
		const hash::map::S2S& 	values() 	const { return vals; }
		bool contains(const std::string& s) { 
			return vals.count(s);
		}
};

class AWritable{
	public:
		virtual const char* msg() const = 0;
};

class AWriter{
	protected:
		virtual void error(const AWritable& a) const = 0;
};


} // END NAMESPACE cli
} // END NAMESPACE kul
#endif /* _KUL_CLI_HPP_ */
