
bool isHttp (std::string* passedString) {

	if (passedString->find("get") == 0) {
		return true;
	}

	return false;

}

void splitKeyValue(std::string passedString) {

	std::cout << passedString << "\n";
	int foundPosition;
	foundPosition = passedString.find("=");
	struct KeyValue returnKeyValue;
	if (foundPosition == std::string::npos) {
	} else {

		returnKeyValue.key = passedString.substr(0, foundPosition);
		returnKeyValue.value = passedString.substr((foundPosition + 1), (passedString.length() - foundPosition));
		if (returnKeyValue.key.compare("id") == 0) {
			currentParams.clientId = returnKeyValue.value;
			std::cout << "ID ==> " << currentParams.clientId << "\n";
		}
		if (returnKeyValue.key.compare("key") == 0) {
			currentParams.clientKey = returnKeyValue.value;
			std::cout << "Key ==> " << currentParams.clientKey << "\n";
		}
		if (returnKeyValue.key.compare("session") == 0) {
			currentParams.sessionId = std::stol(returnKeyValue.value);
		}
		if (returnKeyValue.key.compare("event") == 0) {
			currentParams.event = returnKeyValue.value;
		}

	}

}

struct Storage {
	struct Client* client = NULL;
	struct Session* session = NULL;

} *eventStore;

bool storeEvent() {

	bool stored = true;

	struct Client* thisClient;
	struct Session* thisSession;

	pid_t pid;

	thisClient = clientExists(&(currentParams.clientId), &(currentParams.clientKey));
	if (thisClient == NULL) {
		stored = false;
	}

	thisSession = getSession(thisClient, currentParams.sessionId);
	if (thisSession == NULL) {
		thisSession = addSession(thisClient);
	}

	if (pid = fork(); pid == 0) {

	} else {

		persistEvents(thisClient, thisSession);
		exit(0);

	}

	if (stored) {
		sendToSocket("{\n\t\"Session\":" + toString(thisSession->sessionId) + "\n}");
	} else {
		sendToSocket("Error\n");
	}
	
	return true;

}

void handleRequest (std::string* passedString) {

	int foundCount = 0;
	int currentPosition = 0, previousPosition = -1;
	bool found = true;
	struct KeyValue returned;

	std::cout << "handleRequest \n";

	while (found) {

		currentPosition = passedString->find("&", (previousPosition + 1));
		if (currentPosition == std::string::npos) {

			splitKeyValue(passedString->substr((previousPosition + 1), (passedString->length() - previousPosition)));
			found = false;

		} else {

			splitKeyValue(passedString->substr((previousPosition + 1), ((currentPosition - 1) - previousPosition)));

		}
		previousPosition = currentPosition;

	}
	std::cout << "handleRequest done \n";

}

void interpretHttp (std::string* passedString) {

	int begin, inter, end;
	std::string url, query;

	if (isHttp(passedString)) {

		begin = passedString->find(" ", 0);

		if (begin == std::string::npos) {

			sendToSocket(invalidString);

		} else {

			inter = passedString->find("?", (begin + 1));

			if (inter == std::string::npos) { // no params

				/*
				end = passedString->find(" ", (begin + 1));

				if (end == std::string::npos) {

					url = passedString->substr((begin + 1), (passedString->length() - begin));

				} else {

					url = passedString->substr((begin + 1), ((end - 1) - begin));

				}
				*/
				sendToSocket(invalidString);

			} else { // found some params
				
				end = passedString->find(" ", (inter + 1));

				if (end == std::string::npos) {

					//url = passedString->substr((begin + 1), ((inter - 1) - begin));
					query = passedString->substr((inter + 1), (passedString->length() - inter));

				} else {

					//url = passedString->substr((begin + 1), ((inter - 1) - begin));
					query = passedString->substr((inter + 1), ((end - 1) - inter));

				}

				chomp(&query);
				handleRequest(&query);

				struct Client* tempClient;

				if ((! currentParams.clientId.empty()) && currentParams.sessionId > 0 && (! currentParams.event.empty()) && (! currentParams.clientKey.empty())) {

					tempClient = verifyClient();

					if (tempClient == NULL) {

						sendToSocket(errorString);

					} else {

						storeEvent();

					}

				}

			}

		}
		
	} else {

		sendToSocket(invalidString);

	}

}

