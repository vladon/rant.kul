/**

./inc/kul/http.hpp

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

#include "kul/xml.hpp"
#include "kul/hash.hpp"

namespace kul{ namespace http{

class ARequest{
	private:
		const std::string m;
		kul::hash::map::S2S atts;
	public:
		ARequest(const std::string& m) : m(m){}
		virtual ~ARequest(){}
		virtual void send(const int& sockfd) const = 0;
		void addAttribute(const std::string& k, const std::string& v){
			atts[k] = v;
		}
		const std::string& 			message()	 const { return m; }
		const kul::hash::map::S2S& 	attributes() const { return atts; }
};

class AServer{
	protected:
		long int up;
		short p;
	public:
		AServer(const short& p) : p(p){}
		virtual ~AServer(){}
		virtual void start() = 0;
		const long int& upTime(){ return up; }
		const short& 	port()	{ return p; }
};
class AClient{
	public:
		AClient(){}
		virtual ~AClient(){}
		virtual void start(const std::string& u, const short& p) = 0;
		virtual void send(const ARequest& r) const = 0;
};


class GetRequest  : public ARequest{
	public:
		GetRequest(const std::string& m) : ARequest(m){}
		void send(const int& sockfd) const;
};
class PostRequest : public ARequest{
	public:
		PostRequest(const std::string& m) : ARequest(m){}
		void send(const int& sockfd) const;
};

class Response{};
class GetResponse  : public Response{};
class PostResponse : public Response{};


};};

#endif /* _KUL_HTTP_HPP_ */