/*
 * google.h
 *
 *  Created on: 2 Feb 2013
 *      Author: philix
 */

#ifndef _GOOGLE_H_
#define _GOOGLE_H_

#include "sparsehash/sparse_hash_map"
#include "sparsehash/sparse_hash_set"

#include <string>
#include <vector>

namespace kul{ namespace ext{ namespace goo_gle{

struct StdStringComparator{
	public:
		bool operator()(std::string s1, std::string s2) const{
			return (s1.compare(s2) == 0);
		}
};

typedef google::sparse_hash_map<std::string, std::string, std::tr1::hash<std::string>, StdStringComparator> stringToStringGMap;
typedef google::sparse_hash_set<std::string, std::tr1::hash<std::string>, StdStringComparator> stringGSet;

using namespace google;
template <class T, class HashFcn = std::tr1::hash<std::string>, class EqualKey = StdStringComparator, class Alloc = libc_allocator_with_realloc<std::pair<const std::string, T> > >
class stringToTGMap : google::sparse_hash_map<std::string, T, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<std::string, T, HashFcn, EqualKey, Alloc> stringToT;
		typedef typename stringToT::size_type size_type;
		typedef typename stringToT::key_type key_type;
		typedef typename stringToT::iterator iterator;
		typedef typename stringToT::const_iterator const_iterator;

		std::pair<iterator, bool> 	insert(const std::pair<std::string, T>& obj) 				{  return stringToT::insert(obj); }
		std::pair<iterator, bool> 	insert(std::string s, T t) 														{  return insert(std::pair<std::string, T>(s, t)); }
		T&												operator[](const key_type& key)									{ return stringToT::operator[](key); }
		size_type 									count(const key_type& key) 							const   { return stringToT::count(key); }
		iterator 										begin()               																	{ return stringToT::begin(); }
		iterator 										end()       																				{ return stringToT::end(); }
		const_iterator 							begin() 																	const   { return stringToT::begin(); }
		const_iterator 							end() 																		const   { return stringToT::end(); }
};

using namespace google;
template <class T, class HashFcn = std::tr1::hash<std::string>, class EqualKey = StdStringComparator, class Alloc = libc_allocator_with_realloc<std::pair<const std::string, std::vector<T> > > >
class stringToVectorTGMap : google::sparse_hash_map<std::string, std::vector<T>, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<std::string, std::vector<T>, HashFcn, EqualKey, Alloc> stringToVector;
		typedef typename stringToVector::size_type size_type;
		typedef typename stringToVector::key_type key_type;
		typedef typename stringToVector::iterator iterator;
		typedef typename stringToVector::const_iterator const_iterator;
		std::pair<iterator, bool> 	insert(const std::pair<std::string, std::vector<T> >& obj) 	{ return stringToVector::insert(obj); }
		std::pair<iterator, bool> 	insert(std::string s, std::vector<T> t) 											{ return insert(std::pair<std::string, std::vector<T> >(s, t)); }
		size_type 									count(const key_type& key) 											const   { return stringToVector::count(key); }
		iterator 										begin()               																					{ return stringToVector::begin(); }
		iterator 										end()       																								{ return stringToVector::end(); }
		const_iterator 							begin() 																					const   { return stringToVector::begin(); }
		const_iterator 							end() 																						const   { return stringToVector::end(); }
};

template <class T, class HashFcn = std::tr1::hash<std::string>, class EqualKey = StdStringComparator, class Alloc = libc_allocator_with_realloc<std::pair<const std::string, stringToTGMap<T> > > >
class stringToGMapTGMap : google::sparse_hash_map<std::string, stringToTGMap<T>, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<std::string, stringToTGMap<T>, HashFcn, EqualKey> stringToGMapT;
		typedef typename stringToGMapT::size_type size_type;
		typedef typename stringToGMapT::key_type key_type;
		typedef typename stringToGMapT::iterator iterator;
		typedef typename stringToGMapT::const_iterator const_iterator;
		std::pair<iterator, bool> 	insert(const std::pair<std::string, stringToTGMap<T> >& obj) 				{ return stringToGMapT::insert(obj); }
		std::pair<iterator, bool> 	insert(const std::string s, stringToTGMap<T> t) 											{ return insert(std::pair<std::string, stringToTGMap<T> >(s, t)); }
		stringToTGMap<T>&				operator[](const key_type& key)																		{ return stringToGMapT::operator[](key); }
		size_type 									count(const key_type& key) 																const   { return stringToGMapT::count(key); }
		iterator 										begin()               																										{ return stringToGMapT::begin(); }
		iterator 										end()       																													{ return stringToGMapT::end(); }
		const_iterator 							begin() 																										const   { return stringToGMapT::begin(); }
		const_iterator 							end() 																											const   { return stringToGMapT::end(); }
};

};};};
#endif /* _GOOGLE_H_ */
