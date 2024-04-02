#define PORT 8080
#include <algorithm>
#include <limits>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sstream>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>

using namespace std;

#define INT_MAX numeric_limits<int>::max()
#ifndef BUFFER_SIZE
# define BUFFER_SIZE 4096
#endif

int main() {
	int newSocket;
	int serverFd = socket(AF_INET, SOCK_STREAM, 0);
	struct sockaddr sockAddr = {};
	char buffer[BUFFER_SIZE] = { 0 };
	socklen_t addrLen = sizeof(sockAddr);
	const char* hello = "<html><body><h1> 404 NOT FOUND </h1></body><html>";
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
	if (listen(serverFd, INT_MAX) < 0) {
		perror("listen");
		exit(EXIT_FAILURE);
	}

	while (true) {
		string fullResponse("HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");
		if ((newSocket = accept(serverFd, &sockAddr, &addrLen))
				< 0) {
			perror("accept");
			exit(EXIT_FAILURE);
		}
		int ret = recv(newSocket, buffer, BUFFER_SIZE, 0);
		if (ret == -1) {
			perror("recv");
			exit(EXIT_FAILURE);
		}
		buffer[ret] = 0;
		stringstream copy(buffer);
		string line;
		
		getline(copy, line);

		if (strncmp(line.c_str(), "GET", 3) != 0) {
			cout << "so aceito GET" << endl;
			exit(EXIT_FAILURE);
		}

		string::iterator space(find(line.begin(), line.end(), line.c_str()[3]));
		string path(space + 1, find(space + 1, line.end(), line.c_str()[3]));

		if (path == "/index.html") {
			ifstream index("./index.html");
			string content;

			getline(index, content, '\0');
			index.close();
			fullResponse += content;
			send(newSocket, fullResponse.c_str(), fullResponse.size(), 0);
			close(newSocket);
			continue;
		}
		fullResponse += hello;
		send(newSocket, fullResponse.c_str(), fullResponse.size(), 0);
		close(newSocket);
	}
	close(serverFd);

	return 0;
}
