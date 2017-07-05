#ifndef _FINDVIDEOPATH_HPP
#define _FINDVIDEOPATH_HPP

#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <io.h>

class videoInputPath
{
public:
	videoInputPath() = default;
	videoInputPath(const std::string s, const std::string t);
	~videoInputPath() = default;
	
	std::map<std::string, std::string> videoPath;
	std::vector<std::string> videoName;

private:
	// Check the video directory and save the information of the file in defined file type into 'videoPath' and 'videoName'
	int getVideoInfo(const std::string &s);
	// Check the directory path, change '\' to '/' and add '/' to the end if it not exist
	void checkinPutPath(std::string &s);
	std::string videoDir;
	std::string fileType;
};

#endif