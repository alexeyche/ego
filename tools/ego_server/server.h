#pragma once

#include <ego/base/base.h>

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

namespace NEgo {

	void sigchld_handler(int s) {
	    // waitpid() might overwrite errno, so we save and restore it:
	    int saved_errno = errno;

	    while(waitpid(-1, NULL, WNOHANG) > 0);

	    errno = saved_errno;
	}

	class TServer {
	private:

		static void* GetInAddr(struct sockaddr *sa)
		{
		    if (sa->sa_family == AF_INET) {
		        return &(((struct sockaddr_in*)sa)->sin_addr);
		    }

		    return &(((struct sockaddr_in6*)sa)->sin6_addr);
		}	
	
	public:
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
			if ((status = getaddrinfo(NULL, ps.c_str(), &hints, &Servinfo)) != 0) {
			    fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
			    exit(1);
			}

			SocketNum = socket(Servinfo->ai_family, Servinfo->ai_socktype, Servinfo->ai_protocol);
			ENSURE(SocketNum != -1, "Failed to create socket");

			int yes=1;
			if (setsockopt(SocketNum, SOL_SOCKET,SO_REUSEADDR, &yes,sizeof(int)) == -1) {
			    perror("setsockopt");
			    exit(1);
			} 

			ENSURE(
				bind(SocketNum, Servinfo->ai_addr, Servinfo->ai_addrlen) >= 0, 
				"Failed to bind socket"
			);

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
		            if (send(new_fd, "Hello, world!", 13, 0) == -1) {
		            	perror("send");
		            }
		            close(new_fd);
		            exit(0);
		        }
        		close(new_fd);
        		
			}
		}

		~TServer() {
			close(SocketNum);
			freeaddrinfo(Servinfo);
		}

		int SocketNum;
		struct addrinfo *Servinfo;  
	};

} // namespace NEgo