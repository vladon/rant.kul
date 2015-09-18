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
#ifndef _KUL_HTTP_HPP_
#define _KUL_HTTP_HPP_

#include "kul/log.hpp"
#include "kul/byte.hpp"
#include "kul/time.hpp"
#include "kul/http.base.hpp"

#include <netdb.h>
#include <stdio.h>
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>

namespace kul{ namespace http{

class Server : public kul::http::AServer{
	private:
		int32_t sockfd;
		socklen_t clilen;
		struct sockaddr_in serv_addr, cli_addr;

		std::queue<int> sockets;
	public:
		Server(const short& p, const std::string& w = "localhost") : AServer(p){}
		void start() throw(kul::http::Exception);
		bool started(){ return sockets.size(); }
		void stop();
		virtual const std::pair<kul::hash::set::String, std::string> handle(const std::string& res, kul::hash::map::S2S atts){
			using namespace std;
			kul::hash::set::String set;
			stringstream ss;
			ss << res << " : ";	
			for(const pair<string, string> p : atts)
				ss << p.first << "=" << p.second << " ";
			set.insert("Content-Length: " + std::to_string(ss.str().size()));
			return std::pair<kul::hash::set::String, std::string>(set, ss.str());
		}
};


}}
#endif /* _KUL_HTTP_HPP_ */
