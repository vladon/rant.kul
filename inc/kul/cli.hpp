/**

./inc/kul/cli.hpp

Created on: 13 Feb 2013

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
#ifndef _CLI_HPP_
#define _CLI_HPP_

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
	private:
		static std::shared_ptr<CmdIn> inst;
	public:
		static CmdIn* SET(CmdIn* cin){ 			
			inst.reset(cin); 
			return inst.get();
		}
		static CmdIn* INSTANCE(){
			if(!inst.get()) inst.reset(new CmdIn());
			return inst.get();
		}
		virtual const std::string receive(const std::string txt){
			std::string s;
			std::cout << txt << std::endl;
			std::cin >> s;
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
		const EnvVarMode 	mode() 		const { return m; }
		const std::string 	toString() 	const {
			std::string var(value());
			kul::String::replaceAll(var, kul::OS::newLine(), "");
			kul::String::trim(var);
			std::string ev(OS::getEnvVar(name()));			
			if(ev.compare("") != 0){
				if 		(mode() == EnvVarMode::PREP)
					var = var + std::string(kul::OS::pathSep()) + ev;	
				else if (mode() == EnvVarMode::APPE)				
					var = ev + std::string(kul::OS::pathSep()) + var;	
			}
			return var;
		}
};

enum ArgType{ FLAG = 0, STRING, INT};

class Arg : public Cmd{
	private:
		const char* d;
		ArgType t;
	public:		
		Arg(const char* d, const char* dd, ArgType t, const char* h = "") : Cmd(dd, h), d(d), t(t){}
		Arg(const char* d, const char* dd, const char* h = "") : Cmd(dd, h), d(d), t(ArgType::FLAG){}
		const char* dash() const { return d;}
		const char* dashdash() const { return command();}
		const bool valueExpected() const { return t != ArgType::FLAG;}
		const ArgType& type() const { return t; }
};

class Args{
	private:
		const std::vector<Cmd>& cmds;
		const std::vector<Arg>& args;
		hash::map::S2S vals;
		Args();
	public:
		Args(const std::vector<Cmd>& cmds, const std::vector<Arg>& args) : cmds(cmds), args(args){}
		void process(int argc, char* argv[]);
		const Cmd& commands(const char* c){
			for(const Cmd& cmd : cmds) if(strcmp(cmd.command(), c) == 0) return cmd;
			throw ArgNotFoundException(__FILE__, __LINE__, "No command " + std::string(c) + " found");
		}
		const Arg& dashes(const char* c){
			for(const Arg& a : arguments()) if(strcmp(a.dash(), c) == 0) return a;
			throw ArgNotFoundException(__FILE__, __LINE__, "No argument " + std::string(c) + " found");
		}
		const Arg& doubleDashes(const char* c){
			for(const Arg& a : arguments()) if(strcmp(a.dashdash(), c) == 0) return a;
			throw ArgNotFoundException(__FILE__, __LINE__, "No argument " + std::string(c) + " found");
		}
		const std::vector<Cmd>& commands()	const { return cmds;}
		const std::vector<Arg>& arguments() const { return args;}
		const hash::map::S2S& 	values() 	const { return vals; }
		const bool contains(const std::string& s) 	{ 
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


};};
#endif /* _CLI_HPP_ */
