// Compile: g++ server.cpp -lsocket
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

#include <array>
#include <iostream>

using namespace std;

int main()
{
	int sock;
	struct sockaddr_in server;
	int msgsock;
	array<char, 1024> buf;
	int rval;
	
	
	/* Create socket. */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		cerr << "opening stream socket " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	/* Bind socket using wildcards.*/
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = 0;
	rval = bind(sock, reinterpret_cast<sockaddr *>(&server), sizeof(server));
	if (rval == -1) {
		cerr << "binding stream socket " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	/* Find out assigned port number and print it out. */
	socklen_t length = sizeof(server);
	rval = getsockname(sock, reinterpret_cast<sockaddr *>(&server), &length);
	if (rval == -1) {
		cerr << "getting socket name " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	int server_port = ntohs(server.sin_port);
	cout << "Socket port #" << server_port << endl;
	
	/* Start accepting connections. */
	rval = listen(sock, 5);
	if (rval == -1) {
		cerr << "listening to socket: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	msgsock = accept(sock,(struct sockaddr *) 0,(socklen_t *) 0);
	if (msgsock == -1) {
		cerr << "accept: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	do {
		// Zero our buffer
		buf.fill(0);

		if ((rval = recv(msgsock, static_cast<void *>(buf.data()), buf.size(), 0)) == -1) {
			cerr << "reading stream message: " << strerror(errno) << endl;
		}

		if (rval == 0) {
			cout << "Ending connection" << endl;
		} else {
			cout << "-->" << string(buf.data()) << endl;
		}
	} while (rval != 0);

	close(msgsock);

	return 0;
}

