/**

./inc/kul/code/compiler.hpp

Created on: 13 Oct 2013

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
#ifndef _KUL_CODE_COMPILER_HPP_
#define _KUL_CODE_COMPILER_HPP_


#include "kul/os.hpp"
#include "kul/cli.hpp"
#include "kul/hash.hpp"
#include "kul/proc.hpp"
#include "kul/string.hpp"
#include "kul/except.hpp"

namespace kul{ namespace code{ 

enum Mode { NONE = 0, STAT, SHAR};

class Compiler{	
	protected:
		Compiler(const int& v) : version(v){}
		const int version;
		kul::Process* setupProcess(const std::string& c, const std::string& cmdline, const std::vector<kul::cli::EnvVar>& evs) const {
			
			std::vector<std::string> bits = kul::cli::CmdLine::asArgs(cmdline);

			std::string cmd = c;
			std::string path;
			if(c.find(" ") != std::string::npos)
				for(const std::string& s : kul::String::split(c, ' ')){
					cmd = s;
					break;	
				}
			if(kul::OS::localPath(cmd).find(kul::OS::dirSep()) != std::string::npos){
				path = cmd.substr(0, cmd.rfind(kul::OS::dirSep()));
				cmd = cmd.substr(cmd.rfind(kul::OS::dirSep()) + 1);
			}
			
			kul::Process* p;
			if(path.empty()) 	p = kul::Process::create(cmd);
			else 				p = kul::Process::create(path, cmd);

			bits.erase(bits.begin());			
			for(const std::string& s : bits)
				p->addArg(s);			
			for(const kul::cli::EnvVar& ev : evs)
				p->addEnvVar(ev.name(), ev.toString());

			return p;
		}
	public:
		virtual ~Compiler(){}		
		virtual bool sourceIsBin()		const = 0;
		virtual const std::string buildExecutable(
			const std::string& linker,
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const hash::set::String& libPaths,
			const std::string& out, 
			const Mode& mode) 			const throw (kul::Exception) = 0;
		virtual const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const throw (kul::Exception) = 0;
		virtual const std::string buildStaticLibrary(
			const std::string& archiver,
			const std::vector<std::string>& objects, 	
			const std::string& outDir,
			const std::string& outFile) const throw (kul::Exception) = 0;
		virtual const std::string compileSource	(
			const std::string& compiler,
			const std::vector<std::string>& args, 		
			const hash::set::String& incs, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) = 0;
		virtual void preCompileHeader(
			const hash::set::String& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) = 0;
};

};};
#endif /* _KUL_CODE_COMPILER_HPP_ */