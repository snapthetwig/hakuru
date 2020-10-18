
struct Client {
	std::string clientId;
	std::string clientKey;

	struct Client* start = NULL;
	struct Client* next = NULL;
	struct Client* prev = NULL;

	struct Session* session = NULL;

	bool ready = false;
} *currentClient = NULL;

struct Session {
	struct Session* start = NULL;
	struct Session* prev = NULL;
	struct Session* next = NULL;

	struct timeval lastTransactionTime;
	struct Client* client = NULL;

	unsigned long sessionId;
	struct sockaddr_in sessionAddress;
};

struct Server {
	int socksOpened;
	int sock;
	int newSock;
	struct sockaddr_in address;
	struct sockaddr_in clientAddress;
	socklen_t sinSize;
	char lastRead[1023];
	int readSize;
	int sockCounter;
	int lastSock;

	struct timeval timeout;
	fd_set masterSet, workingSet;
} currentServer;

struct KeyValue {
	std::string key;
	std::string value;
};

struct Params {
	std::string clientId;
	std::string clientKey;
	unsigned long sessionId;
	std::string event;
	unsigned int sessionKey;
} currentParams;

//////

void sendToSocket (std::string);

std::string invalidString("{\n\t\"Status\":\"Invalid\"\n}");

std::string errorString("{\n\t\"Status\":\"Error\"\n}");

std::string storedString("{\n\t\"Status\":\"Stored\"\n}");

std::string successString("{\n\t\"Status\":\"Success\"\n}");

