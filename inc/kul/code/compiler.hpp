/**

./inc/kul/code/compiler.hpp

Created on: 13 Oct 2013

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
	public:
		virtual ~Compiler(){}		
		virtual bool sourceIsBin()		const = 0;
		virtual const std::string buildExecutable(
			const std::string& linker,
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const std::vector<std::string>& libPaths,
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
			const std::vector<std::string>& incs, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) = 0;
		virtual void preCompileHeader(
			const std::vector<std::string>& incs, 
			const hash::set::String& args, 
			const std::string& in, 
			const std::string& out) 	const throw (kul::Exception) = 0;
};

};};
#endif /* _KUL_CODE_COMPILER_HPP_ */