#ifndef _GLOBALMOTIONESTIMATION_HPP
#define _GLOBALMOTIONESTIMATION_HPP

#include <iostream>
#include <fstream>
#include <string>
#include <memory>

#include <io.h>
#include <math.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include "inputFilePath.hpp"
#include "flowFile.hpp"
#include "GMEParameter.hpp"

void calculateRatio(const std::string preIMGPath, const std::string curIMGPath, float &hRatio, float &wRatio);
float defineMinority(cv::Mat &STGS);
void showmask(cv::Mat data);
void geneuratePosition(cv::Mat &pos, int h, int w, bool rowflag);

class GMEParameter;

/*class globalMotionEstimation:
	calculate motionvector from input path and get the GME parameters
	Initiated with the rootPath and a ratio for downsampling*/
class globalMotionEstimation
{
public:
	friend class GMEParameter;

	globalMotionEstimation(const std::string s, const int r = 1);
	~globalMotionEstimation() = default;
	
	void calculateParameter();

	std::vector<cv::Mat> parameter;
	std::vector<cv::Mat> mask;

private:
	int initial();
	void calculateMotionVector(const int i);
	std::shared_ptr<GMEParameter> checkRatio(const int i);

	inputFilePath inputIMG;
	inputFilePath inputFlow;
	std::shared_ptr<flowFile> flow;
	std::shared_ptr<GMEParameter> GMEModel;

	unsigned int nbframes;
	int height = 0, width = 0, ratio;
	std::string rootPath;
	cv::Mat xPos, yPos;
};

#endif