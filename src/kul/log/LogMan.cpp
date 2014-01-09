/**

./src/kul/log/LogMan.cpp

Created on: 6 Dec 2013

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

const kul::xml::NodeValidator kul::log::LogNodeUser::validator(){
	using namespace kul::xml;

	std::vector<std::pair<std::string, NodeValidator> > rootMap;
	
	return NodeValidator(rootMap, 1, 1, false);
}

kul::log::LogNodeUser* kul::log::LogNodeUser::get(){
	if(!this->root()){
		this->reset(kul::xml::NodeFactory::create(this->file().c_str(), "kul-log", validator()));
	}
	using namespace kul::xml;
	//for(const Node* node : this->root()->children()){}

	return this;
}
