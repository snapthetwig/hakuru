
void sendToSocket(std::string passedString) {

	time_t rawTime;
	struct tm* timeInfo;
	time(&rawTime);
	timeInfo = gmtime(&rawTime);

	int rc;
	std::string tempString;
	tempString = "HTTP/1.1 200 OK\r\n";
	tempString = tempString + "Date: " + asctime(timeInfo);
	//tempString = tempString + "Server: Hakuru/0.1\r\n";
	tempString = tempString + "Content-Length: " + toString((unsigned long)passedString.length()) + "\r\n";
	//tempString = tempString + "Connection: Closed\r\n";
	tempString = tempString + "Content-Type: application/json\r\n";
	//rc = send(currentServer.sockCounter, tempString.c_str(), tempString.length(), 0);
	passedString = tempString + "\r\n" + passedString + "\r\n";
	//passedString = passedString + "\r\n";
	rc = send(currentServer.sockCounter, passedString.c_str(), passedString.length(), 0);
	if (rc < 0) {
		perror("send failed");
	}

}

void listener() {
	int on = 1, rc, dReady;
	bool endServer = false;
	bool closeConnection = false;

	currentServer.timeout.tv_sec = 5;
	currentServer.timeout.tv_usec = 0;

	if ((currentServer.sock = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
		perror("socket");
		exit(1);
	}

	/*
	if (setsockopt(currentServer.sock, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) == -1) {
		perror ("ioctl failure");
		close(currentServer.sock);
		exit(1);
	}
*/

	if (setsockopt(currentServer.sock, SOL_SOCKET, SO_RCVTIMEO, (char *)&(currentServer.timeout), sizeof(currentServer.timeout)) == -1) {
		perror ("ioctl timeout failure");
		close(currentServer.sock);
		exit(1);
	}

	if (ioctl(currentServer.sock, FIONBIO, (char *)&on) == -1) {
		perror ("ioctl failure");
		close(currentServer.sock);
		exit(1);
	}
	
	currentServer.address.sin_family = AF_INET;
	currentServer.address.sin_port = htons(serverPort);
	currentServer.address.sin_addr.s_addr = INADDR_ANY;
	bzero(&(currentServer.address.sin_zero), 8);
	
	if (bind(currentServer.sock, (struct sockaddr *)&(currentServer.address), sizeof(struct sockaddr)) == -1) {
		perror("bind");
		close(currentServer.sock);
		exit(1);
	}
	
	if (listen(currentServer.sock, maxSockets) == -1) {
		perror("listen");
		close(currentServer.sock);
		exit(1);
	}

	/*
	if ((currentServer.newSock = accept(currentServer.sock, (struct sockaddr *)&(currentServer.clientAddress), &(currentServer.sinSize))) == -1) {
		perror("accept");
	}

	fcntl(currentServer.lastSock, F_SETFL, O_NONBLOCK);
	fcntl(currentServer.newSock, F_SETFL, O_NONBLOCK);
	*/

	FD_ZERO(&(currentServer.masterSet));
	currentServer.lastSock = currentServer.sock;
	FD_SET(currentServer.sock, &(currentServer.masterSet));

	while (1) {

		memcpy(&(currentServer.workingSet), &(currentServer.masterSet), sizeof(currentServer.masterSet));
		rc = select(currentServer.lastSock + 1, &(currentServer.workingSet), NULL, NULL, &(currentServer.timeout));

		dReady = rc;

		for (currentServer.sockCounter = 0; currentServer.sockCounter <= currentServer.lastSock && dReady > 0; currentServer.sockCounter++) {
			
			if (FD_ISSET(currentServer.sockCounter, &(currentServer.workingSet))) {

				dReady -= 1;

				if (currentServer.sockCounter == currentServer.sock) {

					do {

						currentServer.newSock = accept(currentServer.sock, NULL, NULL);
						if (currentServer.newSock == -1) {
							perror("accept failed");
							break;
						}
						printf("New connection - %d\n", currentServer.newSock);
						FD_SET(currentServer.newSock, &(currentServer.masterSet));
						if (currentServer.newSock > currentServer.lastSock) {
							currentServer.lastSock = currentServer.newSock;
						}
						
					} while (currentServer.newSock != -1);

				} else {

					closeConnection = false;

					while (1) {

						printf("Read ready %d\n", currentServer.sockCounter);

						rc = recv(currentServer.sockCounter, currentServer.lastRead, sizeof(currentServer.lastRead), 0);
						if (rc < 0) {
							if (errno != EWOULDBLOCK) {
								perror("recv failed");
								closeConnection = true;
							}
							break;
						}
						if (rc == 0) {
							printf("Connection closed \n");
							closeConnection = true;
							break;
						}
						currentServer.readSize = rc;

						std::string tempString(currentServer.lastRead);
						std::cout << tempString<< " : was read (" << rc << ")\n";

						//////////////
						// Where we call for handling the HTTP request
						lowercase(&tempString);
						std::cout << "p1\n";
						interpretHttp(&tempString);
						std::cout << "p2\n";
						//std::cout << tempString<< " : was read\n";
						//////////////


						closeConnection = true;
						break;
						//}

					}

					if (closeConnection) {
						close(currentServer.sockCounter);
						FD_CLR(currentServer.sockCounter, &(currentServer.masterSet));

						if (currentServer.sockCounter == currentServer.lastSock) {

							while (FD_ISSET(currentServer.lastSock, &(currentServer.masterSet)) == false) {
								currentServer.lastSock -= 1;
							}

						}

					}

				}

			}


		}
	}


}

