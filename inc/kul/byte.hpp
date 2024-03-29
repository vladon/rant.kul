/**

./inc/kul/byte.hpp

Created on: 3 Sept 2014

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
#ifndef _KUL_BYTE_HPP_
#define _KUL_BYTE_HPP_

#include <stdint.h>

namespace kul{ namespace byte{

inline bool isBigEndian(){
	const int one = 1;
	return (*(char*)&one) == 0;
}

class LittleEndian{
	public:
		static uint16_t UINT16(uint16_t i){
			uint16_t n = 42;
			if(*(char *)&n != 42){
				return (i << 8) | (i >> 8 );
			}
			return i;
		}
		static int16_t INT16(int16_t i){
			int16_t n = 42;
			if(*(char *)&n != 42){
				return (i << 8) | ((i >> 8) & 0xFF);
			}
			return i;
		}
		static uint32_t UINT32(uint32_t i){
			uint32_t n = 42;
			if(*(char *)&n != 42){
				i = ((i << 8) & 0xFF00FF00 ) | ((i >> 8) & 0xFF00FF ); 
				return (i << 16) | (i >> 16);
			}
			return i;
		}
		static int32_t INT32(int32_t i){
			int32_t n = 42;
			if(*(char *)&n != 42){
				i = ((i << 8) & 0xFF00FF00) | ((i >> 8) & 0xFF00FF ); 
				return (i << 16) | ((i >> 16) & 0xFFFF);
			}
			return i;
		}
		static uint64_t UINT64(uint64_t i){
			uint64_t n = 42;
			if(*(char *)&n != 42){
				i = ((i << 8) & 0xFF00FF00FF00FF00ULL ) | ((i >> 8) & 0x00FF00FF00FF00FFULL );
				i = ((i << 16) & 0xFFFF0000FFFF0000ULL ) | ((i >> 16) & 0x0000FFFF0000FFFFULL );
				return (i << 32) | (i >> 32);
			}
			return i;
		}
		static int64_t INT64(int64_t i){
			int64_t n = 42;
			if(*(char *)&n != 42){
				i = ((i << 8) & 0xFF00FF00FF00FF00ULL ) | ((i >> 8) & 0x00FF00FF00FF00FFULL );
				i = ((i << 16) & 0xFFFF0000FFFF0000ULL ) | ((i >> 16) & 0x0000FFFF0000FFFFULL );
				return (i << 32) | ((i >> 32) & 0xFFFFFFFFULL);
			}
			return i;
		}

};

}}

#endif /* _KUL_BYTE_HPP_ */
