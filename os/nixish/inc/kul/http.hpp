/**

./src/kul/http.hpp

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
#ifndef _KUL_NIX_HTTP_HPP_
#define _KUL_NIX_HTTP_HPP_

#include "kul/http.base.hpp"

#include <netdb.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

namespace kul{ namespace http{

class Server : public kul::http::AServer{
	public:
		Server(const short& p) : AServer(p){}
		void start();
};

class Client : public kul::http::AClient{
	private:
		int sockfd;
	public:
		Client() : AClient(){}
		~Client(){}
		void start(const std::string& u, const short& p);
		void send(const ARequest& r) const;
};

}}
#endif /* _KUL_NIX_HTTP_HPP_ */
