#pragma once

#include "http.h"

#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/wait.h>
#include <signal.h>

#include <ego/util/log/log.h>
#include <ego/util/string.h>

namespace NEgo {

	void sigchld_handler(int s) {
	    // waitpid() might overwrite errno, so we save and restore it:
	    int saved_errno = errno;

	    while(waitpid(-1, NULL, WNOHANG) > 0);

	    errno = saved_errno;
	}

	class TServer {
	private:
		static constexpr ui32 ReceiveChunkSize = 1024;

		static void* GetInAddr(struct sockaddr *sa)
		{
		    if (sa->sa_family == AF_INET) {
		        return &(((struct sockaddr_in*)sa)->sin_addr);
		    }

		    return &(((struct sockaddr_in6*)sa)->sin6_addr);
		}

	public:
		using TRequestCallback = std::function<void(std::ostream&)>;

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

			struct sigaction sa;
			sa.sa_handler = sigchld_handler; // reap all dead processes
		    sigemptyset(&sa.sa_mask);
		    sa.sa_flags = SA_RESTART;
		    if (sigaction(SIGCHLD, &sa, NULL) == -1) {
		        perror("sigaction");
		        exit(1);
		    }


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

				if (!fork()) { // this is the child process
		            close(SocketNum); // child doesn't need the listener
		            Receive(new_fd);
		            close(new_fd);
		            exit(0);
		        }
        		close(new_fd);
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
			THttpRequest req = ParseHttpRequest(ss.str());

			L_DEBUG << req;
		}

		~TServer() {
			close(SocketNum);
		}

	private:
		int SocketNum;
		std::map<TString, std::map<TString, TRequestCallback>> Callbacks;
	};

} // namespace NEgo