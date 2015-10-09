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
#include "kul/except.hpp"
#include "kul/string.hpp"

namespace kul{ namespace code{ 

enum Mode { NONE = 0, STAT, SHAR};

class CompilerProcessCapture : public kul::ProcessCapture{
	private:
		std::exception_ptr ep;
		std::string c, t;
	public:
		CompilerProcessCapture() : ep(){}
		CompilerProcessCapture(kul::AProcess& p) : kul::ProcessCapture(p), ep(){}
		CompilerProcessCapture(const CompilerProcessCapture& cp) : kul::ProcessCapture(cp), ep(cp.ep), c(cp.c), t(cp.t){}

		void exception(const std::exception_ptr& e)	{ ep = e; }
		const std::exception_ptr& exception() const	{ return ep; }

		void cmd(const std::string& cm) { this->c = cm; }
		const std::string& cmd() const 	{ return c; }

		void tmp(const std::string& tm) { this->t = tm; }
		const std::string& tmp() const 	{ return t; }
};

class Compiler{	
	protected:
		Compiler(const int& v) : version(v){}
		const int version;
	public:
		virtual ~Compiler(){}		
		virtual bool sourceIsBin()		const = 0;
		virtual const CompilerProcessCapture buildExecutable(
			const std::string& linker,
			const std::string& linkerEnd,
			const std::vector<std::string>& objects, 	
			const std::vector<std::string>& libs,
			const std::vector<std::string>& libPaths,
			const std::string& out, 
			const Mode& mode) 			const throw (kul::Exception) = 0;
		virtual const CompilerProcessCapture buildLibrary(
			const std::string& proc, 
			const std::vector<std::string>& objects, 	
			const kul::File& out, 
			const Mode& mode) 			const throw (kul::Exception) = 0;
		virtual const CompilerProcessCapture compileSource	(
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

}}
#endif /* _KUL_CODE_COMPILER_HPP_ */