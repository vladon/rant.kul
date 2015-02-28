/**

./src/XPather.cpp

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

void kul::xml::XPather::writeNode(const char*x, const Node& node){

}

void kul::xml::XPather::writeText(const char*x, const char*t){
	//doc.select_single_node(x).node().set_value(t);	
	doc.select_single_node(x).node().append_child(pugi::node_pcdata).set_value(t);	
}

void kul::xml::XPather::writeAttr(const char*x, const char*n, const char*v){
	
}
