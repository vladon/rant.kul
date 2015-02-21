/**

./src/Misc.cpp

Created on: 18 Apr 2013

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
#include "kul/log.hpp"
#include "kul/scm.hpp"
#include "kul/code/compilers.hpp"

int kul::log::ALogger::tid(0);

kul::scm::Manager* kul::scm::Manager::instance(0);

kul::code::Compilers* kul::code::Compilers::instance(0);