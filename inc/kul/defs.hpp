/**

./inc/kul/defs.hpp

Created on: 13 Feb 2013

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
#ifndef _KUL_DEFS_HPP_
#define _KUL_DEFS_HPP_

#ifndef uint
	#define uint unsigned int
#endif /*  uint */

#ifndef ulong
	#define ulong unsigned long
#endif /*  ulong */

#ifndef KLOG_LVL
	#define KLOG_LVL 0
#endif /*  KLOG_LVL */

#ifndef __KUL_THREAD_SPAWN_UWAIT__
 	#define __KUL_THREAD_SPAWN_UWAIT__ 1000000
#endif /*  __KUL_THREAD_SPAWN_UWAIT__ */

#define KSTRINGIFY(x) #x
#define KTOSTRING(x) KSTRINGIFY(x)

#ifndef INTMAX_MAX
	#define INTMAX_MAX _I64_MAX
#endif /*  INTMAX_MAX */
#ifndef UINTMAX_MAX
	#define UINTMAX_MAX _UI64_MAX
#endif /*  UINTMAX_MAX */


#endif /* _KUL_DEFS_HPP_ */
