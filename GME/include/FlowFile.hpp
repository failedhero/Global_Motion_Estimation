#ifndef FLOWFILE_HPP
#define FLOWFILE_HPP

#include <io.h>
#include <iostream>
#include <fstream>
#include <string>

#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

class FlowFile
{
public:
	FlowFile(const std::string s) : inputPath(s) { }
	~FlowFile() = default;

	int initial();
	void changeSize(int r);

	cv::Mat uFlow, vFlow;
	int height, width;
private:
	std::string inputPath;
	float tag;
};

#endif