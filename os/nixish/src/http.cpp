/**

./src/kul/OS.cpp

Created on: 10 Sept 2014

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
#include "kul/byte.hpp"
#include "kul/http.hpp"

void kul::http::Server::start() throw(kul::http::Exception){
	int32_t sockfd, newsockfd, portno;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int  n;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		KEXCEPT(Exception, "HTTP Server error opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	portno = this->port();
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = kul::byte::Endian::UINT32(portno);
	
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		KEXCEPT(Exception, "HTTP Server error on binding");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			KEXCEPT(Exception, "HTTP Server error on accept");

		bzero(buffer,256);
		n = read(newsockfd, buffer, 255);
		if (n < 0) perror("ERROR reading from socket");

		std::string b(buffer);
		std::vector<std::string> lines = kul::String::lines(b); 
		std::vector<std::string> l0 = kul::String::split(lines[0], ' ');
		std::string s(l0[1]);
		std::string a;
		if(l0[0].compare("GET") == 0){
			if(s.find("?") != std::string::npos){ //GET
				a = s.substr(s.find("?") + 1);
				s = s.substr(0, s.find("?"));
			}
		}else 
		if(l0[0].compare("POST") == 0){
			bool lb = 0;
			for(uint i = 1; i < lines.size(); i++){
				if(lines[i].size() <= 1 && !lb) lb = 1;
				else if(lb) a += lines[i];
			}
		}else 
		if(l0[0].compare("HEAD") == 0){

		}else
			KEXCEPT(Exception, "HTTP Server request type not handled :" + l0[0]);

		std::string h(handle(s, a));
		n = write(newsockfd, h.c_str(), h.length());
		close(newsockfd);
	}
}

void kul::http::_1_1GetRequest::send(const std::string& h, const int p, const std::string& res){
	struct sockaddr_in servAddr;
	int32_t sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
		KEXCEPT(Exception, "HTTPGet error opening socket");

	std::string hh(h);
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family   = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port	   = kul::byte::Endian::INT16(p);
	int host = getnameinfo((struct sockaddr *) &servAddr, 
							sizeof(struct sockaddr), &hh[0], h.size(), 
							0, 0, NI_NUMERICHOST | NI_NUMERICSERV);
	if(host < 0)
		KEXCEPT(Exception, "HTTPGet No such host: " + h);
	if(connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		KEXCEPT(Exception, "HTTPGet request failed to connect");
	std::string req(toString(h, res));
	::send(sockfd, req.c_str(), req.size(), 0); 
	char buffer[10000];
	int nDataLength;
	std::string s;
	while((nDataLength = recv(sockfd,buffer,10000,0)) > 0){
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
			s += buffer[i];//do something with buffer[i]
			i += 1;
		}
		memset(&buffer[0], 0, sizeof(buffer));
	}
	handle(s);
	close(sockfd);
}

void kul::http::_1_1PostRequest::send(const std::string& h, const int p, const std::string& res){
	struct sockaddr_in servAddr;
	int32_t sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sockfd < 0)
		KEXCEPT(Exception, "HTTPGet error opening socket");
	std::string hh(h);
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family   = AF_INET;
	servAddr.sin_addr.s_addr = INADDR_ANY;
	servAddr.sin_port	   = kul::byte::Endian::INT16(p);
	int host = getnameinfo((struct sockaddr *) &servAddr, 
							sizeof(struct sockaddr), &hh[0], h.size(), 
							0, 0, NI_NUMERICHOST | NI_NUMERICSERV);
	if(host < 0)
		KEXCEPT(Exception, "HTTPGet No such host: " + h);
	if(connect(sockfd, (struct sockaddr *) &servAddr, sizeof(servAddr)) < 0)
		KEXCEPT(Exception, "HTTPGet request failed to connect");
	std::string req(toString(h, res));
	::send(sockfd, req.c_str(), req.size(), 0); 
	char buffer[10000];
	int nDataLength;
	std::string s;
	while((nDataLength = recv(sockfd,buffer,10000,0)) > 0){
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') {
			s += buffer[i];//do something with buffer[i]
			i += 1;
		}
		memset(&buffer[0], 0, sizeof(buffer));
	}
	handle(s);
	close(sockfd);
}
