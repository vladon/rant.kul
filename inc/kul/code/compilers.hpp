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
			gcc		= std::make_unique<cpp::GCCompiler>();
			clang	= std::make_unique<cpp::ClangCompiler>();
			intel	= std::make_unique<cpp::IntelCompiler>();
			winc	= std::make_unique<cpp::WINCompiler>();

			wincs 	= std::make_unique<csharp::WINCompiler>();

			cs.insert(std::pair<std::string, Compiler*>("gcc"		, gcc.get()));
			cs.insert(std::pair<std::string, Compiler*>("g++"		, gcc.get()));
			cs.insert(std::pair<std::string, Compiler*>("nvcc"		, gcc.get()));
			cs.insert(std::pair<std::string, Compiler*>("clang"		, clang.get()));
			cs.insert(std::pair<std::string, Compiler*>("clang++"	, clang.get()));
			cs.insert(std::pair<std::string, Compiler*>("icc"		, intel.get()));
			cs.insert(std::pair<std::string, Compiler*>("icpc"		, intel.get()));
			cs.insert(std::pair<std::string, Compiler*>("cl"		, winc.get()));
			cs.insert(std::pair<std::string, Compiler*>("csc"		, wincs.get()));
		}
		std::unique_ptr<Compiler> gcc;
		std::unique_ptr<Compiler> clang;
		std::unique_ptr<Compiler> intel;
		std::unique_ptr<Compiler> winc;
		std::unique_ptr<Compiler> wincs;
		hash::map::S2T<Compiler*> cs;
	public:
		static Compilers& INSTANCE(){ 
			static Compilers instance;
			return instance;
		}
		const Compiler* get(const std::string& c) throw(CompilerNotFoundException){
			std::string comp = c;
			kul::String::replaceAll(comp, ".exe", "");

			if(cs.count(comp) > 0)
				return (*cs.find(comp)).second;

			if(comp.find(" ") != std::string::npos)
				for(const std::string& s :kul::String::split(comp, ' ')){
					if(cs.count(s) > 0)
						return (*cs.find(s)).second;

					if(std::string(kul::Dir(s).locl()).find(kul::Dir::SEP()) != std::string::npos)
						if(cs.count(s.substr(s.rfind(kul::Dir::SEP()) + 1)) > 0)
							return (*cs.find(s.substr(s.rfind(kul::Dir::SEP()) + 1))).second;
				}
			
			if(std::string(kul::Dir(comp).locl()).find(kul::Dir::SEP()) != std::string::npos)
				if(cs.count(comp.substr(comp.rfind(kul::Dir::SEP()) + 1)) > 0)
					return (*cs.find(comp.substr(comp.rfind(kul::Dir::SEP()) + 1))).second;
			
			KEXCEPT(CompilerNotFoundException, "Compiler for " + comp + " is not implemented");
		}
};


}}
#endif /* _KUL_CODE_COMPILERS_HPP_ */