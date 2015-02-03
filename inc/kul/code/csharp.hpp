/**

./inc/kul/code/csharp.hpp

Created on: 13 Oct 2013

Copyright (c) 2013, Philip Deegan

This file is part of kul (The Kommon Usage Library).

This library is free software; you can redistribute it and/or
modify it under the terms of the GNU Lesser General Public
License as published by the Free Software Foundation; either
version 2.1 of the License, or (at your option) any later version.

This library is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPosE.  See the GNU
Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with this library.  If not, see <http://www.gnu.org/licenses/>.
*/
#ifndef _KUL_CODE_CSHARP_HPP_
#define _KUL_CODE_CSHARP_HPP_

#include "kul/code/compiler.hpp"
#include "kul/log.hpp"

namespace kul{ namespace code{ namespace csharp{ 

class Exception : public kul::Exception{
	public:
		Exception(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class WINCompiler : public Compiler{
	public:
		WINCompiler(const int& v = 0) : Compiler(v){}
		bool sourceIsBin()	const{ return true; }
		const CompilerProcessCapture buildExecutable(
			const std::string& linker, 
			const std::string& linkerEnd,
			const std::vector<std::string>& objects,
			const std::vector<std::string>& libs,
			const std::vector<std::string>& libPaths,
			const std::string& out, 
			const Mode& mode) const throw (kul::Exception){
			
			std::string exe = out + ".exe";
			std::string cmd = linker;
			std::vector<std::string> bits;
			if(linker.find(" ") != std::string::npos){
				bits = kul::String::split(linker, ' ');
				cmd = bits[0];
			}
			std::unique_ptr<kul::Process> p(kul::Process::create(cmd));
			CompilerProcessCapture pc(*p, exe);
			for(unsigned int i = 1; i < bits.size(); i++) (*p).addArg(bits[i]);
			(*p).addArg("/NOLOGO").addArg("/OUT:\"" + exe + "\"");
			for(const std::string& path : libPaths)	(*p).addArg("/LIB:\"" + path + "\"");
			for(const std::string& o : objects)	(*p).addArg("\""+ o + "\"");
			for(const std::string& lib : libs)	(*p).addArg("/REFERENCE:\"" + lib + ".dll\"");
			if(linkerEnd.find(" ") != std::string::npos)
				for(const std::string& s: kul::String::split(linkerEnd, ' '))
					(*p).addArg(s);
			else (*p).addArg(linkerEnd);

			try{
				(*p).start();
			}catch(const kul::proc::Exception& e){ 
				KLOG(INF) << e.debug()<< " : " << typeid(e).name();
				pc.failed();
			}

			return pc;
		}
		const CompilerProcessCapture buildSharedLibrary(
			const std::string& linker, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			const std::string lib(kul::Dir(outDir).join(outFile) + ".dll");

			return CompilerProcessCapture(lib);
		}
		const CompilerProcessCapture buildStaticLibrary(
			const std::string& archiver, 
			const std::vector<std::string>& objects, 
			const std::string& outDir, 
			const std::string& outFile) const throw (kul::Exception){ 

			KEXCEPT(Exception, "No static libraries in C Sharp");
		}
		const CompilerProcessCapture compileSource(
			const std::string& compiler, 
			const std::vector<std::string>& args, 
			const std::vector<std::string>& incs, 
			const std::string& in, 
			const std::string& out) const throw (kul::Exception){

			return CompilerProcessCapture(in);
		}
		virtual void preCompileHeader(			
			const std::vector<std::string>& incs,
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) {

			KEXCEPT(Exception, "Method is not implemented");
		}
};

}}}
#endif /* _KUL_CODE_CSHARP_HPP_ */
