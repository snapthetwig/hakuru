
std::string getSessionKey(std::string publicKey) {
}

std::string getClientId(std::string publicKey) {

}

std::string addressToString(struct sockaddr_in passedAddress) {
	char tempAddress[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(passedAddress.sin_addr), tempAddress, INET_ADDRSTRLEN);
	std::string stringAddress(tempAddress);
	return stringAddress;
}

struct sockaddr_in stringToAddress(std::string passedAddress) {
	struct sockaddr_in netAddress;
	inet_pton(AF_INET, passedAddress.c_str(), &(netAddress.sin_addr));
	return netAddress;
}

struct Client* clientExists(std::string* passedId, std::string* passedKey) {

	bool exhausted = false;
	if (currentClient == NULL) {
		return NULL;
	}

	currentClient = currentClient->start;
	while (! exhausted) {

		if (currentClient->clientId.compare(*passedId) == 0 && currentClient->clientKey.compare(*passedKey) == 0) {
			return currentClient;
		}
		if (currentClient->next == NULL) {
			exhausted = true;
		} else {
			currentClient = currentClient->next;
		}
		std::cout << "s\n";

	}

	return NULL;

}

struct Client* addClient(std::string* passedId, std::string* passedKey) {
	if (currentClient == NULL) {
		currentClient = new Client;
		currentClient->start = currentClient;
	} else {
		currentClient = currentClient->start->prev;
		currentClient->next = new Client;
		currentClient->next->start = currentClient->start;
		currentClient->next->prev = currentClient;
		currentClient = currentClient->next;
	}

	std::cout << "addClient 1 \n";
	currentClient->clientId = *passedId;
	currentClient->clientKey = *passedKey;

	currentClient->start->prev = currentClient;

	//tailClient = currentClient;

	return currentClient;
}

struct Session* addSession(struct Client* passedClient) {
	if (passedClient->session == NULL) {
		passedClient->session = new Session;
		passedClient->session->start = passedClient->session;
		passedClient->session->sessionId = 1;
	} else {
		passedClient->session = passedClient->session->start->prev;
		passedClient->session->next = new Session;
		passedClient->session->next->start = passedClient->session->start;
		passedClient->session->next->prev = passedClient->session;
		passedClient->session = passedClient->session->next;
		passedClient->session->sessionId = passedClient->session->prev->sessionId + 1;
	}

	passedClient->session->start->prev = passedClient->session;

	return passedClient->session;
	
}


bool verifyClient() {
	struct Session* newSession = NULL;
	struct Client* tempClient = NULL;
	bool verified = false;

	tempClient = clientExists(&(currentParams.clientId), &(currentParams.clientKey));
	if (tempClient != NULL) {
		newSession = addSession(tempClient);
		verified = true;
	} else {

		std::string clientFilePath = dataPath + "clients/key/" + currentParams.clientId;
		if (! std::filesystem::exists(clientFilePath)) {
			return false;
		}
	
		std::ifstream clientFile(clientFilePath);
		std::string line;
		int counter;
		if (clientFile.is_open()) {
			getline(clientFile,  line);
			std::string tempKey = sha256(line);
			if (currentParams.clientKey.compare(tempKey) == 0) {
				tempClient = addClient(&(currentParams.clientId), &tempKey);
				newSession = addSession(tempClient);
				verified = true;
			}
			clientFile.close();
		} else {
			return false;
		}

	}

	if (newSession != NULL) {
		sendToSocket("{\n\t\"Session\":" + toString(newSession->sessionId) + "\n}");
	} else {
		sendToSocket("Error");
	}

	return verified;

}

