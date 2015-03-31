/**

./src/kul/http.cpp

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
#include "kul/def.hpp"
#include "kul/log.hpp"
#include "kul/http.hpp"

void kul::http::Server::start() const throw(kul::http::Exception){
	ULONG RequestBufferLength 	= sizeof(HTTP_REQUEST) + 2048;
	PCHAR pRequestBuffer 		= (PCHAR) wAlloc(RequestBufferLength);
	if(pRequestBuffer == NULL)
		KEXCEPT(Exception, "Buffer allocation failed: " + ERROR_NOT_ENOUGH_MEMORY);

	PHTTP_REQUEST req = (PHTTP_REQUEST) pRequestBuffer;
	HTTP_REQUEST_ID requestId;
	HTTP_SET_NULL_ID(&requestId);
	DWORD bytesRead;
	ULONG r = 0;
	while(true){
		RtlZeroMemory(req, RequestBufferLength);

		r = HttpReceiveHttpRequest(
			this->q,			// Req Queue
			requestId,			// Req ID
			0,	 				// Flags
			req,				// HTTP request buffer
			RequestBufferLength,// req buffer length
			&bytesRead,			// bytes received
			NULL);				// LPOVERLAPPED

		if(r == NO_ERROR){
			if(req->Verb == HttpVerbGET){
				//getters.respond
			}else
			if(req->Verb == HttpVerbPOST){
			}
			else{

			}
		}else
		if(r == ERROR_MORE_DATA){
			requestId = req->RequestId;
			RequestBufferLength = bytesRead;
			wFreeM(pRequestBuffer);
			pRequestBuffer = (PCHAR) wAlloc(RequestBufferLength);
		}else if(r == ERROR_CONNECTION_INVALID && !HTTP_IS_NULL_ID(&requestId)){
			HTTP_SET_NULL_ID(&requestId);
		}else KEXCEPT(Exception, "HttpReceiveHttpRequest failed: " + r);
	}
}

bool kul::http::Server::doGet(IN PHTTP_REQUEST req){
	//if(gMap.find(req.name)) gMap[req.name].do(req.val)
	if(1){
		KLOG(INF) << req->RequestId;
		KLOG(INF) << req->CookedUrl.pFullUrl;
		KLOG(INF) << req->pEntityChunks;
	}
	else KLOG(ERR) << "No GET response for " + req->RequestId;
	return true;
}
bool kul::http::Server::doPost(IN PHTTP_REQUEST req){
	//if(pMap.find(req.name)) pMap[req.name].do(req.val)
	//else KLOG(ERR) << "No POST response for " + req.name;
	return true;
}


void kul::http::Client::start(const std::string& u, const short& p){}

void kul::http::Client::send(const ARequest& r) const{}

void kul::http::GetRequest::send(const int& sockfd) const{}

void kul::http::PostRequest::send(const int& sockfd) const{}
