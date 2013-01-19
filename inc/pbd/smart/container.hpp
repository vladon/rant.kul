/*
 * ScopedVector.hpp
 *
 *  Created on: 13 Jan 2013
 *      Author: philix
 */

#ifndef _SMARTVECTOR_HPP_
#define _SMARTVECTOR_HPP_

#include <vector>

namespace pbd {

template <class T> class SmartVector{
	private:
		const std::vector<T*>* ts;
	public:
		explicit SmartVector<T>(const std::vector<T*> * ts = 0 ) : ts(ts){}
		SmartVector<T>(const SmartVector<T>& s) : ts(s.get()){}
		~SmartVector() {
			if(ts){
				for(typename std::vector<T*, std::allocator<T*> >::const_iterator it = ts->begin();  it != ts->end(); it++){
					delete *it;
				}
				delete ts;
			}
		}
		SmartVector<T>& operator=(const SmartVector& s){ return *this;}

		const std::vector<T*>* get() const {
			return ts;
		}

		const std::vector<T*> * operator->() const {
			return ts;
		}
};



};
#endif /* _SMARTVECTOR_HPP_ */
