#ifndef _FINDFILEPATH_HPP
#define _FINDFILEPATH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <io.h>

// inputFilePath(const std::string inputDir, const std::string fileType)
// input: inputDir that contains the searched files, fileType, defined the file type
// Check the inputDir also the subDir and add the files' names and files' pathes to the map 'filePath' and the vector 'fileName'
class inputFilePath
{
public:
	inputFilePath(const std::string s, const std::string t);
	~inputFilePath() = default;
	
	std::map<std::string, std::string> filePath;
	std::vector<std::string> fileName;

private:
	// Check the input directory and save the information of the file in defined file type into 'filePath' and 'fileName'
	int getFileInfo(const std::string &s);
	// Check the directory path, change '\' to '/' and add '/' to the end if it not exist
	void checkInPutDir(std::string &s);
	std::string inputDir;
	std::string fileType;
};

#endif