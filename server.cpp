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

int main() {
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
	/* This is an internet socket */
	server.sin_family = AF_INET;
	/* Accepts any in address */
	server.sin_addr.s_addr = INADDR_ANY;
	/* Allow the system to specify a port for us */
	server.sin_port = 0;

	/* Bind the socket to our given address space */
	rval = bind(sock, reinterpret_cast<sockaddr *>(&server), sizeof(server));
	if (rval == -1) {
		cerr << "binding stream socket " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	/* Find out assigned port number and print it out. */
	socklen_t length = sizeof(server);
	/* Get the info about how this socket was bound */
	rval = getsockname(sock, reinterpret_cast<sockaddr *>(&server), &length);
	if (rval == -1) {
		cerr << "getting socket name " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	/* 
	 * Convert the network style port number from addr to a architecture
	 * compatible port number with ntohs (network to host short)
	 */
	int server_port = ntohs(server.sin_port);
	cout << "Socket port #" << server_port << endl;
	
	/* Start accepting connections. */
	rval = listen(sock, 5);
	if (rval == -1) {
		cerr << "listening to socket: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	/* Actually get a connection, this returns a new fd for our new connection */
	msgsock = accept(sock,(struct sockaddr *) 0,(socklen_t *) 0);
	if (msgsock == -1) {
		cerr << "accept: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	do {
		/* Zero our buffer */
		buf.fill(0);

		/* Read the bytes off the network socket */
		rval = recv(msgsock, static_cast<void *>(buf.data()), buf.size(), MSG_NOSIGNAL);
		if (rval == -1) {
			cerr << "reading stream message: " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}

		if (rval == 0) {
			cout << "Ending connection" << endl;
		} else {
			cout << "-->" << string(buf.data()) << endl;
		}
	} while (rval != 0);

	close(msgsock);

	return EXIT_SUCCESS;
}

