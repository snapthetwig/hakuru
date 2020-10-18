
bool fileExists (std::string passedFile) {

	return std::filesystem::exists(dataPath + passedFile);

}

