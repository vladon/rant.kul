/**

./src/proc.cpp

Created on: 11 Aug 2015

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
#include "kul/os.hpp"
#include "kul/log.hpp"

unsigned short kul::Logger::tid(0);

bool kul::Dir::cp(const Dir& d) const{
	if(!d.is() && !d.mk()) KEXCEPT(fs::Exception, "Directory: \"" + d.path() + "\" is not valid");
	Dir c(d.join(name()));
	c.mk();
	for(const auto& f : files())
		f.cp(c);
	for(const auto& dd : dirs())
		dd.cp(c);
	return 1;
}


