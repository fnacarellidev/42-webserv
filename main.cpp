#define PORT 8080
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

#ifndef BUFFER_SIZE
# define BUFFER_SIZE 4096
#endif

int main() {
	int newSocket;
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr sockAddr = {};
	char buffer[BUFFER_SIZE] = { 0 };
	socklen_t addrLen = sizeof(sockAddr);
	const char* hello = "<html><body>\n<h1> Hello </h1>\n</body><html>";
	int opt = 1;

	if (serverFd < 0) {
		perror("socket failed");
		exit(EXIT_FAILURE);
	}
	if (setsockopt(serverFd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt))) {
		perror("setsockopt");
		exit(EXIT_FAILURE);
	}
	// memset(&sockAddr, 0, sizeof(sockAddr));
	sockAddr.sa_family = AF_INET;
	unsigned short port = htons(8080);
	memcpy(&sockAddr.sa_data, &port, sizeof(port));
	if (bind(serverFd, &sockAddr, sizeof(sockAddr)) < 0) {
		perror("bind failed");
		exit(EXIT_FAILURE);
	}
	if (listen(serverFd, 3) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	std::string fullResponse("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");

	fullResponse += hello;
	while (true) {
		// aceita o pacote sempre q surgir um request
		if ((newSocket = accept(serverFd, &sockAddr, &addrLen))
				< 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		// le o request
		recv(newSocket, buffer, BUFFER_SIZE, 0);

		//manda resposta com o html completo
		send(newSocket, fullResponse.c_str(), fullResponse.size(), 0);
		close(newSocket);
	}
	close(serverFd);

	return 0;
}
