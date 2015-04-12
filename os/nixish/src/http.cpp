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
#include "kul/bash.hpp"
#include "kul/byte.hpp"
#include "kul/http.hpp"

#include <arpa/inet.h>

void kul::http::Server::start() throw(kul::http::Exception){
	int32_t sockfd, newsockfd;
	socklen_t clilen;
	char buffer[256];
	struct sockaddr_in serv_addr, cli_addr;
	int  r;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if (sockfd < 0)
		KEXCEPT(Exception, "HTTP Server error opening socket");

	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = kul::byte::isBigEndian() ? htons(this->port()) : kul::byte::LittleEndian::UINT32(this->port());
	
	if (bind(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0)
		KEXCEPT(Exception, "HTTP Server error on binding");

	listen(sockfd, 5);
	clilen = sizeof(cli_addr);
	while(1){
		newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
		if (newsockfd < 0)
			KEXCEPT(Exception, "HTTP Server error on accept");

		bzero(buffer,256);
		r = read(newsockfd, buffer, 255);
		if (r < 0) perror("ERROR reading from socket");

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
		std::string ret("HTTP/1.1 200 OK\r\n");
		ret += "Content-Type: text/html\r\n";
		ret += "Server: kserv\r\n";
		ret += "Date: " + kul::DateTime::NOW() + "\r\n";
		ret += "Connection: close\r\n";
		ret += "Content-Length: " + std::to_string(h.size()) + "\r\n\r\n";
		ret += h;
		r = write(newsockfd, ret.c_str(), ret.length());
		close(newsockfd);
	}
}

void kul::http::_1_1GetRequest::send(const std::string& h, const int& p, const std::string& res){
	int32_t sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sck < 0)
		KEXCEPT(Exception, "HTTP GET error opening socket");
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family   = AF_INET;
	int r = 0;
	if(h.compare("localhost") == 0 || h.compare("127.0.0.1") == 0){
		servAddr.sin_port = kul::byte::isBigEndian() ? htons(p) : kul::byte::LittleEndian::UINT32(p);
		servAddr.sin_addr.s_addr = INADDR_ANY;
		r = connect(sck, (struct sockaddr*) &servAddr, sizeof(servAddr));
	}
	else{
		std::vector<std::string> ips(kul::bash::digHostIPv4(h));
		if(ips.empty()) KEXCEPT(Exception, "HTTP GET No such host: " + h);
		servAddr.sin_port = htons(p);
		for(std::string& ip : ips){
			servAddr.sin_addr.s_addr = inet_addr(&ip[0]);
			if((r = connect(sck, (struct sockaddr*) &servAddr, sizeof(servAddr))) == 0) break;
		}
	}
	if(r < 0) KEXCEPT(Exception, "HTTP GET failed to connect to host: " + h);
	std::string req(toString(h, res));
	::send(sck, req.c_str(), req.size(), 0); 
	char buffer[10000];
	std::string s;
	while(recv(sck, buffer, 10000, 0) > 0){
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') s += buffer[i++];
		memset(&buffer[0], 0, sizeof(buffer));
	}
	handle(s);
	close(sck);
}

void kul::http::_1_1PostRequest::send(const std::string& h, const int& p, const std::string& res){
	int32_t sck = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sck < 0)
		KEXCEPT(Exception, "HTTP GET error opening socket");
	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family   = AF_INET;
	int r = 0;
	if(h.compare("localhost") == 0 || h.compare("127.0.0.1") == 0){
		servAddr.sin_port = kul::byte::isBigEndian() ? htons(p) : kul::byte::LittleEndian::UINT32(p);
		servAddr.sin_addr.s_addr = INADDR_ANY;
		r = connect(sck, (struct sockaddr*) &servAddr, sizeof(servAddr));
	}
	else{
		std::vector<std::string> ips(kul::bash::digHostIPv4(h));
		if(ips.empty()) KEXCEPT(Exception, "HTTP GET No such host: " + h);
		servAddr.sin_port = htons(p);
		for(std::string& ip : ips){
			servAddr.sin_addr.s_addr = inet_addr(&ip[0]);
			if((r = connect(sck, (struct sockaddr*) &servAddr, sizeof(servAddr))) == 0) break;
		}
	}
	if(r < 0) KEXCEPT(Exception, "HTTP GET failed to connect to host: " + h);
	std::string req(toString(h, res));
	::send(sck, req.c_str(), req.size(), 0); 
	char buffer[10000];
	std::string s;
	while(recv(sck, buffer, 10000, 0) > 0){
		int i = 0;
		while (buffer[i] >= 32 || buffer[i] == '\n' || buffer[i] == '\r') s += buffer[i++];
		memset(&buffer[0], 0, sizeof(buffer));
	}
	handle(s);
	close(sck);
}
