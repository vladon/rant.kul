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

namespace kul{ 

struct StdStringComparator{
	public:
		bool operator()(std::string s1, std::string s2) const{
			return (s1.compare(s2) == 0);
		}
};
using namespace google;

typedef google::sparse_hash_map<std::string, std::string, std::tr1::hash<std::string>, StdStringComparator> StringToStringHashMap;

template <class T, class HashFcn, class EqualKey, class Alloc = libc_allocator_with_realloc<T> >
class HashSet : google::sparse_hash_set<T, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_set<T, HashFcn, EqualKey, Alloc> Hash;
		typedef typename Hash::size_type size_type;
		typedef typename Hash::key_type key_type;
		typedef typename Hash::iterator iterator;
		typedef typename Hash::const_iterator const_iterator;

		std::pair<iterator, bool> 	insert(const T obj) 									{ return Hash::insert(obj); }
		T&							operator[](const key_type& key)							{ return Hash::operator[](key); }
		size_type 					count(const key_type& key) 						const   { return Hash::count(key); }
		size_type 					erase(const key_type& key) 								{ return Hash::erase(key); }
		iterator 					begin()               									{ return Hash::begin(); }
		iterator 					end()       											{ return Hash::end(); }
		const_iterator 				begin() 										const   { return Hash::begin(); }
		const_iterator 				end() 											const   { return Hash::end(); }
		size_type 					size() 											const   { return Hash::size(); }
};
typedef HashSet<std::string, std::tr1::hash<std::string>, StdStringComparator> StringHashSet;

template <class K, class V, class HashFcn, class EqualKey, class Alloc = libc_allocator_with_realloc<std::pair<K, V> > >
class HashMap : google::sparse_hash_map<K, V, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<K, V, HashFcn, EqualKey> map;
		typedef typename map::size_type size_type;
		typedef typename map::key_type key_type;
		typedef typename map::iterator iterator;
		typedef typename map::const_iterator const_iterator;
		std::pair<iterator, bool> 	insert(const std::pair<K, V>& obj)			{ return map::insert(obj); }
		std::pair<iterator, bool> 	insert(const K k, V v)						{ return insert(std::pair<K, V>(k, v)); }
		V&							operator[](const key_type& key)				{ return map::operator[](key); }
		size_type 					count(const key_type& key) 			const 	{ return map::count(key); }
		iterator 					begin()               						{ return map::begin(); }
		iterator 					end()       								{ return map::end(); }
		iterator 					find(const key_type& key)      				{ return map::find(key); }
		const_iterator 				begin() 							const   { return map::begin(); }
		const_iterator 				end() 								const   { return map::end(); }
		const_iterator 				find(const key_type& key) 			const 	{ return map::find(key); }
};

template <class T, class HashFcn = std::tr1::hash<std::string>, class EqualKey = StdStringComparator, class Alloc = libc_allocator_with_realloc<std::pair<const std::string, T> > >
class StringToTGMap : google::sparse_hash_map<std::string, T, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<std::string, T, HashFcn, EqualKey, Alloc> StringToT;
		typedef typename StringToT::size_type size_type;
		typedef typename StringToT::key_type key_type;
		typedef typename StringToT::iterator iterator;
		typedef typename StringToT::const_iterator const_iterator;

		std::pair<iterator, bool> 	insert(const std::pair<std::string, T>& obj) 			{ return StringToT::insert(obj); }
		std::pair<iterator, bool> 	insert(const std::string& s, const T& t)				{ return insert(std::pair<std::string, T>(s, t)); }
		T&							operator[](const key_type& key)							{ return StringToT::operator[](key); }
		size_type 					count(const key_type& key) 						const   { return StringToT::count(key); }
		iterator 					begin()               									{ return StringToT::begin(); }
		iterator 					end()       											{ return StringToT::end(); }
		iterator 					find(const key_type& key)      							{ return StringToT::find(key); }
		const_iterator 				begin() 										const   { return StringToT::begin(); }
		const_iterator 				end() 											const   { return StringToT::end(); }
		const_iterator 				find(const key_type& key) 						const 	{ return StringToT::find(key); }
		size_type 					size() 											const   { return StringToT::size(); }
};

template <class T, class HashFcn = std::tr1::hash<std::string>, class EqualKey = StdStringComparator, class Alloc = libc_allocator_with_realloc<std::pair<const std::string, std::vector<T> > > >
class StringToVectorTGMap : google::sparse_hash_map<std::string, std::vector<T>, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<std::string, std::vector<T>, HashFcn, EqualKey, Alloc> StringToVector;
		typedef typename StringToVector::size_type size_type;
		typedef typename StringToVector::key_type key_type;
		typedef typename StringToVector::iterator iterator;
		typedef typename StringToVector::const_iterator const_iterator;
		std::pair<iterator, bool> 	insert(const std::pair<std::string, std::vector<T> >& obj) 			{ return StringToVector::insert(obj); }
		std::pair<iterator, bool> 	insert(const std::string& s, const std::vector<T>& t)				{ return insert(std::pair<std::string, std::vector<T> >(s, t)); }
		size_type 					count(const key_type& key) 									const   { return StringToVector::count(key); }
		iterator 					begin()               												{ return StringToVector::begin(); }
		iterator 					end()       														{ return StringToVector::end(); }
		iterator 					find(const key_type& key)      										{ return StringToVector::find(key); }
		const_iterator 				begin() 													const 	{ return StringToVector::begin(); }
		const_iterator 				end() 														const 	{ return StringToVector::end(); }
		const_iterator 				find(const key_type& key) 									const 	{ return StringToVector::find(key); }
		size_type 					size() 														const   { return StringToVector::size(); }
};

template <class T, class HashFcn = std::tr1::hash<std::string>, class EqualKey = StdStringComparator, class Alloc = libc_allocator_with_realloc<std::pair<const std::string, StringToTGMap<T> > > >
class StringToGMapTGMap : google::sparse_hash_map<std::string, StringToTGMap<T>, HashFcn, EqualKey>{
	public:
		typedef typename google::sparse_hash_map<std::string, StringToTGMap<T>, HashFcn, EqualKey> StringToGMapT;
		typedef typename StringToGMapT::size_type size_type;
		typedef typename StringToGMapT::key_type key_type;
		typedef typename StringToGMapT::iterator iterator;
		typedef typename StringToGMapT::const_iterator const_iterator;
		std::pair<iterator, bool> 	insert(const std::pair<std::string, StringToTGMap<T> >& obj) 		{ return StringToGMapT::insert(obj); }
		std::pair<iterator, bool> 	insert(const std::string s, StringToTGMap<T> t) 					{ return insert(std::pair<std::string, StringToTGMap<T> >(s, t)); }
		StringToTGMap<T>&			operator[](const key_type& key)										{ return StringToGMapT::operator[](key); }
		size_type 					count(const key_type& key) 									const   { return StringToGMapT::count(key); }
		iterator 					begin()               												{ return StringToGMapT::begin(); }
		iterator 					end()       														{ return StringToGMapT::end(); }
		iterator 					find(const key_type& key)      										{ return StringToGMapT::find(key); }
		const_iterator 				begin() 													const   { return StringToGMapT::begin(); }
		const_iterator 				end() 														const   { return StringToGMapT::end(); }
		const_iterator 				find(const key_type& key) 									const 	{ return StringToGMapT::find(key); }
};

};
#endif /* _GOOGLE_H_ */
