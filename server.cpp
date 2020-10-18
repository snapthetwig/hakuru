
#include <unistd.h> 
#include <stdio.h> 
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/time.h>
#include <errno.h> 
#include <string.h> 
#include <string> 
#include <sys/types.h> 
#include <netinet/in.h> 
#include <sys/socket.h> 
#include <sys/wait.h> 
#include <arpa/inet.h>
#include <fcntl.h> 
#include <errno.h>
#include <iostream>
#include <filesystem>
#include <fstream>
#include <openssl/sha.h>
#include <sstream>
#include <iomanip>
#include <sstream>

#include "global.h"
#include "network/socket.h"

#include "misc.cpp"

#include "storage/clients.cpp"

#include "network/http.cpp"
#include "network/socket.cpp"

int main() {
	listener();
	return 1;
}

