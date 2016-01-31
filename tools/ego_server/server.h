#pragma once

#include "http.h"

#include <ego/util/log/log.h>
#include <ego/util/string.h>
#include <ego/util/optional.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

#include <future>

namespace NEgo {

	class TServer {
	private:
		static constexpr ui32 ReceiveChunkSize = 1024;
		static constexpr ui32 SendChunkSize = 1024;

		static void* GetInAddr(struct sockaddr *sa)
		{
		    if (sa->sa_family == AF_INET) {
		        return &(((struct sockaddr_in*)sa)->sin_addr);
		    }

		    return &(((struct sockaddr_in6*)sa)->sin6_addr);
		}

		static int SendAll(int s, const char *buf, int *len) {
		    int total = 0;        // how many bytes we've sent
		    int bytesleft = *len; // how many we have left to send
		    int n;

		    while(total < *len) {
		        n = send(s, buf+total, bytesleft, 0);
		        if (n == -1) { break; }
		        total += n;
		        bytesleft -= n;
		    }

		    *len = total; // return number actually sent here

		    return n==-1?-1:0; // return -1 on failure, 0 on success
		} 
		
	
	public:

		using TRequestCallback = std::function<void(const THttpRequest&, TResponseBuilder&)>;

		TServer(ui32 port, ui32 max_connections = 10) {
			int status;
			struct addrinfo hints;

			memset(&hints, 0, sizeof hints); // make sure the struct is empty
			hints.ai_family = AF_UNSPEC;     // don't care IPv4 or IPv6
			hints.ai_socktype = SOCK_STREAM; // TCP stream sockets
			hints.ai_flags = AI_PASSIVE;     // fill in my IP for me

			std::stringstream pss;
			pss << port;
			std::string ps = pss.str();

			struct addrinfo *servinfo;
			if ((status = getaddrinfo(NULL, ps.c_str(), &hints, &servinfo)) != 0) {
			    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
			    exit(1);
			}

			for(struct addrinfo* p = servinfo; p != NULL; p = p->ai_next) {
		        if ((SocketNum = socket(p->ai_family, p->ai_socktype,
		                p->ai_protocol)) == -1) {
		            perror("server: socket");
		            continue;
		        }

				int yes=1;
		        if (setsockopt(SocketNum, SOL_SOCKET, SO_REUSEADDR, &yes,
		                sizeof(int)) == -1) {
		            perror("setsockopt");
		            exit(1);
		        }

		        if (bind(SocketNum, p->ai_addr, p->ai_addrlen) == -1) {
		            close(SocketNum);
		            perror("server: bind");
		            continue;
		        }

		        break;
		    }
			freeaddrinfo(servinfo);

			ENSURE(
				listen(SocketNum, max_connections) >= 0,
				"Failed to listen"
			);
		}

		TServer& AddCallback(TString method, TString path, TRequestCallback cb) {
			auto pathsPtr = Callbacks.find(method);
			if(pathsPtr == Callbacks.end()) {
				pathsPtr = Callbacks.insert(MakePair(method, std::map<TString, TRequestCallback>())).first;
			}
			auto res = pathsPtr->second.insert(MakePair(path, cb));
			ENSURE(res.second, "Found duplicates for callbacks " << method << "->" << path);
			return *this;
		}

		TServer& AddCallback(TString method, TRequestCallback cb) {
			auto res = DefaultCallbacks.insert(MakePair(method, cb));
			ENSURE(res.second, "Found duplicates for default callback for method " << method);
			return *this;
		} 
		
		void MainLoop() {
			while (true) {
				struct sockaddr_storage their_addr;
				socklen_t addr_size = sizeof(their_addr);

				int new_fd = accept(SocketNum, (struct sockaddr *)&their_addr, &addr_size);
				ENSURE(new_fd >= 0, "Failed to accept to socket");

				char s[INET6_ADDRSTRLEN];
				inet_ntop(their_addr.ss_family,
            		GetInAddr((struct sockaddr *)&their_addr),
            		s, sizeof s);

				L_DEBUG << "Server: got connection from " << s;

				Receive(new_fd);
        		close(new_fd);

	            // std::thread(
	            // 	[new_fd, this]() {
	            // 		Receive(new_fd);
	            // 		close(new_fd);
	            // 	}
	            // ).detach();
			}
		}

		void Receive(int socket) {
			std::stringstream ss;
			ui32 bytesReceived = 0;
			while (true) {
				char chunk[ReceiveChunkSize+1];

				int chunkReceived = recv(socket, chunk, ReceiveChunkSize, 0);
				if (chunkReceived < 0) {
					perror("Some errors while reading recv socket");
					exit(0);
				}

				if (chunkReceived == 0) {
					break;
				}

				bytesReceived += chunkReceived;

				if (chunkReceived < ReceiveChunkSize) {
					chunk[chunkReceived] = '\0';
					ss << std::string(chunk);
					break;
				} else {
					chunk[chunkReceived+1] = '\0';
					ss << std::string(chunk);
				}
			}
			L_DEBUG << "Bytes received: " << bytesReceived;

			THttpRequest req = ParseHttpRequest(ss.str());
			
			TString path = NStr::LStrip(req.Path, "/");
			
			TOptional<TRequestCallback> cb;
			auto methodCbPtr = Callbacks.find(req.Method);
			if (methodCbPtr != Callbacks.end()) {
				auto pathCbPtr = methodCbPtr->second.find(path);
				if (pathCbPtr != methodCbPtr->second.end()) {
					cb = pathCbPtr->second;
				}
			}
			if (!cb) {
				auto defCbPtr = DefaultCallbacks.find(req.Method);
				ENSURE(defCbPtr != DefaultCallbacks.end(), "Can't find appropriate callback for method " << req.Method);
				cb = defCbPtr->second;
			}
			
			TResponseBuilder respBuilder(req);
			THttpResponse resp;
			
			try {
				(*cb)(req, respBuilder);
				resp = respBuilder
					.Good()
				    .FormResponse();
			} catch (const TEgoFileNotFound& e) {
				resp = respBuilder
					.Body(e.what())
					.NotFound()
				    .FormResponse();
			} catch (const std::exception& e) {
				L_DEBUG << "Internal error: " << e.what();
				resp = respBuilder
					.Body(e.what())
					.InternalError()
				    .FormResponse();
			}
			
			L_DEBUG << req.Method << " " << req.Path << " -> " << resp.Code << " " << resp.Status;

			std::ostringstream respStr;
			respStr << resp;
			TString respStrInst = respStr.str();
			const char *respStrArray = respStrInst.c_str();
			int len = respStrInst.size();
			ENSURE(SendAll(socket, respStrArray, &len) >= 0, "Failed to send data");
			L_DEBUG << "Successfully sent " << respStrInst.size() << " bytes of full message size and " << resp.Body.size() << " bytes of content length";
		}

		~TServer() {
			close(SocketNum);
		}

	private:
		int SocketNum;
		std::map<TString, TRequestCallback> DefaultCallbacks;
		std::map<TString, std::map<TString, TRequestCallback>> Callbacks;
	};

} // namespace NEgo