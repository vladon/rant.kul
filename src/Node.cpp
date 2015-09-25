/**

./src/Node.cpp

Created on: 14 Jan 2013

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
#include "kul/xml.hpp"

const kul::xml::Node& kul::xml::Node::operator[](const std::string& s) const  throw (Exception){
	for(const Node& n : this->children())
		if(s.compare(n.name()) == 0)
			return n;
	KEXCEPT(Exception, "XML Exception: Element " + s + " doesn't exist under node " + this->name());
}

const kul::xml::Node& kul::xml::Node::operator()(const std::string& c, const std::string& a, const std::string& v) const throw (Exception){
	for(const Node& n : this->children())
		if(c.compare(n.name()) == 0){
			try{
				if(n.att(a).compare(v) == 0) return n;
			}catch(const Exception& e){ }
		}
	KEXCEPT(Exception, "XML Exception: No Element " + c + " contains attribute " + a + " with value " + v + " under node " + this->name());
}

const std::string kul::xml::Node::att(const std::string& s) const throw (Exception){
	for(const std::pair<const std::string, std::string> p : this->attributes())
		if(s.compare(p.first) == 0)
			return p.second;
	KEXCEPT(Exception, "XML Exception: Attribute " + s + " doesn't exist under node " + this->name());
}

bool kul::xml::Node::hasChild(const std::string& s) const{
	try{
		(*this)[s];
		return 1;
	}catch(const kul::xml::Exception& e){}
	return 0;
}

bool kul::xml::Node::hasAtt(const std::string& s) const{
	try{
		this->att(s);
		return 1;
	}catch(const kul::xml::Exception& e){}
	return 0;
}

