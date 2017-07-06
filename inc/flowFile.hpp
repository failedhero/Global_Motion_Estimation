#ifndef _FLOWFILE_HPP
#define _FLOWFILE_HPP

#include <io.h>
#include <iostream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class flowFile
{
public:
	flowFile(const std::string s) : inputPath(s) { }
	~flowFile() = default;

	int initial();
	void changeSize(int r);

	cv::Mat uFlow, vFlow;
	int height, width;
private:
	std::string inputPath;
	float tag;
};

#endif