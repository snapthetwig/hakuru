std::string toString(unsigned long passedLong) {
	std::ostringstream oss;
	oss << passedLong;
	return oss.str();
}

void chomp(std::string* passedString) {
	int pos;
	while ((pos = passedString->find("\n")) != std::string::npos) {
		passedString->erase(pos);
	}
	while ((pos = passedString->find("\r")) != std::string::npos) {
		passedString->erase(pos);
	}
}

void lowercase (std::string* passedString) {

	for (auto& c : *(passedString)) {
		c = std::tolower(c);
	}

}

std::string sha256(const std::string str) {
	unsigned char hash[SHA256_DIGEST_LENGTH];
	SHA256_CTX sha256;
	SHA256_Init(&sha256);
	SHA256_Update(&sha256, str.c_str(), str.size());
	SHA256_Final(hash, &sha256);
	std::stringstream ssss;
	for (int i = 0; i < SHA256_DIGEST_LENGTH; i++) {
		ssss << std::hex << std::setw(2) << std::setfill('0') << (int)hash[i];
	}
	return ssss.str();
}
