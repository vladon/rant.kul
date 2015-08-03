/**

./inc/kul/bash.hpp

Created on: 12 Apr 2015

Copyright (c) 2014, Philip Deegan

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
#ifndef _KUL_BASH_HPP_
#define _KUL_BASH_HPP_

#include "kul/proc.hpp"

namespace kul{ namespace bash{

const std::vector<std::string> digHostIPv4(const std::string& h){
	kul::Process p("dig");
	kul::ProcessCapture pc(p);
	try{
		p.arg(h).arg("+short").start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
	return pc.outs();
}
const std::vector<std::string> digHostIPv6(const std::string& h){
	kul::Process p("dig");
	kul::ProcessCapture pc(p);
	try{
		p.arg("AAAA").arg(h).arg("+short").start();
	}catch(const kul::proc::ExitException& e){
		KEXCEPT(Exception, "SCM ERROR " + std::string(e.what()));
	}
	return pc.outs();
}

}// END NAMESPACE bash
}// END NAMESPACE kul
#endif /* _KUL_BASH_HPP_ */
