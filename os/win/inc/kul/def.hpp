/**

./inc/kul/def.hpp

Created on: 18 Aug 2014

Copyright (c) 2013, Philip Deegan

This file is part of kul.win (The Kommon Usage Library for Windows Environments).

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
#ifndef _KUL_DEF_HPP_
#define _KUL_DEF_HPP_

#include "kul/defs.hpp"

#ifndef __KUL_OS__
 	#define __KUL_OS__ win
#endif /*  __KUL_OS__ */

#ifndef PROCESS_BUFFER_SIZE
	#define PROCESS_BUFFER_SIZE 4096
#endif /*  PROCESS_BUFFER_SIZE */

#pragma warning(disable : 4005)
#pragma warning(disable : 4503)
#pragma warning(disable : 4800)

#define KWINTOSTRING(x) KTOSTRING(x)

#define NOEXCEPT

#ifndef INTMAX_MAX
	#define INTMAX_MAX _I64_MAX
#endif /*  INTMAX_MAX */
#ifndef UINTMAX_MAX
	#define UINTMAX_MAX _UI64_MAX
#endif /*  UINTMAX_MAX */

#endif /* _KUL_DEF_HPP_ */
