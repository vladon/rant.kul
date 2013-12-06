/**

./inc/kul/code/csharp.hpp

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
#ifndef _KUL_CODE_CSHARP_HPP_
#define _KUL_CODE_CSHARP_HPP_

#include "kul/code/compiler.hpp"

namespace kul{ namespace code{ namespace csharp{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class WINCompiler : public Compiler{
	public:
		WINCompiler(const int& v = 0) : Compiler(v){}
		bool sourceIsBin()	const{ return true; }
		const std::string buildExecutable(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects,
			const std::vector<std::string>& libs,
			const hash::set::String& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception){
			
			std::string exe = out + ".exe"; 
			std::string cmd = linker + " /NOLOGO ";
			
			for(const std::string& path : libPaths)
				cmd += " /LIBPATH:\"" + path + "\" ";

			cmd += " /out:\"" + exe + "\" ";
			for(const std::string& o : objects)
				cmd += " " + o + " ";

			for(const std::string& lib : libs)
				cmd += " /reference:" + lib + ".dll ";			

			cmd += linkerEnd;
			LOG(INFO) << cmd;

			LOG(INFO) << kul::OS::getEnvVar("PATH");
			if(kul::OS::execReturn(cmd) != 0)
				throw Exception(__FILE__, __LINE__, "Failed to build executable");

			return exe; 
		}
		const std::string buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::OS::dirJoin(outDir, outFile) + ".dll"); 
			return lib;
		}
		const std::string buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			throw Exception(__FILE__, __LINE__, "No static libraries in C Sharp");
		}
		const std::string compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const hash::set::String& incs, 
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){

			return in;
		}
		virtual void preCompileHeader(			
			const hash::set::String& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			throw Exception(__FILE__, __LINE__, "Method is not implemented");
		}
};

};};};
#endif /* _KUL_CODE_CSHARP_HPP_ */