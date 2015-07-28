/**

./inc/kul/def.hpp

Created on: 18 Aug 2014

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
#ifndef _KUL_DEF_HPP_
#define _KUL_DEF_HPP_

#include "kul/defs.hpp"

#ifndef __KUL_OS__
 	#define __KUL_OS__ "bsd"
#endif /*  __KUL_OS__ */

#ifndef __KUL_THREAD_SPAWN_WAIT__
 	#define __KUL_THREAD_SPAWN_WAIT__ 5000000
#endif /*  __KUL_THREAD_SPAWN_WAIT__ */

#ifndef NOEXCEPT
	#define NOEXCEPT noexcept
#endif /*  NOEXCEPT */

#endif /* _KUL_DEF_HPP_ */
