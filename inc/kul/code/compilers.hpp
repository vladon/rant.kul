/**

./inc/kul/code/compilers.hpp

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
			cpp::IntelCompiler* intel	= new cpp::IntelCompiler();
			cpp::WINCompiler* 	winc	= new cpp::WINCompiler();

			csharp::WINCompiler* wincs 	= new csharp::WINCompiler();

			compilers.insert(std::pair<std::string, Compiler*>("gcc"		, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("g++"		, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("nvcc"		, gcc));
			compilers.insert(std::pair<std::string, Compiler*>("clang"		, clang));
			compilers.insert(std::pair<std::string, Compiler*>("clang++"	, clang));
			compilers.insert(std::pair<std::string, Compiler*>("icc"		, intel));
			compilers.insert(std::pair<std::string, Compiler*>("icpc"		, intel));
			compilers.insert(std::pair<std::string, Compiler*>("cl"			, winc));
			compilers.insert(std::pair<std::string, Compiler*>("csc"		, wincs));
		}
		static Compilers* instance;
		hash::map::S2T<Compiler*> compilers;
	public:
		static Compilers* INSTANCE(){ if(instance == 0) instance = new Compilers(); return instance;}
		const Compiler* get(const std::string& c) throw(CompilerNotFoundException){
			std::string comp = c;
			kul::String::replaceAll(comp, ".exe", "");

			if(Compilers::compilers.count(comp) > 0)
				return (*Compilers::compilers.find(comp)).second;

			if(comp.find(" ") != std::string::npos)
				for(const std::string& s :kul::String::split(comp, ' ')){
					if(Compilers::compilers.count(s) > 0)
						return (*Compilers::compilers.find(s)).second;

					if(std::string(kul::Dir(s).locl()).find(kul::Dir::SEP()) != std::string::npos)
						if(Compilers::compilers.count(s.substr(s.rfind(kul::Dir::SEP()) + 1)) > 0)
							return (*Compilers::compilers.find(s.substr(s.rfind(kul::Dir::SEP()) + 1))).second;
				}
			
			if(std::string(kul::Dir(comp).locl()).find(kul::Dir::SEP()) != std::string::npos)
				if(Compilers::compilers.count(comp.substr(comp.rfind(kul::Dir::SEP()) + 1)) > 0)
					return (*Compilers::compilers.find(comp.substr(comp.rfind(kul::Dir::SEP()) + 1))).second;
			
			throw CompilerNotFoundException(__FILE__, __LINE__, "Compiler for " + comp + " is not implemented");
		}
};


}}
#endif /* _KUL_CODE_COMPILERS_HPP_ */