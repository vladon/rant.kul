/*
 * compilers.hpp
 *
 *  Created on: Feb 15, 2013
 *      Author: philip
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
		kul::StringToTGMap<Compiler*> compilers;
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