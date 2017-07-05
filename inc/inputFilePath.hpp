#ifndef _INPUTFILEPATH_HPP
#define _INPUTFILEPATH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <io.h>

void checkInPutDir(std::string &s);

// inputFilePath(const std::string inputDir, const std::string fileType)
// input: inputDir that contains the searched files, fileType, defined the file type
// Check the inputDir also the subDir and add the files' names and files' pathes to the map 'filePath' and the vector 'fileName'
class inputFilePath
{
public:
	inputFilePath() = default;
	inputFilePath(const std::string s, const std::string t) : inputDir(s), fileType(t) { }
	~inputFilePath() = default;

	// Initial the class 'inputFilePath'
	// return value:
	// 0 - normal, 1 - failed
	int initial(bool flag);
	
	std::map<std::string, std::string> filePath;
	std::vector<std::string> fileName;
	std::string inputDir;
	std::string fileType;
	bool CHECKSUBDIR = true;
private:
	// Check the input directory and save the information of the file in defined file type into 'filePath' and 'fileName'
	int getFileInfo(const std::string &s);
	// Check the directory path, change '\' to '/' and add '/' to the end if it not exist
};

#endif