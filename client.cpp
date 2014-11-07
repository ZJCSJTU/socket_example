// Compile: g++ client.cpp -lsocket -lnsl
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>

#include <cstdlib>
#include <cstring>

#include <iostream>

using namespace std;

static const char * data = "Hello World";

/*
 * This program creates a socket and initiates a connection with the
 * socket given in the command line. Some data are sent over the
 * connection and then the socket is closed, ending the connection.
 * The form of the command line is: client hostname portnumber
 * Usage: pgm host port
 */
int main(int argc, char *argv[]) {
	int sock;
	struct sockaddr_in server;
	struct hostent *hp;
	int rval;

	if (argc < 3) {
		cout << "Usage: " << argv[0] << " <host> <socket>" << endl;
		exit(EXIT_FAILURE);
	}
	
	/* Create socket. */
	sock = socket(AF_INET, SOCK_STREAM, 0);
	if (sock == -1) {
		cerr << "opening stream socket: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}
	
	/* Connect socket using name specified by command line. */
	server.sin_family = AF_INET;
	hp = gethostbyname(argv[1]);
	
	/*
	 * gethostbyname returns a structure including the network address
	 * of the specified host.
	 */
	if (hp == nullptr) {
		cerr << argv[1] << ": unknown host" << endl;
		exit(EXIT_FAILURE);
	}

	memcpy(reinterpret_cast<char *>(&server.sin_addr),
			reinterpret_cast<char *>(hp->h_addr), hp->h_length);

	/* Set the client to connect to the given server port */
	server.sin_port = htons(atoi(argv[2]));

	/* Actually connect to the desired server port */
	rval = connect(sock, reinterpret_cast<struct sockaddr *>(&server),
			sizeof(server));
	if (rval == -1) {
		cerr << "connecting stream socket: " << strerror(errno) << endl;
		exit(EXIT_FAILURE);
	}

	int nsent = 0;
	int ntosend = strlen(data)+1;
	const char *data_ptr = data;
	/* 
	 * We have to send in a loop, because send doesn't guarantee that it will
	 * successfully send all the characters we want to!
	 */
	while (ntosend != 0) {

		/* Send any unsent data bytes remaining */
		rval = send(sock, data_ptr + nsent, ntosend, 0);
		if (rval == -1) {
			cerr << "sending on stream socket: " << strerror(errno) << endl;
			exit(EXIT_FAILURE);
		}

		ntosend -= rval;
		nsent += rval;
	}

	close(sock);

	return EXIT_SUCCESS;
}

