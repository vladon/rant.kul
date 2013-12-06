/**

./inc/kul/code/compilers.hpp

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
#ifndef _KUL_CODE_COMPILERS_HPP_
#define _KUL_CODE_COMPILERS_HPP_

#include "kul/code/cpp.hpp"
#include "kul/code/csharp.hpp"


namespace kul{ namespace code{ 

class CompilerNotFoundException : public kul::Exception{
	public:
		CompilerNotFoundException(const char*f, const int l, std::string s) : kul::Exception(f, l, s){}
};

class Compilers{
	private:
		Compilers(){
			// add compilers to map
			cpp::GCCompiler* 	gcc		= new cpp::GCCompiler();
			cpp::ClangCompiler* clang	= new cpp::ClangCompiler();
			cpp::WINCompiler* 	winc	= new cpp::WINCompiler();

			csharp::WINCompiler* wincs 	= new csharp::WINCompiler();

			compilers.insert(std::pair<std::string, Compiler*>("gcc"		, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("g++"		, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("nvcc"		, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("clang"		, clang));
			compilers.insert(std::pair<std::string, Compiler*>("clang++"	, clang));
			compilers.insert(std::pair<std::string, Compiler*>("cl"			, winc));
			compilers.insert(std::pair<std::string, Compiler*>("csc"		, wincs));
		}
		static Compilers* instance;
		hash::map::S2T<Compiler*> compilers;
	public:
		static Compilers* INSTANCE(){ if(instance == 0) instance = new Compilers(); return instance;}
		const Compiler* get(const std::string& c) throw(CompilerNotFoundException){
			std::string compiler = c;
			kul::String::replaceAll(compiler, ".exe", "");

			if(Compilers::compilers.count(compiler) > 0)
				return (*Compilers::compilers.find(compiler)).second;

			if(compiler.find(" ") != std::string::npos)
				for(const std::string& s :kul::String::split(compiler, ' ')){
					if(Compilers::compilers.count(s) > 0)
						return (*Compilers::compilers.find(s)).second;

					if(kul::OS::localPath(s).find(kul::OS::dirSep()) != std::string::npos)
						if(Compilers::compilers.count(s.substr(s.rfind(kul::OS::dirSep()) + 1)) > 0)
							return (*Compilers::compilers.find(s.substr(s.rfind(kul::OS::dirSep()) + 1))).second;
				}
			
			if(kul::OS::localPath(compiler).find(kul::OS::dirSep()) != std::string::npos)
				if(Compilers::compilers.count(compiler.substr(compiler.rfind(kul::OS::dirSep()) + 1)) > 0)
					return (*Compilers::compilers.find(compiler.substr(compiler.rfind(kul::OS::dirSep()) + 1))).second;
			
			throw CompilerNotFoundException(__FILE__, __LINE__, "Compiler for " + compiler + " is not implemented");
		}
};


};};
#endif /* _KUL_CODE_COMPILERS_HPP_ */